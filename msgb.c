#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "msgb.h"
#include <osmocom/core/talloc.h>


void *tall_msgb_ctx;


/*
  put @data with len = @datalen to the msgb struct;
 */
struct msgb *msgb_alloc(const char *name, char *data, uint16_t datalen, fp_QUEUE_EVENT e)
{
	struct msgb *msg;

#ifndef DEBUG_MEM
	msg = _talloc_zero(tall_msgb_ctx, sizeof(*msg), name);
#else
	msg = (struct msgb*) malloc(sizeof(struct msgb));
#endif

	if (!msg) {
		//LOGP(DRSL, LOGL_FATAL, "unable to allocate msgb\n");
		return NULL;
	}

	msg->data_len = datalen;
	msg->len = msg->data_len;

	msg->data = data;
	msg->head = msg->data;
	msg->tail = msg->data + datalen;

	msg->event = e;

	return msg;
}

/*
   !! won't free the msgb->data
 */
void msgb_free(struct msgb *m)
{
#ifndef DEBUG_MEM
	talloc_free(m);
#else
	free(m);
#endif

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
	msg->data = NULL;
	msg->head = NULL;
	msg->tail = NULL;
}

uint8_t *msgb_data(const struct msgb *msg)
{
	return (uint8_t*) (msg->data);
}

uint16_t msgb_length(const struct msgb *msg)
{
	return msg->len;
}

void msgb_set_talloc_ctx(void *ctx)
{
	tall_msgb_ctx = ctx;
}



struct msgb* msgb_queue_read(fp_threadsafe_queue *q)
{
	pthread_mutex_lock(&q->qlock);

	struct msgb *msg = msgb_dequeue(&q->qlist);
	while( msg == NULL && q->qevent != Q_DESTROY) {
		pthread_cond_wait(&q->qsignal, &q->qlock);
		msg = msgb_dequeue(&q->qlist);
	}

	if(msg != NULL) q->current_len--;

	/*
	  msg may be NULL, since q->qevent may be changed to Q_DESTROY,
	  the caller should check the return value!
	 */
	pthread_mutex_unlock(&q->qlock);
	return msg;
}

void msgb_queue_write(fp_threadsafe_queue *q, struct msgb* msg)
{
	pthread_mutex_lock(&q->qlock);

	msgb_enqueue(&q->qlist, msg);
	q->current_len++;
	pthread_cond_signal(&q->qsignal);

	pthread_mutex_unlock(&q->qlock);
}
