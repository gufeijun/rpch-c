#ifndef __RPCH_REQUEST_H_
#define __RPCH_REQUEST_H_
#include <stdint.h>
#include <stdlib.h>

#include "argument.h"
#include "buffer.h"
#include "error.h"

enum parse_state {
    READING_REQUEST_LINE,
    READING_ARGS,
};

typedef struct request {
    enum parse_state state;
    char* service_method;
    uint64_t argcnt;
    uint64_t seq;
    struct argument* args;
    struct argument* cur_arg;
} request_t;

request_t* request_new();
void request_destroy(request_t*);

//返回1代表一个请求读取完毕，0代表还未读取完
int read_request(request_t*, buffer_t*, error_t*);
void request_set_init_state(struct request*);

#endif
