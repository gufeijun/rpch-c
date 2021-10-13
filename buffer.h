#ifndef __RPCH_BUFFER_H_
#define __RPCH_BUFFER_H_
#include "conn.h"
#include "error.h"

typedef struct buffer {
    conn_t* conn;
    char* buf;
    int cap;
    int len;
    int front;
    int rear;

} buffer_t;

buffer_t* buffer_new(conn_t* conn, int buf_size);

void buffer_destroy(buffer_t*);

//读取成功返回1，失败返回-1，对端关闭返回0
int buffer_fill(buffer_t* buff, error_t* err);

int buffer_copy(buffer_t* buff, char* dst, int size);

void buffer_drop(buffer_t* buff, int size);

static inline int buffer_buffered(buffer_t* buff) { return buff->len; }

static inline int buffer_avaliable(buffer_t* buff) {
    return buff->cap - buff->len;
}

#endif
