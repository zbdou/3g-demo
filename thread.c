#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include "fp_entity.h"
#include "msgb.h"
#include "thread.h"

void thread_init(fp_thread *th, size_t stack_size, task func, void* arg)
{
	th->thread = 0;
	pthread_attr_init(&th->attrib);
	th->stack_size = stack_size;
	th->thread_task = func;
	th->thread_arg = arg;
}

void thread_destroy(fp_thread *th)
{
	pthread_attr_destroy(&th->attrib);
}

void thread_join(fp_thread *th)
{
	int s = pthread_join(th->thread, NULL);
	assert(!s);
	th->thread = 0;
}

void thread_start(fp_thread *th)
{
	assert(th->thread == 0);
	int s = pthread_attr_setstacksize(&th->attrib, th->stack_size);
	assert(!s);
	s = pthread_create(&th->thread, &th->attrib, th->thread_task, th->thread_arg);
	assert(!s);
}

void* queue_manager_thread_func(void* arg)
{
	fp_entity* fpe = (fp_entity*) arg;
	assert(fpe);
	struct msgb *msg2 = NULL;
	struct sockaddr_in dest_addr;
	int addr_len = sizeof(struct sockaddr_in);

	/* setup the dest node context for TX */
	bzero(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(fpe->peer.port);
	dest_addr.sin_addr.s_addr = inet_addr(fpe->peer.addr);

	while (queue_event(&fpe->txrx_queue) != Q_DESTROY) {

		struct msgb* msg = msgb_queue_read(&fpe->txrx_queue);
		if (queue_event(&fpe->txrx_queue) == Q_DESTROY) break;

		if (msg != NULL) {
			switch(msg->event) {
			case Q_TX_PKT: {
				/* send pkt out, call socket func. sendto(...) */
				int ret;
				ret = sendto(fpe->sock_fd, msg->data, msg->data_len, 0,
							 (struct sockaddr*) &dest_addr, addr_len);

				printf("send pkt out, ret = %d\n", ret);
					
				/* free msg */
				free(msg);
				break;
			}
			case Q_RX_PKT: {
				printf("receiver_thread_func pkt\n");
				fpe->cbreceived(fpe, msg);
				
				/* free msg */
				free(msg);
				break;
			}
			default:
				break;
			} /* switch... */
		}
	} /* while... */

	/* if txrx_queue is not empty, flush it */
	while( msg2 = msgb_dequeue(&fpe->txrx_queue.qlist), msg2 ) {
		// printf("msg2 = %0x\n", msg2);
		fpe->txrx_queue.current_len--;
		if(fpe->cbflushed)
			fpe->cbflushed(fpe, msg2);
		/* free msg2 */
		free(msg2);
	}
	return ((void*)0);
}

#define BUFFER_SIZE (1500)
void* receiver_thread_func(void* arg)
{
	fp_entity* fpe = (fp_entity*) arg;
	assert(fpe);

	int ret;
	char recvbuf[BUFFER_SIZE];
	struct sockaddr_in client_addr;
	socklen_t client_len = (socklen_t) sizeof(struct sockaddr_in);

	while (ret = recvfrom(fpe->sock_fd, recvbuf, sizeof(recvbuf), 0,
						  (struct sockaddr*) &client_addr, &client_len), ret != -1) {

		if(queue_event(&fpe->txrx_queue) == Q_DESTROY) break;

		/*
		  put the received msg into the txrx_queue and
		  wakeup the queue_manager thread
		*/
		{
			recvbuf[ret] = '\0';
			struct msgb* msg = msgb_alloc("data_block", recvbuf, ret, Q_RX_PKT);
			msgb_queue_write(&fpe->txrx_queue, msg);
		}
	} /* while... */
	return ((void*)0);
}
