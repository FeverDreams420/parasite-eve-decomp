#include "common.h"
#include "pe1/boot_display.h"
#include "pe1/cdrom.h"

#include "playback_externs.h"

/* Retry labels preserve the original command-error control flow. */
s32 BootDisplay_StartPlayback(void) {
    u8 mode[8];
    u8 filter[8];
    u8 result[8];
    s32 request;
    s32 status;
    mode[0] = 200;
retryMode:
    do {
        while (func_8007F72C() != 1) {}
    } while (func_8007F778() != 0);
    request = func_8007EE84(14, mode, 0, -1);
    if (request == 0) goto retryMode;
    while ((status = func_8007F418(request, result)) != 2) {
        if (status != 0) goto retryMode;
    }
    filter[0] = 1;
    filter[1] = 0;
retryFilter:
    do {
        while (func_8007F72C() != 1) {}
    } while (func_8007F778() != 0);
    request = func_8007EE84(13, filter, 0, -1);
    if (request == 0) goto retryFilter;
    while ((status = func_8007F418(request, result)) != 2) {
        if (status != 0) goto retryFilter;
    }
    func_800824C8(BootDisplay_StepPlaybackTransfer);
    func_80073D24(BootDisplay_CheckPlaybackTime);
    func_801258EC(200, 27);
    do {
        while (func_8007F72C() != 1) {}
    } while (func_8007F778() != 0);
    return 0;
}

int BootDisplay_SetCdFilter(int code) {
    int result[2];
    u8 command[2];
    command[0] = 1;
    command[1] = code;
    while (func_8007F418(0, 0) != 2) {}
    DsControlB(13, command, result);
    return 0;
}

int func_801258EC(int command, int parameter) {
    int result[2];
    int status;
    int *request = &D_80172CE8;
    func_80080B44(*request, result);
    status = func_8007F0C8((u8)command, result, (u8)parameter, 0, -1);
    D_80172CF0 = *request;
    D_80172CF4++;
    return (u8)status;
}

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

void BootDisplay_RecordReadPosition(u8 mode, int value) {
    if (mode == 2) {
        D_80172CFC = func_80080C48(value + 5);
        if (D_80172CFC != 0) {
            D_80172CF0 = D_80172CFC;
        }
    }
}
