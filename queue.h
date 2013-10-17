#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stddef.h>
#include <pthread.h>
#include <osmocom/core/linuxlist.h>

typedef enum {
	Q_TX_PKT = 0,
	Q_RX_PKT,
	Q_DESTROY
} fp_QUEUE_EVENT;

typedef struct {
	pthread_mutex_t qlock;
	pthread_cond_t qsignal;

	struct llist_head qlist;
	size_t current_len;

	fp_QUEUE_EVENT qevent;
} fp_threadsafe_queue;


extern void queue_init(fp_threadsafe_queue *q, fp_QUEUE_EVENT qtype);
extern void queue_destroy(fp_threadsafe_queue *q);
extern size_t queue_size(fp_threadsafe_queue *q);
extern void queue_destroy_notify(fp_threadsafe_queue *q);


#endif	/* _QUEUE_H_ */
