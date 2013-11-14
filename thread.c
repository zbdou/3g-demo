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

void queue_manager_thread_func(void* arg)
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
				msgb_free(msg);
				break;
			}
			case Q_RX_PKT: {
				printf("receiver_thread_func pkt\n");
				fpe->cbreceived(fpe, msg);
				
				/* free msg */
				msgb_free(msg);
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
		msgb_free(msg2);
	}
	sem_post(&fpe->wait_queue_manager);
}

#define BUFFER_SIZE (1500)
void receiver_thread_func(void* arg)
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
	sem_post(&fpe->wait_receiver);
}
