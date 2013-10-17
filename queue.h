#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stddef.h>
#include <pthread.h>
#include <osmocom/core/linuxlist.h>

typedef enum {
	Q_NORMAL = 0,				/* initial status of a queue */
	Q_TX_PKT,					/* mark the msg for TX */
	Q_RX_PKT,					/* mark the msg as received (RX) */
	Q_DESTROY					/* mark this queue to be destroyed */
} fp_QUEUE_EVENT;

typedef struct {
	pthread_mutex_t qlock;
	pthread_cond_t qsignal;

	struct llist_head qlist;
	size_t current_len;

	fp_QUEUE_EVENT qevent;
} fp_threadsafe_queue;


extern void queue_init(fp_threadsafe_queue *q);
extern void queue_destroy(fp_threadsafe_queue *q);
extern void queue_destroy_notify(fp_threadsafe_queue *q, int notify);
extern size_t queue_size(fp_threadsafe_queue *q);
static inline fp_QUEUE_EVENT queue_event(fp_threadsafe_queue *q)
{
	return q->qevent;
}



#endif	/* _QUEUE_H_ */
