/*
 * Author: zbdou
 * Date: 10/17/2013
 */

#include <stdio.h>

#include "test.h"

/*
  we use test.c main(...) to test FP Entity functionalites.
 */

void* cbr(void* fpe, struct msgb* msg)
{
	printf("cbr.......\n");
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
	INIT_FP_ENTITY(&fpentity);

	fpentity.mode = RX_ONLY;

	sprintf(fpentity.self.addr, "%s", "localhost");
	fpentity.self.port = 10001;

	sprintf(fpentity.peer.addr, "%s", "localhost");
	fpentity.peer.port = 10002;
	
	fpentity.cbreceived = cbr;
	fpentity.cbflushed = cbf;

	fp_entity_init(&fpentity);

	fp_entity_destroy(&fpentity);
	return 0;
}
