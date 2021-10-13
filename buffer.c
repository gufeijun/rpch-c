#include "buffer.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "conn.h"

static inline void buffer_adjust(buffer_t* buff) {
    if (buff->front == 0) return;
    memcpy(buff->buf, buff->buf + buff->front, buffer_buffered(buff));
    buff->rear = buffer_buffered(buff);
    buff->front = 0;
}

buffer_t* buffer_new(conn_t* conn, int buf_size) {
    buffer_t* buff;
    buff = malloc(sizeof(buffer_t));
    buff->conn = conn;
    buff->buf = malloc(buf_size);
    buff->cap = buf_size;
    buff->len = buff->front = buff->rear = 0;
    return buff;
}

void buffer_destroy(buffer_t* buff) {
    conn_destroy(buff->conn);
    free(buff->buf);
    free(buff);
}

int buffer_fill(buffer_t* buff, error_t* err) {
    int n;

    if (buff->rear == buff->cap) buffer_adjust(buff);
    n = conn_read(buff->conn, buff->buf + buff->rear, buff->cap - buff->rear);
    if (n < 0) {
        error_put(err, strerror(errno));
        return -1;
    }
    if (n == 0) {
        error_put(err, "peer close connetion");
        return 0;
    }
    buff->rear += n;
    buff->len += n;
    return 1;
}

int buffer_copy(buffer_t* buff, char* dst, int size) {
    if (buffer_buffered(buff) < size) {
        size = buffer_buffered(buff);
    }
    memcpy(dst, buff->buf + buff->front, size);
    return size;
}

void buffer_drop(buffer_t* buff, int size) {
    if (size < 0) return;
    if (buffer_buffered(buff) < size) size = buff->len;
    buff->front += size;
    buff->len -= size;
}
