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
    error_t err;

} buffer_t;

buffer_t* buffer_new(conn_t* conn, int buf_size);

void buffer_destroy(buffer_t*);

int buffer_read(buffer_t* buff, char* dst, int size);

int buffer_fill(buffer_t* buff);

int buffer_peek(buffer_t* buff, char* dst, int size);

// int buffer_readline(buffer_t* buff);

static inline int buffer_buffered(buffer_t* buff) { return buff->len; }

static inline int buffer_avaliable(buffer_t* buff) {
    return buff->cap - buff->len;
}

#endif
