/* ASSEMBLER: GNU */
/* Psy-Q LIBDS DSSYS_1.OBJ, part 4 of 11: DS_cw, DS_vsync_callback, DS_ready_callback, DS_start_callback, DS_sync_callback, DS_system_status, DS_lastcom, DS_lastmode, DS_lastpos, DS_lastread, DS_lastseek, DS_status, DS_sync, DS_ready, DS_shell_open, DS_cw_system. */
#include "pe1/psyq_cd.h"
#include "pe1/psyq_ds.h"
#include "pe1/cdrom.h"

int DS_cw(u8 command, void *parameter) {
    register int *base asm("$6");
    register int *prev asm("$7");

    base = &g_CdRomCmdTimeout;
    asm volatile("" : "=r"(base) : "0"(base));
    if (base[0] > 0) {
        return 0;
    }
    if (base[-0x11] == 0) {
        prev = base - 0x11;
        return 0;
    }
    prev = base - 0x11;
    asm volatile("" : "=r"(prev) : "0"(prev));
    if (base[-9] != 1) {
        return 0;
    }
    prev[7] = 0x1F;
    base[-9] = 2;
    return LIBDS_DSSYS_1_text_368((prev[9] = 0xB, command), parameter);
}

void DS_vsync_callback(DsCallback callback) {
    g_DsPollCallback = callback;
}

void DS_sync_callback(DsEventCallback callback) {
    g_DsSyncCallback = callback;
}

void DS_ready_callback(DsEventCallback callback) {
    g_DsReadyCallback = callback;
}

void DS_start_callback(DsEventCallback callback) {
    g_DsDispatchCallback = callback;
}

u32 DS_system_status(u32 mode) {
    return ((u32 *)&g_DsReadStatusBlock)[mode];
}

#include "dssys_1_4_externs_1.h"

int DS_lastcom(void) {
    return g_CdLastCmd;
}

#include "dssys_1_4_externs_2.h"

int DS_lastmode(void) {
    return g_CdCmdMode;
}

CdlLOC *DS_lastpos(void) {
    return &g_CdCurPosPtr;
}

#include "dssys_1_4_externs_3.h"

int DS_lastseek(void) {
    return g_CdRetryCount;
}

#include "dssys_1_4_externs_4.h"

int DS_lastread(void) {
    return g_CdCmdParam;
}

int DS_status(void) {
    return g_CdSeekState.eventStatus;
}

void DS_sync(u8 *result) {
    CD_sync(1, result);
}

void DS_ready(u8 *result) {
    CD_ready(1, result);
}

#include "dssys_1_4_externs_5.h"

int DS_shell_open(void) {
    return g_CdDiscType;
}

int DS_cw_system(int arg0, void *parameter) {
    volatile int *ptr;
    ptr = &g_CdRomCmdTimeout;
    if (*ptr > 0) {
        return 0;
    }
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    {
        volatile CdRomCommandState *command =
            (volatile CdRomCommandState *)((char *)ptr -
                PE1_OFFSETOF(CdRomCommandState, read.readyResult));
        command->eventValue = 0x20;
    }
    return LIBDS_DSSYS_1_text_368(arg0, parameter);
}
