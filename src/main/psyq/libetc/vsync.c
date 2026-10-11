/* ASSEMBLER: GNU */
/* Psy-Q LIBETC VSYNC.OBJ: VSync, v_wait. */
#include "common.h"
#include "pe1/psyq_bios.h"

#include "vsync_externs.h"
void v_wait(int target, int frames);

int VSync(int mode) {
    u32 status;
    int elapsed;
    register int target asm("$2");
    int frames;
    int next;
    volatile u32 sample;

    status = *gpu_status;
    do {
        sample = *hblank_counter;
    } while (sample != *hblank_counter);
    elapsed = (sample - last_hblank) & 0xFFFF;
    if (mode < 0) return g_VSyncCount;
    if (mode == 1) return elapsed;
    if (mode > 0) {
        target = last_vblank - 1;
        target += mode;
    } else {
        target = last_vblank;
    }
    frames = 0;
    if (mode > 0) frames = mode - 1;
    v_wait(target, frames);
    status = *gpu_status;
    next = g_VSyncCount;
    next++;
    v_wait(next, 1);
    if (status & 0x400000) {
        while (!((status ^ *gpu_status) & 0x80000000)) {}
    }
    last_vblank = g_VSyncCount;
    do {
        last_hblank = *hblank_counter;
    } while (last_hblank != *hblank_counter);
    return elapsed;
}

#include "vsync_externs_1.h"

int ChangeClearRCnt(int counter, int enabled);

void v_wait(int arg0, int arg1) {
    volatile int timeout = arg1 << 15;

    if (g_VSyncCount < arg0) {
        do {
            if (--timeout == -1) {
                puts(D_800116FC);
                ChangeClearPAD(0);
                ChangeClearRCnt(3, 0);
                return;
            }
        } while (g_VSyncCount < arg0);
    }
}
