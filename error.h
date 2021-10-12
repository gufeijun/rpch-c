#ifndef __RPCH_ERR_H_
#define __RPCH_ERR_H_
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct error {
    char msg[64];
    bool null;
} error_t;

static inline struct error error_new() {
    struct error e;
    e.null = false;
    return e;
}

void errorf(struct error* err, char* format, ...) {
    if (err == NULL) return;
    va_list marker;
    va_start(marker, format);
    sprintf(err->msg, format, marker);
    va_end(marker);
    err->null = false;
}

void error_put(struct error* err, char* str) {
    if (err == NULL) return;
    strcpy(err->msg, str);
    err->null = false;
}

#endif
