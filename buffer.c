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
    buff->len = buff->len = buff->front = buff->rear = 0;
    buff->err = error_new();
    return buff;
}

void buffer_destroy(buffer_t* buff) {
    conn_destroy(buff->conn);
    free(buff->buf);
    free(buff);
}

int buffer_read(buffer_t* buff, char* dst, int size) {
    if (!buff->err.null) return -1;
    if (buffer_buffered(buff) == 0) {
        return conn_read(buff->conn, dst, size);
    }
    if (buffer_buffered(buff) < size) {
        size = buffer_buffered(buff);
    }
    memcpy(dst, buff->buf, size);
    buff->front += size;
    buff->len -= size;
    buffer_adjust(buff);
    return size;
}

int buffer_fill(buffer_t* buff) {
    if (!buff->err.null) return -1;
    if (buffer_avaliable(buff) == 0) return 0;
    int n;
    n = conn_read(buff->conn, buff->buf + buff->rear, buffer_avaliable(buff));
    if (n <= 0) goto bad;
    buff->rear += n;
    buff->len += n;
bad:
    if (n < 0)
        error_put(&buff->err, strerror(errno));
    else if (n == 0)
        error_put(&buff->err, "peer close the connection");
    return -1;
}

int buffer_peek(buffer_t* buff, char* dst, int size) {
    if (!buff->err.null) return -1;
    if (buffer_buffered(buff) < size) {
        size = buffer_buffered(buff);
    }
    memcpy(dst, buff->buf, size);
    return size;
}
