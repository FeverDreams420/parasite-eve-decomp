#include "func_80195CD8_externs.h"

void *func_80195CD8(void *arg0, int value) {
    if (value < 0x200) {
        value = 0x200;
    }

    D_80195EF0 = value;
    return D_80195EF4;
}
