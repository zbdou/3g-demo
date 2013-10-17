#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <osmocom/core/socket.h>

#include "fp_entity.h"

#define TX_NOT_ALLOWED(fpe) (fpe->txrx_queue->qevent == Q_DESTROY)

/*
  return SUCCESS or FAILURE  
 */
int setup_socket(fp_entity *fpe)
{
	fpe->sock_fd = osmo_sock_init(AF_INET, SOCK_DGRAM, IPPROTO_UDP,
								  fpe->self.addr, fpe->self.port, OSMO_SOCK_F_BIND);
	if(fpe->sock_fd < 0) return FAILURE;
	return SUCCESS;
}

void close_socket(fp_entity *fpe)
{
	close(fpe->sock_fd);
}
