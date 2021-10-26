#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "addr.h"
#include "argument.h"
#include "buffer.h"
#include "conn.h"
#include "error.h"
#include "type.h"

#define BUFSIZE 4096
#define MAGIC 0x00686A6C

static conn_t* client_connect(client_t* cli, const char* addr, error_t* err) {
    int cfd, ret;
    struct sockaddr_in svr_addr;
    struct addr_v4 add;
    conn_t* conn;

    parse_addr(&add, addr, err);
    if (!err->null) return NULL;

    cfd = socket(AF_INET, SOCK_STREAM, 0);
    if (cfd == -1) goto bad;
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_port = htons(add.port);
    svr_addr.sin_addr.s_addr = add.ip;

    ret = connect(cfd, (struct sockaddr*)&svr_addr, sizeof(svr_addr));
    if (ret == -1) {
        close(cfd);
        goto bad;
    }
    conn = conn_new(cfd, &svr_addr);
    return conn;
bad:
    if (err->null) error_put(err, strerror(errno));
    return NULL;
}

struct client* client_dial(const char* addr, error_t* err) {
    struct client* cli = NULL;
    conn_t* conn = NULL;
    uint32_t magic = MAGIC;
    int n;

    if (err == NULL) {
        error_t e = error_new();
        err = &e;
    }
    cli = malloc(sizeof(client_t));
    cli->err = error_new();
    conn = client_connect(cli, addr, err);
    if (!err->null) goto bad;
    n = conn_write(conn, (char*)&magic, 4);
    if (n <= 0) goto bad;
    cli->seq = 0;
    pthread_mutex_init(&cli->seq_lock, NULL);
    cli->err = error_new();
    cli->buff = buffer_new(conn, 0, BUFSIZE);
    return cli;
bad:
    if (err->null) error_put(err, strerror(errno));
    if (conn) conn_destroy(conn);
    if (cli) free(cli);
    return NULL;
}

void client_destroy(client_t* cli) {
    if (cli == NULL) return;
    pthread_mutex_destroy(&cli->seq_lock);
    buffer_destroy(cli->buff);
    free(cli);
}

static void client_send_request_line(client_t* cli,
                                     struct client_request* req) {
    buffer_write(cli->buff, req->service, strlen(req->service), &cli->err);
    buffer_write(cli->buff, " ", 1, &cli->err);
    buffer_write(cli->buff, req->method, strlen(req->method), &cli->err);
    buffer_write(cli->buff, " ", 1, &cli->err);
    sprintf(cli->buf, "%d", req->args_cnt);
    buffer_write(cli->buff, cli->buf, strlen(cli->buf), &cli->err);
    buffer_write(cli->buff, " ", 1, &cli->err);
    sprintf(cli->buf, "%" PRIu64 "\r\n", cli->seq++);
    buffer_write(cli->buff, cli->buf, strlen(cli->buf), &cli->err);
}

static void client_send_arguments(client_t* cli, struct argument* arg) {
    memcpy(cli->buf, (void*)&arg->type_kind, 2);
    memcpy(cli->buf + 2, (void*)&arg->type_name_len, 2);
    memcpy(cli->buf + 4, (void*)&arg->data_len, 4);
    buffer_write(cli->buff, cli->buf, 8, &cli->err);
    buffer_write(cli->buff, arg->type_name, arg->type_name_len, &cli->err);
    buffer_write(cli->buff, arg->data, arg->data_len, &cli->err);
}

static void read_full(conn_t* conn, char* buf, int size, error_t* err) {
    int n = 0;

    while (size != 0) {
        n = conn_read(conn, buf, size);
        if (n <= 0) goto bad;
        size -= n;
        buf += n;
    }
    return;
bad:
    error_put(err, strerror(errno));
}

static void discard_data(client_t* cli, int n) {
    int nn;
    char buf[64];
    while (n > 0) {
        nn = conn_read(cli->buff->conn, buf, n);
        if (nn <= 0) error_put(&cli->err, strerror(errno));
        n -= nn;
    }
}

static void read_err(client_t* cli, struct argument* resp) {
    int to_read;
    to_read = resp->data_len < 128 ? resp->data_len : 127;
    read_full(cli->buff->conn, cli->err.msg, to_read, &cli->err);
    if (client_failed(cli)) return;
    cli->err.msg[resp->data_len] = '\0';
    cli->err.null = false;
    if (resp->data_len >= 128) discard_data(cli, resp->data_len - 127);
    resp->data_len = 0;
}

static uint64_t read_response(client_t* cli, struct argument* resp) {
    uint64_t seq;

    read_full(cli->buff->conn, cli->resp_hbuf, 16, &cli->err);
    if (!cli->err.null) return 0;
    // 8B(seq) 2B(TypeKind) 2B(type_name_len) 4B(data_len) Typename data
    memcpy(&seq, cli->resp_hbuf, 8);
    memcpy(&resp->type_kind, cli->resp_hbuf + 8, 2);
    memcpy(&resp->type_name_len, cli->resp_hbuf + 10, 2);
    memcpy(&resp->data_len, cli->resp_hbuf + 12, 4);
    if (resp->type_kind == TYPE_NORTN) {
        return seq;
    }
    if (resp->type_kind == TYPE_ERROR) {
        read_err(cli, resp);
        return seq;
    }

    resp->type_name = malloc(resp->type_name_len + 1);
    read_full(cli->buff->conn, resp->type_name, resp->type_name_len, &cli->err);
    if (!cli->err.null) goto bad;
    resp->type_name[resp->type_name_len] = '\0';
    resp->data = malloc(resp->data_len + 1);
    read_full(cli->buff->conn, resp->data, resp->data_len, &cli->err);
    if (!cli->err.null) goto bad;
    resp->data[resp->data_len] = '\0';
    return seq;
bad:
    if (resp->type_name) free(resp->type_name);
    if (resp->data) free(resp->data);
    return 0;
}

uint64_t client_call(client_t* cli, struct client_request* req,
                     struct argument* resp) {
    int i;
    uint64_t seq = 0;

    resp->data = resp->type_name = NULL;
    pthread_mutex_lock(&cli->seq_lock);
    client_send_request_line(cli, req);
    if (!cli->err.null) goto end;
    for (i = 0; i < req->args_cnt; i++) {
        client_send_arguments(cli, req->args + i);
        if (!cli->err.null) goto end;
    }
    buffer_flush(cli->buff, &cli->err);
    seq = read_response(cli, resp);
end:
    pthread_mutex_unlock(&cli->seq_lock);
    return seq;
}
