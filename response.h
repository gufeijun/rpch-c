#ifndef __RPCH_RESPONSE_H_
#define __RPCH_RESPONSE_H_
#include "request.h"

typedef struct response {
    struct req_arg* arg;
    int failed;
} response_t;

#endif
