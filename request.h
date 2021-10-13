#ifndef __RPCH_REQUEST_H_
#define __RPCH_REQUEST_H_
#include <stdint.h>
#include <stdlib.h>

#include "buffer.h"
#include "error.h"

struct req_arg;

typedef struct request {
    int state;
    char* service;
    char* method;
    uint64_t argcnt;
    uint64_t seq;
    struct req_arg* args;
} request_t;

request_t* request_new();
void request_destroy(request_t*);

//返回1代表一个请求读取完毕，0代表还未读取完
int read_request(request_t*, buffer_t*, error_t*);

#endif
