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
        assert(!client_failed(cli) && res == 2);
    }
    {
        int32_t res = Math_Sub(2, 3, cli);
        assert(!client_failed(cli) && res == -1);
    }
    {
        struct TwoNum nums;
        nums.A = 3;
        nums.B = 4;
        int32_t res = Math_Multiply(&nums, cli);
        assert(!client_failed(cli) && res == 12);
    }
    {
        //返回值为指针，使用完后我们有义务释放内存，如下Quotient_delete(res);
        struct Quotient* res = Math_Divide(9, 4, cli);
        assert(!client_failed(cli) && res->Quo == 2);
        assert(res->Rem == 1);
        Quotient_delete(res);
    }
    printf("test succ\n");
    client_destroy(cli);
    return 0;
}
