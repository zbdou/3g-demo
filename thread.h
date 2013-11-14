#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>


/* callback function definition */
typedef void*(*task)(void*);

typedef struct {
	pthread_t thread;
	pthread_attr_t attrib;
	size_t stack_size;

	task thread_task;
	void* thread_arg;

} fp_thread;


extern void thread_init(fp_thread *th, size_t stack_size, task func, void* arg);
extern void thread_destroy(fp_thread *th);
extern void thread_join(fp_thread *th);
extern void thread_start(fp_thread *th);

extern void queue_manager_thread_func(void* arg);
extern void receiver_thread_func(void* arg);


#endif	/* _THREAD_H_ */
