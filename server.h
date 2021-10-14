#ifndef __RPCH_SERVER_H_
#define __RPCH_SERVER_H_
#include <stdint.h>

#include "argument.h"
#include "error.h"
#include "hashmap.h"
#include "request.h"
#include "threadpool.h"

typedef struct server {
    int lfd;
    int epoll_fd;
    uint16_t port;
    uint32_t ip;
    threadpool_t* pool;
    hashmap_t* services;
} server_t;

struct server* server_create();
void server_listen(struct server* svr, const char* addr, error_t* err);
void server_destroy(struct server*);
static inline void server_register(server_t* svr, char* service_method,
                                   void (*handler)(request_t* req, error_t* err,
                                                   struct argument* resp)) {
    hashmap_set(svr->services, service_method, (void*)handler);
}

#endif
