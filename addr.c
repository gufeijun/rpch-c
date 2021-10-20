#include "addr.h"

void parse_addr(struct addr_v4* _ad, const char* _addr, error_t* err) {
    int ret;
    char addr[64];
    char* pos;
    struct in_addr ad;

    strcpy(addr, _addr);
    pos = strchr(addr, ':');
    if (!pos || *(pos + 1) == 0) {
        error_put(err, "[invalid addr]port not specificed");
        return;
    }
    if (pos == addr) {
        _ad->ip = INADDR_ANY;
        return;
    }
    *pos = '\0';
    ret = inet_pton(AF_INET, addr, &(ad));
    if (ret != 1) {
        errorf(err, "invalid addr:%s", addr);
        return;
    }
    _ad->ip = ad.s_addr;
    pos++;
    sscanf(pos, "%hd", &_ad->port);
}
