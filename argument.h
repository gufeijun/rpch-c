#ifndef __RPCH_ARGUMENT_H_
#define __RPCH_ARGUMENT_H_
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define TYPE_KIND_NORMAL 0
#define TYPE_KIND_STREAM 1
#define TYPE_KIND_MESSAGE 2
#define TYPE_KIND_ERROR 3
#define TYPE_KIND_NORTN 4

struct argument {
    uint16_t type_kind;
    uint16_t type_name_len;
    uint32_t data_len;
    int head_bytes_read;  //请求头部8B是否读取完
    int no_free;          //是否需要free data以及type_name
    char* type_name;
    char* data;
};

static inline void argument_init_with_option(struct argument* arg,
                                             uint16_t kind, char* type_name,
                                             void* src, int size) {
    arg->type_kind = kind;
    arg->type_name_len = strlen(type_name);
    arg->type_name = type_name;
    arg->data_len = size;
    arg->data = (char*)src;
}

static inline void argument_init(struct argument* arg) {
    arg->type_name_len = 0;
    arg->type_kind = arg->data_len = 0;
    arg->data = arg->type_name = NULL;
    arg->head_bytes_read = arg->no_free = 0;
}

static inline void argument_destroy(struct argument* arg) {
    if (arg->no_free) return;
    free(arg->type_name);
    free(arg->data);
}

#endif
