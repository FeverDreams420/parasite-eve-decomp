#include "common.h"
#include "room_m273_boss_state.h"

#include "func_8019A1A0_externs.h"

int func_80052B2C(void);

void *func_8019A1A0(void *unused, int mode) {
    int direction;
    int value;

    D_8019AEFA = mode;
    switch (mode) {
    case 0:
        D_8019AEEE = 0;
        D_8019AEEA = 2;
        break;
    case 1:
        direction = (func_80052B2C() & 1) ? 42 : -42;
        D_8019AEF2 = 2;
        D_8019AEEA = 2;
        PE1_COMPILER_MEMORY_BARRIER();
        value = 0x61;
        D_8019AEEE = direction;
        D_8019AEF6 = value;
        break;
    case 2:
        direction = (func_80052B2C() & 1) ? 0x80 : -0x80;
        D_8019AEF2 = 4;
        D_8019AEEA = 5;
        PE1_COMPILER_MEMORY_BARRIER();
        value = 0xB0;
        D_8019AEEE = direction;
        D_8019AEF6 = value;
        break;
    }
    return &D_8019AE9C;
}
