#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "msgb.h"
#include <osmocom/core/talloc.h>


void *tall_msgb_ctx;


/*
  put @data with len = @datalen to the msgb struct;
 */
struct msgb *msgb_alloc(const char *name, unsigned char *data, uint16_t datalen)
{
	struct msgb *msg;

	msg = _talloc_zero(tall_msgb_ctx, sizeof(*msg), name);

	if (!msg) {
		//LOGP(DRSL, LOGL_FATAL, "unable to allocate msgb\n");
		return NULL;
	}

	msg->data_len = datalen;
	msg->len = msg->data_len;

	msg->data = data;
	msg->head = msg->_data;
	msg->tail = msg->_data;

	return msg;
}

/*
   !! won't free the msgb->data
 */
void msgb_free(struct msgb *m)
{
	talloc_free(m);
}


void msgb_enqueue(struct llist_head *queue, struct msgb *msg)
{
	llist_add_tail(&msg->list, queue);
}

struct msgb *msgb_dequeue(struct llist_head *queue)
{
	struct llist_head *lh;

	if (llist_empty(queue))
		return NULL;

	lh = queue->next;
	llist_del(lh);
	
	return llist_entry(lh, struct msgb, list);
}

void msgb_reset(struct msgb *msg)
{
	msg->len = 0;
	msg->data = msg->_data;
	msg->head = msg->_data;
	msg->tail = msg->_data;
}

uint8_t *msgb_data(const struct msgb *msg)
{
	return msg->data;
}

uint16_t msgb_length(const struct msgb *msg)
{
	return msg->len;
}

void msgb_set_talloc_ctx(void *ctx)
{
	tall_msgb_ctx = ctx;
}


