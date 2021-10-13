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

#include "buffer.h"
#include "error.h"
#include "evctx.h"
#include "request.h"
#include "threadpool.h"

#define DEFAULT_LISTEN_CNT 128
#define DEFAULT_THREAD_CNT 5
#define MAX_EVENTS 1024
#define MAGIC 0x00686A6C
#define MAGIC_SIZE 4

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

static void cfd_callback(void* arg) {
    int ret;
    uint32_t magic;
    struct evctx* ctx;
    struct request* req;

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

        //------------------------------------------------------
        printf("[%s.%s]arg cnts: %ld, sequence: %ld :\n", req->service,
               req->method, req->argcnt, req->seq);
        for (int i = 0; i < req->argcnt; i++) {
            struct req_arg* arg = req->args + i;
            printf(
                "\t\t[arg%d]type_kind=%hd, typename=%s, datalen=%u, data=%s\n ",
                i, arg->type_kind, arg->type_name, arg->data_len, arg->data);
        }
        //------------------------------------------------------
        request_set_init_state(ctx->req);
    }

not_ready:
    reset_oneshot(ctx);
    return;
bad:
    // TODO error handling
    printf("err :%s\n", ctx->err.msg);
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

    buff = buffer_new(conn, 4096);
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

static void init(struct server* svr, error_t* err) {
    int ret;

    ret = init_listenfd(svr);
    if (ret != 0) goto bad;
    ret = init_epollfd(svr);
    if (ret != 0) goto bad;
    return;
bad:
    error_put(err, strerror(errno));
}

static void parse_addr(struct server* svr, const char* _addr, error_t* err) {
    int ret;
    char addr[64];
    char* pos;
    struct in_addr ad;

    strcpy(addr, _addr);
    pos = strchr(addr, ':');
    if (!pos || *(pos + 1) == 0) {
        error_put(err, "[invalid addr]port not specificed");
        return;
    }
    if (pos == addr) {
        svr->ip = INADDR_ANY;
        return;
    }
    *pos = '\0';
    ret = inet_pton(AF_INET, addr, &(ad));
    if (ret != 1) {
        errorf(err, "invalid addr:%s", addr);
        return;
    }
    svr->ip = ad.s_addr;
    pos++;
    sscanf(pos, "%hd", &svr->port);
}

struct server* server_create() {
    struct server* svr;

    svr = malloc(sizeof(struct server));
    svr->lfd = -1;
    svr->epoll_fd = -1;
    svr->port = 0;
    svr->pool = pool_init(DEFAULT_THREAD_CNT);
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
    free(svr);
}

void server_listen(struct server* svr, const char* addr, error_t* err) {
    int ready, i;
    struct epoll_event evs[MAX_EVENTS];
    struct evctx* ctx;

    if (!err) {
        error_t e = error_new();
        err = &e;
    }
    parse_addr(svr, addr, err);
    if (!err->null) return;
    init(svr, err);
    if (!err->null) return;
    printf("lfd=%d\n", svr->lfd);
    while (1) {
        ready = epoll_wait(svr->epoll_fd, evs, MAX_EVENTS, -1);
        if (ready < 0) {
            errorf(err, "%s: epoll_wait failed: %s", __func__, strerror(errno));
            return;
        }
        for (i = 0; i < ready; i++) {
            if (!(evs[i].events & EPOLLIN)) continue;
            ctx = (struct evctx*)(evs[i].data.ptr);
            printf("[events]fd=%d\n", ctx->fd);
            pool_put(svr->pool, ctx->callback, ctx);
        }
    }
}
