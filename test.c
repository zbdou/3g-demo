/*
 * Author: zbdou
 * Date: 10/17/2013
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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


int main(void)
{
	int max_entity_alloced = 100000;
	fp_entity* fpe = NULL;
	
	while (max_entity_alloced-- > 0)
	{
		fpe = (fp_entity*)malloc(sizeof(fp_entity));
		if(fpe) {
			MAKE_TRX_FP_ENTITY((fpe), "localhost", 5001, "127.0.0.1", 5002, cbr, cbf, NULL);
			if( fp_entity_init(fpe) == FAILURE ) {
				printf("fp_entity_init failed\n");
				return 0;
			}
			usleep(10);
			fp_entity_destroy(fpe);
		}
		free(fpe);
		fpe = NULL;
	}
	
	/* fp_entity fpentity; */
	/* MAKE_TRX_FP_ENTITY((&fpentity), "localhost", 5001, "127.0.0.1", 5002, cbr, cbf, NULL); */
	/* // MAKE_TRX_FP_ENTITY((&fpentity), "localhost", 5002, "127.0.0.1", 5001, cbr, cbf, NULL); */

	/* if( fp_entity_init(&fpentity) == FAILURE ) { */
	/* 	printf("fp_entity_init failed\n"); */
	/* 	return 0; */
	/* } */

	/* int max_pkt_sent = 1000; */
	/* char data[100] = "zbdou.udp.text"; */
	/* data_block db = { */
	/* 	strlen(data), */
	/* 	data */
	/* }; */


	/* while(max_pkt_sent--) { */
	/* 	fp_entity_send(&fpentity, &db); */
	/* 	usleep(100); */
	/* } */



	/* sleep(10); */
	/* fp_entity_destroy(&fpentity); */

	return 0;
}
