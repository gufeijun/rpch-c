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

buffer_t* buffer_new(conn_t* conn, int buf_size, int wbuf_size) {
    buffer_t* buff;
    buff = malloc(sizeof(buffer_t));
    buff->conn = conn;
    buff->buf = buf_size > 0 ? malloc(buf_size) : NULL;
    buff->cap = buf_size;
    buff->len = buff->front = buff->rear = 0;
    buff->wbuf = wbuf_size > 0 ? malloc(wbuf_size) : NULL;
    buff->wbuf_cap = wbuf_size;
    buff->wbuf_len = 0;
    return buff;
}

void buffer_destroy(buffer_t* buff) {
    conn_destroy(buff->conn);
    free(buff->buf);
    free(buff->wbuf);
    free(buff);
}

int buffer_write(buffer_t* buff, char* src, int size, error_t* err) {
    if (!err->null) return -1;
    int n;

    if (size + buff->wbuf_len > buff->wbuf_cap) {
        buffer_flush(buff, err);
        if (!err->null) return -1;
        n = conn_write(buff->conn, src, size);
        if (n <= 0) goto bad;
        buff->wbuf_len = 0;
        return size;
    }
    memcpy(buff->wbuf + buff->wbuf_len, src, size);
    buff->wbuf_len += size;
    return size;
bad:
    if (n == 0)
        error_put(err, "peer close connetion");
    else
        error_put(err, strerror(errno));
    return -1;
}

void buffer_flush(buffer_t* buff, error_t* err) {
    int n;

    n = conn_write(buff->conn, buff->wbuf, buff->wbuf_len);
    if (n == 0)
        error_put(err, "peer close connetion");
    else if (n < 0)
        error_put(err, strerror(errno));
    else
        buff->wbuf_len = 0;
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
        return -1;
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
