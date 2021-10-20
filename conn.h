#ifndef __RPCH_CONN_H_
#define __RPCH_CONN_H_

#include <netinet/in.h>
#include <stdint.h>

typedef struct conn conn_t;

conn_t* conn_new(int cfd, struct sockaddr_in* addr);
void conn_destroy(conn_t*);
struct addr_v4 conn_remote_addr(conn_t*);
int conn_read(conn_t* conn, char* buf, int size);
int conn_write(conn_t* conn, char* data, int size);

#endif
