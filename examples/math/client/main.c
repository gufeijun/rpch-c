#include <assert.h>
#include <stdint.h>

#include "../math.rpch.client.h"
#include "client.h"

int main() {
    error_t err = error_new();
    client_t* cli = client_dial("127.0.0.1:8080", &err);
    if (!err.null) {
        printf("err occurred: %s\n", err.msg);
        return -1;
    }
    {
        uint32_t res = Math_Add(1, 1, cli);
        assert(res == 2);
    }
    {
        int32_t res = Math_Sub(2, 3, cli);
        assert(res == -1);
    }
    {
        struct TwoNum nums;
        nums.A = 3;
        nums.B = 4;
        int32_t res = Math_Multiply(&nums, cli);
        assert(res == 12);
    }
    {
        struct Quotient* res = Math_Divide(9, 4, cli);
        assert(res->Quo == 2);
        assert(res->Rem == 1);
        Quotient_destroy(res);
    }
    printf("test succ\n");
    client_destroy(cli);
    return 0;
}