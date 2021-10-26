#include "server.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include "addr.h"
#include "argument.h"
#include "buffer.h"
#include "error.h"
#include "evctx.h"
#include "hashmap.h"
#include "request.h"
#include "threadpool.h"

#define DEFAULT_LISTEN_CNT 128
#define DEFAULT_THREAD_CNT 5
#define MAX_EVENTS 1024
#define MAGIC 0x00686A6C
#define MAGIC_SIZE 4

typedef void (*handler_t)(request_t*, error_t*, struct argument*);

static int init_listenfd(struct server* svr) {
    int lfd, ret, opt;
    struct sockaddr_in svr_addr;

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0) return lfd;
    opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    svr->lfd = lfd;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(svr->port);
    svr_addr.sin_addr.s_addr = svr->ip;
    ret = bind(lfd, (struct sockaddr*)&svr_addr, sizeof(struct sockaddr_in));
    if (ret < 0) return ret;
    ret = listen(lfd, DEFAULT_LISTEN_CNT);
    if (ret < 0) return ret;
    return 0;
}

static struct evctx* build_evctx(int fd, struct server* svr, buffer_t* buff,
                                 void (*cb)(void*)) {
    struct evctx* ctx;

    ctx = malloc(sizeof(struct evctx));
    ctx->buff = buff;
    ctx->callback = cb;
    ctx->events = EPOLLIN;
    ctx->fd = fd;
    ctx->self = ctx;
    ctx->svr = svr;
    ctx->req = NULL;
    ctx->err = error_new();
    ctx->magic_read = 0;
    return ctx;
}

static void reset_oneshot(struct evctx* ctx) {
    struct epoll_event ev;

    ev.data.ptr = (void*)ctx;
    ev.events = EPOLLIN | EPOLLONESHOT;
    epoll_ctl(ctx->svr->epoll_fd, EPOLL_CTL_MOD, ctx->fd, &ev);
}

static void send_response(uint64_t seq, struct argument* arg,
                          struct evctx* ctx) {
    buffer_t* buff = ctx->buff;
    error_t* err = &ctx->err;

    //用户在Handler中设置错误
    if (!err->null) {
        arg->type_kind = TYPE_KIND_ERROR;
        arg->type_name_len = 0;
        arg->data_len = strlen(ctx->err.msg);
        arg->data = err->msg;
        arg->no_free = 1;
        err->null = true;
    }
    buffer_write(buff, (char*)&seq, 8, err);
    buffer_write(buff, (char*)&arg->type_kind, 2, err);
    buffer_write(buff, (char*)&arg->type_name_len, 2, err);
    buffer_write(buff, (char*)&arg->data_len, 4, err);
    buffer_write(buff, arg->type_name, arg->type_name_len, err);
    buffer_write(buff, arg->data, arg->data_len, err);
    if (!err->null) return;
    buffer_flush(buff, err);
    return;
}

static void cfd_callback(void* arg) {
    int ret;
    uint32_t magic;
    struct evctx* ctx;
    struct request* req;
    handler_t handler;
    struct argument resp;

    // read data into buffer
    ctx = (struct evctx*)arg;
    buffer_fill(ctx->buff, &ctx->err);
    if (!ctx->err.null) goto bad;

    // read magic number
    if (!ctx->magic_read) {
        if (buffer_buffered(ctx->buff) < 4) goto not_ready;
        magic = *(uint32_t*)(ctx->buff->buf + ctx->buff->front);
        if (magic != MAGIC) {
            error_put(&ctx->err, ERR_INVALID_MAGIC);
            goto bad;
        }
        buffer_drop(ctx->buff, 4);
        ctx->magic_read = 1;
    }

    // read request
    while (1) {
        ret = read_request(ctx->req, ctx->buff, &ctx->err);
        if (!ctx->err.null) goto bad;
        if (ret == 0) goto not_ready;
        req = ctx->req;

        handler =
            (handler_t)hashmap_get(ctx->svr->services, req->service_method);
        if (handler == NULL) {
            error_put(&ctx->err, ERR_BAD_REQUEST_SERVICE);
            goto bad;
        }
        argument_init(&resp);
        handler(req, &ctx->err, &resp);
        send_response(req->seq, &resp, ctx);
        argument_destroy(&resp);
        if (!ctx->err.null) goto bad;

        request_set_init_state(ctx->req);
    }

not_ready:
    reset_oneshot(ctx);
    return;
bad:
    // TODO error handling
    // printf("err :%s\n", ctx->err.msg);
    epoll_ctl(ctx->svr->epoll_fd, EPOLL_CTL_DEL, ctx->fd, NULL);
    evctx_destroy(ctx);
}

