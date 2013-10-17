#include <assert.h>

#include "thread.h"

void thread_init(fp_thread *th, size_t stack_size, task func, void* arg)
{
	th->thread = 0;
	pthread_attr_init(&th->attrib);
	th->stack_size = stack_size;
	th->thread_task = func;
	th->thread_arg = arg;
}

void thread_destroy(fp_thread *th)
{
	pthread_attr_destroy(&th->attrib);
}

void thread_join(fp_thread *th)
{
	int s = pthread_join(th->thread, NULL);
	assert(!s);
	th->thread = 0;
}

void thread_start(fp_thread *th)
{
	assert(th->thread == 0);
	int s = pthread_attr_setstacksize(&th->attrib, th->stack_size);
	assert(!s);
	s = pthread_create(&th->thread, &th->attrib, th->thread_task, th->thread_arg);
	assert(!s);
}

void* queue_manager_thread_func(void* arg)
{
	return ((void*)0);
}

void* receiver_thread_func(void* arg)
{
	return ((void*)0);
}
