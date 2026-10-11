#include "room_m349.h"

short *func_801900D0(int arg0, int arg1, int arg2, int arg3) {
    if (arg0 == 1) {
        D_80190194 = arg1;
        D_80190196 = arg2;
        D_80190198 = arg3;
    }
    if (arg0 == 2) {
        D_8019019C = arg1;
        D_801901A4 = arg2;
    } else {
        if (arg1 < 2) {
            arg1 = 2;
        }
        D_801901A0 = arg1;
    }
    return &D_80190194;
}
