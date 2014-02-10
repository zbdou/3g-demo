/*
 * Author: zbdou
 * Date: 10/17/2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "test.h"


/*
  we use test.c main(...) to test FP Entity functionalites.
 */

void* cbr(void* fpe, struct msgb* msg)
{
	printf("cbr: %s\n", msg->data);
	return ((void*)0);
}



void* cbf(void* fpe, struct msgb* msg)
{
	printf("cbf.......\n");
	return ((void*)0);
}

#define THREAD 50
#define SIZE 8192

int main(void)
{
	int i;
	int max_entity_alloced = 200;
	fp_entity* fpe[10];

	/* create the thread pool */
	threadpool_t* pool = threadpool_create(THREAD, SIZE, 0);
	
	while (max_entity_alloced-- > 0)
	{
		for (i = 9; i >= 0; i--) {
			fpe[i] = (fp_entity*)malloc(sizeof(fp_entity));
			if(fpe[i]) {
				MAKE_TRX_FP_ENTITY((fpe[i]), "localhost", 5001+i, "127.0.0.1", 5002, cbr, cbf, NULL);
				if( fp_entity_init(fpe[i], pool) == FAILURE ) {
					printf("fp_entity_init failed\n");
					return 0;
				}
			}
		}

		usleep(10);

		for (i = 0; i < 10; i++)
		{
			fp_entity_destroy(fpe[i]);
			free(fpe[i]);
			fpe[i] = NULL;
		}
	}


	/* destroy the thread pool */
	assert(threadpool_destroy(pool, 0) == 0);

	return 0;
}
