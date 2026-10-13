/* ASSEMBLER: GNU */
/* CC1_FLAGS: -fno-expensive-optimizations */
/* Psy-Q LIBCD BIOS.OBJ, part 2 of 7: CD_sync, CD_ready. */

#include "bios_internal.h"

/* Adjacent command/ready polling routines from LIBCD BIOS_1.OBJ. */

int CD_sync(int mode, u8 *result) {
    u8 *sync;
    u8 *ready;
    const char **commands;
    const char **events;
    D_800A3478 = VSync(-1) + 0x3C0;
    commands = g_CdCommandNames;
    events = g_CdInterruptNames;
    sync = (u8 *)&g_CdInterruptEvents;
    ready = (u8 *)&((CdInterruptEvents *)sync)->ready;
    D_800A347C = 0;
    D_800A3480 = D_80011BA0;
    do {
        int status;
        if (timed_out(commands, events, (CdInterruptEvents *)sync)) {
            return -1;
        }
        if (CheckCallback()) {
            dispatch_interrupts(sync, ready);
        }
        status = ((CdInterruptEvents *)sync)->sync;
        if (status == 2 || status == 5) {
            ((CdInterruptEvents *)sync)->sync = 2;
            copy_result(result, D_800A3460);
            return status;
        }
    } while (!mode);
    return 0;
}

int CD_ready(int mode, u8 *result) {
    u8 *sync;
    u8 *ready;
    volatile u8 *end;
    const char **commands;
    const char **events;
    D_800A3478 = VSync(-1) + 0x3C0;
    commands = g_CdCommandNames;
    events = g_CdInterruptNames;
    sync = (u8 *)&g_CdInterruptEvents;
    ready = (u8 *)&((CdInterruptEvents *)sync)->ready;
    end = (volatile u8 *)&((CdInterruptEvents *)sync)->end;
    D_800A347C = 0;
    D_800A3480 = D_80011BA8;
    do {
        int status;
        if (timed_out(commands, events, (CdInterruptEvents *)sync)) {
            return -1;
        }
        if (CheckCallback()) {
            dispatch_interrupts(sync, ready);
        }
        status = *end;
        if (status) {
            ((CdInterruptEvents *)sync)->end = 0;
            copy_result(result, D_800A3470);
            return status;
        }
        status = end[-1];
        if (status) {
            ((CdInterruptEvents *)sync)->ready = 0;
            copy_result(result, D_800A3468);
            return status;
        }
    } while (!mode);
    return 0;
}
