#ifndef __RPCH_ADDR_H_
#define __RPCH_ADDR_H_
#include <arpa/inet.h>

#include "error.h"

struct addr_v4 {
    char ip_str[INET_ADDRSTRLEN];
    uint16_t port;
    uint32_t ip;
};

void parse_addr(struct addr_v4* ad, const char* _addr, error_t* err);
#endif
