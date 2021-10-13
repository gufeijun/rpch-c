#ifndef __RPCH_ERR_H_
#define __RPCH_ERR_H_
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define ERR_SHORT_READ "rpch: short read"
#define ERR_INVALID_MAGIC "rpch: invalid magic number"
#define ERR_INVALID_KIND "rpch: invalid type kind"
#define ERR_BAD_RQUEST_LINE "rpch: invalid request line"
#define ERR_BAD_REQUEST_SERVICE "rpch: request non-existent service"
#define ERR_BAD_REQUEST_METHOD "rpch: request non-existent method "
#define ERR_BAD_REQUEST_MESSAGE "rpch: unrecognized request message"
#define ERR_BAD_REQUEST_TYPE "rpch: unrecognized request builtin type"
#define ERR_BAD_REQUEST_ARG_CNT \
    "rpch: request argument count dose not confirm to method signature"
#define ERR_BAD_STREAM_TYPE "rpch: unrecognized stream type"

typedef struct error {
    char msg[128];
    bool null;
} error_t;

static inline struct error error_new() {
    struct error e;
    e.null = true;
    return e;
}

static inline void errorf(struct error* err, char* format, ...) {
    if (err == NULL) return;
    va_list marker;
    va_start(marker, format);
    sprintf(err->msg, format, marker);
    va_end(marker);
    err->null = false;
}

static inline void error_put(struct error* err, char* str) {
    if (err == NULL) return;
    strcpy(err->msg, str);
    err->null = false;
}

#endif
