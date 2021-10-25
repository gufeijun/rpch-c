#include "conn.h"

#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>

#include "addr.h"

struct conn {
    int cfd;
    struct addr_v4 addr;
};

conn_t* conn_new(int cfd, struct sockaddr_in* addr) {
    conn_t* conn;
    conn = malloc(sizeof(conn_t));
    conn->cfd = cfd;
    if (addr == NULL) return conn;
    conn->addr.port = addr->sin_port;
    inet_ntop(AF_INET, &addr->sin_addr.s_addr, conn->addr.ip_str,
              INET_ADDRSTRLEN);
    return conn;
}

void conn_destroy(conn_t* conn) {
    close(conn->cfd);
    free(conn);
}

struct addr_v4 conn_remote_addr(conn_t* conn) {
    return conn->addr;
}

int conn_read(conn_t* conn, char* buf, int size) {
    return read(conn->cfd, buf, size);
}

int conn_write(conn_t* conn, char* data, int size) {
    return write(conn->cfd, data, size);
}
