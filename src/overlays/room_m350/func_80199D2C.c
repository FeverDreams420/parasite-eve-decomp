#include "room_m350_shared.h"
#include "func_80199D2C_externs.h"

char *func_80199D2C(void *arg0, char arg1, short arg2, short arg3) {
    char *ptr = (char *)&D_8019A831;

    *ptr = arg1;
    D_8019A82A[0] = arg2;
    D_8019A82C = arg3;
    return ptr - 0x29;
}
