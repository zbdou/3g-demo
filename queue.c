#include <assert.h>
#include <string.h>

#include "queue.h"

void queue_init(fp_threadsafe_queue *q)
{
	
	memset(q, 0, sizeof(*q));

	INIT_LLIST_HEAD(&q->qlist);

	int s = pthread_mutex_init(&q->qlock, NULL);
	assert(!s);
	s = pthread_cond_init(&q->qsignal, NULL);
	assert(!s);

	q->qevent = Q_NORMAL;
}

void queue_destroy(fp_threadsafe_queue *q)
{
	pthread_cond_destroy(&(q->qsignal));
	assert(q->qevent == Q_DESTROY);
}

int queue_size(fp_threadsafe_queue *q)
{
	return q->current_len;
}

void queue_destroy_notify(fp_threadsafe_queue *q, int notify)
{
	q->qevent = Q_DESTROY;

	if (notify) {
		/* wake up the queue_manager thread */
		pthread_mutex_lock(&q->qlock);
		pthread_cond_signal(&q->qsignal);
		pthread_mutex_unlock(&q->qlock);
	}
}





