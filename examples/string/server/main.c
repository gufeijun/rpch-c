#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "../string.rpch.server.h"
#include "server.h"

char* string_toupper(char* str, error_t* err) {
    unsigned long len = strlen(str);
    for (unsigned long i = 0; i < len; i++) {
        str[i] = toupper(str[i]);
    }
    //必须返回新开辟的堆区数据
    return strdup(str);
}

char* string_tolower(char* str, error_t* err) {
    unsigned long len = strlen(str);
    for (unsigned long i = 0; i < len; i++) {
        str[i] = tolower(str[i]);
    }
    //必须返回新开辟的堆区数据
    return strdup(str);
}

char* string_concat(char* str1, char* str2, error_t* err) {
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    char* str = malloc(len1 + len2 + 1);
    memcpy(str, str1, len1);
    memcpy(str + len1, str2, len2);
    return str;
}

int32_t string_atoi(char* str, error_t* err) {
    int32_t res = 0;
    sscanf(str, "%d", &res);
    return res;
}

int main() {
    error_t err = error_new();
    server_t* svr = server_create();
    register_string_service(svr);
    struct server_attr attr;
    attr.thread_cnt = 5;
    server_listen(svr, "127.0.0.1:8080", &attr, &err);
    if (!err.null) {
        printf("err occurred: %s\n", err.msg);
        return -1;
    }
    server_destroy(svr);
    return 0;
}
