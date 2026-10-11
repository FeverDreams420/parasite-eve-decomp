#include "func_80190A90_externs.h"

int *func_80190A90(int arg0, int arg1, int arg2, int arg3) {
    if (arg0 == 1) {
        int *base = &D_80190B8C;

        base[0] = arg1;
        D_80190B90 = arg2;
        D_80190B94 = arg3;
        base[0] = 0x1000;
    }
    return &D_80190B8C;
}
