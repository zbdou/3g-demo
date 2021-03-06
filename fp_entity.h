#ifndef _FP_ENTITY_H_
#define _FP_ENTITY_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <semaphore.h>

#include "msgb.h"
#include "thread.h"
#include "queue.h"
#include "threadpool.h"

#define _INIT_FP_ENTITY(fpe) (memset(fpe, 0, sizeof(*fpe)))
#define MAKE_TX_ONLY_FP_ENTITY(fpe, self_addr, self_port, peer_addr, peer_port, cbr, cbf, cba) { \
		_INIT_FP_ENTITY(fpe);											\
		fpe->mode = TX_ONLY;											\
		_MAKE_FP_ENTITY(fpe, self_addr, self_port, peer_addr, peer_port, cbr, cbf, cba); \
	}

#define MAKE_RX_ONLY_FP_ENTITY(fpe, self_addr, self_port, peer_addr, peer_port, cbr, cbf, cba) { \
	_INIT_FP_ENTITY(fpe);												\
	fpe->mode = RX_ONLY;												\
	_MAKE_FP_ENTITY(fpe, self_addr, self_port, peer_addr, peer_port, cbr, cbf, cba); \
	}

#define MAKE_TRX_FP_ENTITY(fpe, self_addr, self_port, peer_addr, peer_port, cbr, cbf, cba) { \
	_INIT_FP_ENTITY(fpe);												\
	fpe->mode = TRX;													\
	_MAKE_FP_ENTITY(fpe, self_addr, self_port, peer_addr, peer_port, cbr, cbf, cba); \
	}

#define _MAKE_FP_ENTITY(fpe, self_addr, self_port, peer_addr, peer_port, cbr, cbf, cba) { \
	strcpy(fpe->self.addr, self_addr);									\
	fpe->self.port = self_port;											\
	strcpy(fpe->peer.addr, peer_addr);									\
	fpe->peer.port = peer_port;											\
	fpe->cbreceived = cbr;												\
	fpe->cbflushed = cbf;												\
	fpe->cbacked = cba;													\
	}

#define SUCCESS (1)
#define FAILURE (0)


#define MAX_NODE_ADDR (100)

typedef struct {
	char addr[MAX_NODE_ADDR];
	uint16_t port;
} fp_node_info;


typedef enum {
	TX_ONLY = 0,				/* transmitter */
	RX_ONLY,					/* receiver */
	TRX							/* transceiver */
} fp_MODE;


/*
  represent the RLC/MAC block
 */
typedef struct {
	size_t data_len;
	char *data;
} data_block;


/*
  NOTE: void *fpe should be fp_entity *fpe
 */
typedef void*(*cb_received)(void *fpe, struct msgb* msg);
typedef cb_received cb_flushed;
typedef cb_received cb_acked;

typedef struct {
	fp_MODE mode;
	fp_node_info self;
	fp_node_info peer;

	cb_received cbreceived;		/* callback function: received pkt */
	cb_flushed cbflushed;		/* callback function: flushed */
	cb_acked cbacked;

	int sock_fd;

	sem_t wait_receiver;
	sem_t wait_queue_manager;

	fp_threadsafe_queue txrx_queue;

} fp_entity;

/*
  init a FP ENTITY,
  return SUCCESS or FAILURE
 */
extern int fp_entity_init(fp_entity *fpe, threadpool_t* tpool);

/*
  destroy a FP ENTITY
  return SUCCESS or FAILURE
 */
extern int fp_entity_destroy(fp_entity* fpe);

/*
  send an RLC/MAC data block
  return SUCCESS or FAILURE  
 */
extern int fp_entity_send(fp_entity* fpe, data_block *db);

#endif	/* _FP_ENTITY_H_ */
