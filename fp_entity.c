#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <assert.h>
#include <osmocom/core/linuxlist.h>
#include <osmocom/core/socket.h>

#include "fp_entity.h"
#include "msgb.h"

#define TX_NOT_ALLOWED(fpe) (fpe->txrx_queue.qevent == Q_DESTROY)

/* thread pool */
static threadpool_t* g_tpool = NULL;

/* forward declaration */
int setup_socket(fp_entity *fpe);
void close_socket(fp_entity *fpe);

/*
  init a FP ENTITY,
  return SUCCESS or FAILURE
*/
#define HAS_RX(fpe) (fpe->mode == RX_ONLY || fpe->mode == TRX)

/*
  this function should make itself easier to use.
 */
int fp_entity_init(fp_entity *fpe, threadpool_t* tpool)
{
	int ret;
	if (fpe == NULL || tpool == NULL) return FAILURE;

	if( g_tpool == NULL ) {
		g_tpool = tpool;
	}

	/* check if fpe->self, fpe->peer are valid */
	if(HAS_RX(fpe)) {
		ret = sem_init(&fpe->wait_receiver, 0, 0);
		if( ret != 0 ) return FAILURE;
		
		if(fpe->cbreceived == NULL)
			return FAILURE;
    }
	ret = sem_init(&fpe->wait_queue_manager, 0, 0);
	if( ret != 0 ) return FAILURE;

	/* setup socket */
	if(setup_socket(fpe) == FAILURE) return FAILURE;

	/* create && setup fpe->txrx_queue */
	queue_init(&fpe->txrx_queue);

	/* now, the receiver and the queue_manager are both started */
	if(HAS_RX(fpe)) assert(threadpool_add(g_tpool, receiver_thread_func, (void*) fpe, 0) == 0);
	assert(threadpool_add(g_tpool, queue_manager_thread_func, (void*)fpe, 0) == 0);
	
	return SUCCESS;
}


/*
  destroy a FP ENTITY
  return SUCCESS or FAILURE
*/
int fp_entity_destroy(fp_entity* fpe)
{
	if (fpe == NULL) return FAILURE;

	switch(fpe->mode) {
	case RX_ONLY:
		/* terminate fpe->receiver thread */
		queue_destroy_notify(&fpe->txrx_queue, 0);

		/* fall through */
	case TX_ONLY:
	case TRX:
		/* terminate fpe->queue_manager with txrx_queue flushed */
		queue_destroy_notify(&fpe->txrx_queue, 1);
		break;
	}

	close_socket(fpe);
	queue_destroy(&fpe->txrx_queue);

	/*
	  the txrx_queue should be flushed
	  wait until the receiver thread (if has), queue_manager thread are finished.
	 */
	if(HAS_RX(fpe)) {
		sem_wait(&fpe->wait_receiver);
		sem_destroy(&fpe->wait_receiver);
	}

	sem_wait(&fpe->wait_queue_manager);
	sem_destroy(&fpe->wait_queue_manager);

	return SUCCESS;
}


/*
  send an RLC/MAC data block
  return SUCCESS or FAILURE  
*/
int fp_entity_send(fp_entity* fpe, data_block *db)
{
	if (fpe == NULL ||
		TX_NOT_ALLOWED(fpe)) return FAILURE;

	/* go through if the qevent != Q_DESTROY */
	if (db == NULL) return FAILURE;

	/* allocate a msgb struct to accommodate this db (for TX)*/
	struct msgb* msg = msgb_alloc("data_block", db->data, db->data_len, Q_TX_PKT);
	if (msg == NULL) return FAILURE;

	/* add this msgb to the txrx_queue && wakeup queue_manager */
	msgb_queue_write(&fpe->txrx_queue, msg);

	return SUCCESS;
}


/*
  return SUCCESS or FAILURE  
*/
int setup_socket(fp_entity *fpe)
{
	fpe->sock_fd = osmo_sock_init(AF_INET, SOCK_DGRAM, IPPROTO_UDP,
								  fpe->self.addr, fpe->self.port, OSMO_SOCK_F_BIND);
	if(fpe->sock_fd < 0) return FAILURE;
	return SUCCESS;
}

void close_socket(fp_entity *fpe)
{
	/*
	  close wont work!
	  see: http://ticktick.blog.51cto.com/823160/845536
	 */
	// close(fpe->sock_fd);
	shutdown(fpe->sock_fd, SHUT_RDWR);
	close(fpe->sock_fd);
}
