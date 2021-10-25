#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../string.rpch.client.h"
#include "client.h"

int main() {
    error_t err = error_new();
    client_t* cli = client_dial("127.0.0.1:8080", &err);
    if (!err.null) {
        printf("err occurred: %s\n", err.msg);
        return -1;
    }
    {
        char* res = string_toupper("hello", cli);
        assert(!client_failed(cli) && strcmp(res, "HELLO") == 0);
        free(res);
    }
    {
        char* res = string_tolower("HELLO", cli);
        assert(!client_failed(cli) && strcmp(res, "hello") == 0);
        free(res);
    }
    {
        char* res = string_concat("hello", " world", cli);
        assert(!client_failed(cli) && strcmp(res, "hello world") == 0);
        free(res);
    }
    {
        int32_t res = string_atoi("10", cli);
        assert(!client_failed(cli) && res == 10);
    }
    printf("test succ\n");
    client_destroy(cli);
    return 0;
}
