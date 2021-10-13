#ifndef __RPCH_SERVER_H_
#define __RPCH_SERVER_H_
#include <stdint.h>

#include "error.h"
#include "threadpool.h"

typedef struct server {
    int lfd;
    int epoll_fd;
    uint16_t port;
    uint32_t ip;
    threadpool_t* pool;
} server_t;

struct server* server_create();
void server_listen(struct server* svr, const char* addr, error_t* err);
void server_register();
void server_destroy(struct server*);

#endif
