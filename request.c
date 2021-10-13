#include "request.h"

#include <stdio.h>
#include <stdlib.h>

#include "buffer.h"
#include "error.h"

struct req_arg {
    uint16_t type_kind;
    uint16_t type_name_len;
    uint32_t data_len;
    char* type_name;
    char* data;
};

static void req_arg_init(struct req_arg* arg) {
    arg->type_name_len = 0;
    arg->type_kind = arg->data_len = 0;
    arg->data = arg->type_name = NULL;
}

static inline void req_arg_destroy(struct req_arg* arg) {
    free(arg->type_name);
    free(arg->data);
}

static int parse_request_line(buffer_t* buf, request_t* req, error_t* err) {
    char *start, *end, *iter;
    int n, i, ret;
    int spaces[3];

    start = buf->buf + buf->front;
    end = buf->buf + buf->rear;
    for (i = 0, n = 0, iter = start; iter != end; iter++) {
        if (*iter == ' ') {
            if (i < 3) spaces[i] = n;
            ++i;
        }
        if (*iter == '\n') break;
        ++n;
    }
    if (iter == end) {
        if (buffer_avaliable(buf) == 0) goto bad;
        return 0;
    }
    if (i != 3 || iter == start || (iter == start + 1 && *start == '\r'))
        goto bad;

    buffer_drop(buf, n + 1);
    printf("rest: %d\n", buffer_buffered(buf));
    if (*(iter - 1) == '\r') --n;

    start[n] = '\0';
    req->service = malloc(spaces[0] + 1);
    req->method = malloc(spaces[1] - spaces[0] + 1);
    ret = sscanf(start, "%s%s%ld%ld", req->service, req->method, &req->argcnt,
                 &req->seq);
    if (ret == 4) return 1;
bad:
    error_put(err, ERR_BAD_RQUEST_LINE);
    return -1;
}

int read_request(request_t* req, buffer_t* buf, error_t* err) {
    int ret;

    ret = parse_request_line(buf, req, err);
    return ret;
}

request_t* request_new() {
    request_t* req;

    req = malloc(sizeof(request_t));
    req->service = req->method = NULL;
    req->argcnt = req->seq = req->state = 0;
    return req;
}

void request_destroy(request_t* req) {
    int i;

    free(req->service);
    free(req->method);
    if (req->args != NULL) {
        for (i = 0; i < req->argcnt; i++) {
            req_arg_destroy(req->args + i);
        }
        free(req->args);
    }
    free(req);
}

