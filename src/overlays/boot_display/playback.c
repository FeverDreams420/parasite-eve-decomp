#include "pe1/boot_display.h"

s32 BootDisplay_StepPlaybackTransfer(u8 mode) {
    volatile u16 *index;
    u16 next;
    if (func_8007F72C() == 1 && func_8007F7A8() != D_800B0DD4)
        func_800719E4(1);
    if (mode == 1) {
        index = &D_80172CF8;
        func_80080AE4(D_80172D00 + (*index << 11), 512);
        next = (*index + 1) & 7;
        *index = next;
    } else {
        D_80172CFA = D_80172CFA + 1;
        next = D_80172CFA;
    }
    return next;
}

void BootDisplay_CheckPlaybackTime(void) {
    volatile s32 *time;
    if ((func_80073A44(-1) & 31) == 0) {
        time = &D_80172CF0;
        if (*time > D_80172CEC) {
            func_800870F0(0);
            D_80172CE4 = 1;
        } else if (*time < D_80172CE8 - 300) {
            func_801258EC(200, 27);
        } else {
            func_8007EE84(17, 0, D_80125AF0, 0);
        }
    }
}
