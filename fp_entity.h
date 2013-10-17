#ifndef _FP_ENTITY_H_
#define _FP_ENTITY_H_

#include <stdint.h>

#define MAKE_TX_ONLY_FP_ENTITY(fpe) ()
#define MAKE_RX_ONLY_FP_ENTITY(fpe) ()
#define MAKE_TRX_FP_ENTITY(fpe) ()

#define SUCCESS (1)
#define FAILURE (0)


#define MAX_NODE_ADDR (100)

typedef struct {
	char *addr[MAX_NODE_ADDR];
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


struct fp_entity_t;
typedef void*(*cb_received)(fp_entity_t *fpe, data_block *db);
typedef cb_received cb_flushed;
typedef cb_received cb_acked;

typedef struct {
	
	
} fp_entity_cb_funcs;

typedef struct fp_entity_t {
	fp_MODE mode;
	fp_node_info self;
	fp_node_info peer;

	
	int sock_fd;
} fp_entity;


/*
  init an FP ENTITY,
  return SUCCESS or FAILURE
 */
extern int fp_entity_init(fp_entity *fpe);

#endif	/* _FP_ENTITY_H_ */
