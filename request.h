#ifndef __RPCH_REQUEST_H_
#define __RPCH_REQUEST_H_
#include <stdint.h>
#include <stdlib.h>

#include "buffer.h"
#include "error.h"

struct req_arg {
    uint16_t type_kind;
    uint16_t type_name_len;
    uint32_t data_len;
    int head_bytes_read;  //头部8B是否读取完
    char* type_name;
    char* data;
};

enum parse_state {
    READING_REQUEST_LINE,
    READING_ARGS,
};

typedef struct request {
    enum parse_state state;
    char* service;
    char* method;
    uint64_t argcnt;
    uint64_t seq;
    struct req_arg* args;
    struct req_arg* cur_arg;
} request_t;

request_t* request_new();
void request_destroy(request_t*);

//返回1代表一个请求读取完毕，0代表还未读取完
int read_request(request_t*, buffer_t*, error_t*);
void request_set_init_state(struct request*);

#endif
