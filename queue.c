#include <assert.h>
#include <string.h>

#include "queue.h"

void queue_init(fp_threadsafe_queue *q, fp_QUEUE_EVENT qtype)
{
	memset(q, 0, sizeof(*q));

	int s = pthread_mutex_init(&q->qlock, NULL);
	assert(!s);
	s = pthread_cond_init(&q->qsignal, NULL);
	assert(!s);

	q->qevent = qtype;
	assert(q->qevent != Q_DESTROY);
}

void queue_destroy(fp_threadsafe_queue *q)
{
	pthread_cond_destroy(&(q->qsignal));
	assert(q->qevent == Q_DESTROY);
}

size_t queue_size(fp_threadsafe_queue *q)
{
	return q->current_len;
}

void queue_destroy_notify(fp_threadsafe_queue *q)
{
	q->qevent = Q_DESTROY;

	/* wake up the queue_manager thread */
	pthread_mutex_lock(&q->qlock);
	pthread_cond_signal(&q->qsignal);
	pthread_mutex_unlock(&q->qlock);
}