static void lfd_callback(void* arg) {
    int cfd;
    struct sockaddr_in cli_addr;
    struct epoll_event ev;
    struct buffer* buff;
    struct evctx *lfd_ctx, *ctx;

    lfd_ctx = (struct evctx*)arg;
    socklen_t cli_addr_len = sizeof(struct sockaddr_in);
    cfd = accept(lfd_ctx->svr->lfd, (struct sockaddr*)&cli_addr, &cli_addr_len);
    if (cfd < 0) perror("accept failed");
    reset_oneshot(lfd_ctx);
    conn_t* conn = conn_new(cfd, &cli_addr);

    buff = buffer_new(conn, 4096, 4096);
    ctx = build_evctx(cfd, lfd_ctx->svr, buff, cfd_callback);
    ctx->req = request_new();
    ev.data.ptr = ctx;
    ev.events = EPOLLIN | EPOLLONESHOT;
    epoll_ctl(lfd_ctx->svr->epoll_fd, EPOLL_CTL_ADD, cfd, &ev);
}

static int init_epollfd(struct server* svr) {
    int ret;
    int epoll_fd;
    struct epoll_event ev;

    epoll_fd = epoll_create(1);
    if (epoll_fd < 0) return epoll_fd;
    svr->epoll_fd = epoll_fd;
    ev.data.ptr = build_evctx(svr->lfd, svr, NULL, lfd_callback);
    ev.events = EPOLLIN | EPOLLONESHOT;
    ret = epoll_ctl(svr->epoll_fd, EPOLL_CTL_ADD, svr->lfd, &ev);
    return ret;
}

static void init(struct server* svr, struct server_attr* attr, error_t* err) {
    int ret;

    svr->pool = pool_init(attr->thread_cnt);
    ret = init_listenfd(svr);
    if (ret != 0) goto bad;
    ret = init_epollfd(svr);
    if (ret != 0) goto bad;
    return;
bad:
    error_put(err, strerror(errno));
}

struct server* server_create() {
    struct server* svr;

    svr = malloc(sizeof(struct server));
    svr->lfd = -1;
    svr->epoll_fd = -1;
    svr->port = 0;
    svr->services = hashmap_init(NULL, Type_String);
    return svr;
}

void server_destroy(struct server* svr) {
    if (svr->lfd != -1) {
        close(svr->lfd);
    }
    if (svr->epoll_fd != -1) {
        close(svr->epoll_fd);
    }
    pool_destory(svr->pool);
    hashmap_destroy(svr->services);
    free(svr);
}

static inline void init_default_attr(struct server_attr* attr) {
    attr->thread_cnt = DEFAULT_THREAD_CNT;
}

void server_listen(struct server* svr, const char* addr,
                   struct server_attr* attr, error_t* err) {
    int ready, i;
    struct epoll_event evs[MAX_EVENTS];
    struct evctx* ctx;
    struct addr_v4 add;
    struct server_attr _attr;

    if (!err) {
        error_t e = error_new();
        err = &e;
    }
    parse_addr(&add, addr, err);
    if (!err->null) return;
    svr->ip = add.ip;
    svr->port = add.port;
    if (attr == NULL) {
        init_default_attr(&_attr);
        attr = &_attr;
    }
    init(svr, attr, err);
    if (!err->null) return;
    while (1) {
        ready = epoll_wait(svr->epoll_fd, evs, MAX_EVENTS, -1);
        if (ready < 0) {
            errorf(err, "%s: epoll_wait failed: %s", __func__, strerror(errno));
            return;
        }
        for (i = 0; i < ready; i++) {
            if (!(evs[i].events & EPOLLIN)) continue;
            ctx = (struct evctx*)(evs[i].data.ptr);
            pool_put(svr->pool, ctx->callback, ctx);
        }
    }
}

void build_resp(struct argument* resp, uint16_t type_kind,
                const char* type_name, uint32_t data_len, char* data) {
    resp->type_kind = type_kind;
    resp->type_name_len = strlen(type_name);
    resp->data_len = data_len;
    resp->type_name = strdup(type_name);
    resp->data = malloc(data_len);
    memcpy(resp->data, data, data_len);
}
