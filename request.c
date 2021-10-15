#include "request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"
#include "error.h"

static int parse_request_line(request_t* req, buffer_t* buf, error_t* err) {
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
    if (*(iter - 1) == '\r') --n;

    start[n] = '\0';
    req->service_method = malloc(spaces[1] + 1);
    ret = sscanf(start, "%s%s%ld%ld", req->service_method,
                 req->service_method + spaces[0] + 1, &req->argcnt, &req->seq);
    req->service_method[spaces[0]] = '.';
    if (ret == 4 && req->argcnt < 10) return 1;
bad:
    error_put(err, ERR_BAD_RQUEST_LINE);
    return -1;
}

static inline void copy(buffer_t* buff, char** dst, int len) {
    *dst = malloc(len + 1);
    memcpy(*dst, buff->buf + buff->front, len);
    (*dst)[len] = '\0';
    buffer_drop(buff, len);
}

static int parse_args(request_t* req, buffer_t* buf, error_t* err) {
    int i;
    char* start;
    struct argument *cur_arg, *last_arg;

    if (req->args == NULL) {
        req->args = malloc(sizeof(struct argument) * req->argcnt);
        for (i = 0; i < req->argcnt; i++) argument_init(req->args + i);
        req->cur_arg = req->args;
    }
    last_arg = req->args + (req->argcnt - 1);
    while (1) {
        cur_arg = req->cur_arg;
        start = buf->buf + buf->front;
        if (!cur_arg->head_bytes_read) {
            if (buffer_buffered(buf) < 8) return 0;
            cur_arg->type_kind = *(uint16_t*)start;
            cur_arg->type_name_len = *(uint16_t*)(start + 2);
            cur_arg->data_len = *(uint32_t*)(start + 4);
            cur_arg->head_bytes_read = 1;
            buffer_drop(buf, 8);
        }
        if (buffer_buffered(buf) < cur_arg->data_len + cur_arg->type_name_len)
            return 0;
        if (cur_arg->type_name_len != 0)
            copy(buf, &cur_arg->type_name, cur_arg->type_name_len);
        if (cur_arg->data_len != 0)
            copy(buf, &cur_arg->data, cur_arg->data_len);
        if (cur_arg == last_arg) break;
        req->cur_arg++;
    }
    return 1;
}

int read_request(request_t* req, buffer_t* buf, error_t* err) {
    int ret;

    switch (req->state) {
        case READING_REQUEST_LINE:
            ret = parse_request_line(req, buf, err);
            if (ret == 0 || ret == -1) break;
            req->state = READING_ARGS;
        case READING_ARGS:
            ret = parse_args(req, buf, err);
            if (ret == 0 || ret == -1) break;
            req->state = READING_REQUEST_LINE;
            break;
        default:
            ret = -1;
    }
    return ret;
}

request_t* request_new() {
    request_t* req;

    req = malloc(sizeof(request_t));
    req->service_method = NULL;
    req->cur_arg = req->args = NULL;
    req->argcnt = req->seq = 0;
    req->state = READING_REQUEST_LINE;
    return req;
}

void request_destroy(request_t* req) {
    int i;

    free(req->service_method);
    if (req->args != NULL) {
        for (i = 0; i < req->argcnt; i++) {
            argument_destroy(req->args + i);
        }
        free(req->args);
    }
    free(req);
}

void request_set_init_state(struct request* req) {
    free(req->service_method);
    free(req->args);
    req->state = READING_REQUEST_LINE;
    req->service_method = NULL;
    req->seq = req->argcnt = 0;
    req->args = req->cur_arg = NULL;
}
