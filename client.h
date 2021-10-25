#ifndef __RPCH_CLIENT_H_
#define __RPCH_CLIENT_H_
#include <pthread.h>
#include <stdint.h>
#include <string.h>

#include "argument.h"
#include "buffer.h"
#include "conn.h"
#include "error.h"

struct client_request {
    char* service;
    char* method;
    struct argument* args;
    int args_cnt;
};

static inline void client_request_init(struct client_request* req,
                                       char* service, char* method,
                                       int arg_cnt) {
    req->args =
        arg_cnt ? (struct argument*)malloc(sizeof(struct argument) * arg_cnt)
                : NULL;
    req->args_cnt = arg_cnt;
    req->service = service;
    req->method = method;
}

typedef struct client {
    buffer_t* buff;
    int64_t seq;
    pthread_mutex_t seq_lock;
    error_t err;
    char buf[24];
    char resp_hbuf[16];
} client_t;

struct client* client_dial(const char* addr, error_t* err);
void client_destroy(client_t*);
// return seq
uint64_t client_call(client_t* cli, struct client_request* req,
                     struct argument* resp);
static inline int client_failed(client_t* cli) { return !cli->err.null; }
static inline char* client_failed_msg(client_t* cli) { return cli->err.msg; }

#endif
