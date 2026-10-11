#include "room_m350_data.h"

short *func_8019A120(void *arg0, short arg1) {
    short *ptr = &D_8019A8B8.repeats;

    *ptr = arg1;
    return ptr - 0x24;
}
