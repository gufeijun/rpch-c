#include "../math.rpch.server.h"
#include "server.h"

uint32_t Math_Add(uint32_t a, uint32_t b, error_t* err) { return a + b; }

int32_t Math_Sub(int32_t a, int32_t b, error_t* err) { return a - b; }

int32_t Math_Multiply(struct TwoNum* num, error_t* err) {
    return num->A * num->B;
}

struct Quotient* Math_Divide(uint64_t a, uint64_t b, error_t* err) {
    //返回值为指针，利用自动生成的Quotient_create分配堆区内存
    //框架内部会对其进行free操作，无需担心内存泄露
    struct Quotient* resp = Quotient_create();
    resp->Rem = a % b;
    resp->Quo = a / b;
    return resp;
}

int main() {
    error_t err = error_new();
    server_t* svr = server_create();
    register_Math_service(svr);
    server_listen(svr, "127.0.0.1:8080", NULL, &err);
    if (!err.null) {
        printf("err occurred: %s\n", err.msg);
        return -1;
    }
    server_destroy(svr);
    return 0;
}
