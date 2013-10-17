#ifndef _MSGB_H
#define _MSGB_H

#include <stdint.h>
#include <osmocom/core/linuxlist.h>
#include <osmocom/core/utils.h>

#include "queue.h"

/*! \defgroup msgb Message buffers
 *  @{
 */

/*! \file msgb.h
 *  \brief Osmocom message buffers
 * The Osmocom message buffers are modelled after the 'struct skb'
 * inside the Linux kernel network stack.  As they exist in userspace,
 * they are much simplified.  However, terminology such as headroom,
 * tailroom, push/pull/put etc. remains the same.
 */

#define MSGB_DEBUG

/*! \brief Osmocom message buffer */
struct msgb {
	struct llist_head list; /*!< \brief linked list header */

	uint16_t data_len;   /*!< \brief length of underlying data array */
	uint16_t len;	     /*!< \brief length of bytes used in msgb */

	unsigned char *head;	/*!< \brief start of underlying memory buffer */
	unsigned char *tail;	/*!< \brief end of message in buffer */
	unsigned char *data;	/*!< \brief start of message in buffer */

	fp_QUEUE_EVENT event;
};

extern struct msgb *msgb_alloc(const char *name, unsigned char *data, uint16_t datalen, fp_QUEUE_EVENT e);
extern void msgb_free(struct msgb *m);
extern void msgb_enqueue(struct llist_head *queue, struct msgb *msg);
extern struct msgb *msgb_dequeue(struct llist_head *queue);
extern void msgb_reset(struct msgb *m);
uint16_t msgb_length(const struct msgb *msg);




extern struct msgb* msgb_queue_read(fp_threadsafe_queue *q);
extern void msgb_queue_write(fp_threadsafe_queue *q, struct msgb* msg);



#ifdef MSGB_DEBUG
#include <osmocom/core/panic.h>
#define MSGB_ABORT(msg, fmt, args ...) do {		\
	osmo_panic("msgb(%p): " fmt, msg, ## args);	\
	} while(0)
#else
#define MSGB_ABORT(msg, fmt, args ...)
#endif

/* non inline functions to ease binding */
uint8_t *msgb_data(const struct msgb *msg);
void msgb_set_talloc_ctx(void *ctx);

/*! @} */

#endif /* _MSGB_H */
