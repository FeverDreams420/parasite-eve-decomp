#include "common.h"
#include "pe1/mdec_diag.h"

#include "libpress_decdct_externs.h"

unsigned short func_8010BF94(unsigned short *value) {
    return *value;
}

void func_8010BFA0(s32 *arg0, s32 arg1) {
    if (arg1 & 1) {
        *arg0 &= ~0x08000000;
    } else {
        *arg0 |= 0x08000000;
    }

    if (arg1 & 2) {
        *arg0 |= 0x02000000;
    } else {
        *arg0 &= ~0x02000000;
    }

    MdecDiag_SendInput(arg0, *(u16 *)arg0);
}

void func_8010C01C(s32 *buffer, u32 size) {
    MdecDiag_ReceiveOutput(buffer, size);
}

s32 func_8010C03C(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = func_8010C308();
    } else {
        result = ((u32)func_8010C430() >> 29) & 1;
    }
    return result;
}

s32 func_8010C078(s32 mode) {
    s32 result;

    if (mode == 0) {
        result = func_8010C39C();
    } else {
        result = ((u32)func_8010C430() >> 24) & 1;
    }
    return result;
}

void func_80073CF4(int mode, void *value);

void func_8010C0B4(void *value) {
    func_80073CF4(0, value);
}

void func_80073CF4(int mode, void *value);

void func_8010C0D8(void *value) {
    func_80073CF4(1, value);
}
