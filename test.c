/*
 * Author: zbdou
 * Date: 10/17/2013
 */

#include <stdio.h>
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
	fp_entity fpentity;
	MAKE_TX_ONLY_FP_ENTITY((&fpentity), "localhost", 5001, "127.0.0.1", 5002, cbr, cbf, NULL);

	if( fp_entity_init(&fpentity) == FAILURE ) {
		printf("fp_entity_init failed\n");
		return 0;
	}

	int max_pkt_sent = 1;
	char data[100] = "zbdou.udp.text";
	data_block db = {
		strlen(data),
		data
	};
	
	while(max_pkt_sent--) {
		fp_entity_send(&fpentity, &db);
		sleep(1);
	}

	sleep(1);
	fp_entity_destroy(&fpentity);

	return 0;
}
