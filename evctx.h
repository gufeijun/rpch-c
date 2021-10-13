#ifndef __RPCH_EVCTX_H_
#define __RPCH_EVCTX_H_
#include <stdlib.h>

#include "buffer.h"
#include "error.h"
#include "request.h"

struct server;

struct evctx {
    int fd;
    struct server* svr;
    int events;
    void* self;
    void (*callback)(void*);
    int magic_read;
    buffer_t* buff;
    request_t* req;
    error_t err;
};

static inline void evctx_destroy(struct evctx* ctx) {
    buffer_destroy(ctx->buff);
    if (ctx->req != NULL) {
        request_destroy(ctx->req);
    }
    free(ctx);
}

#endif
