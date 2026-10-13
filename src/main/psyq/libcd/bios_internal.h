#ifndef PE1_PSYQ_LIBCD_BIOS_INTERNAL_H
#define PE1_PSYQ_LIBCD_BIOS_INTERNAL_H

#include "pe1/psyq_bios.h"
#include "pe1/psyq_cd.h"

/* Compare the previous poll count, then commit the incremented counter. */
static inline int count_poll(void) {
    int count = D_800A347C;
    int old = count++;
    D_800A347C = count;
    return old;
}

static inline int timed_out(const char **commands, const char **events,
                            CdInterruptEvents *interrupts) {
    if (VSync(-1) > D_800A3478 || count_poll() > 0x3C0000) {
        puts(D_80011B18);
        printf(D_80011B28, D_800A3480, commands[g_CdLastCom],
               events[interrupts->sync], events[interrupts->ready]);
        CD_flush();
        return -1;
    }
    return 0;
}

static inline void copy_result(u8 *destination, const volatile u8 *source) {
    int remaining;
    if (destination) {
        remaining = 7;
        for (; remaining != -1; remaining--) {
            *destination++ = *source++;
        }
    }
}

static inline void dispatch_interrupts(u8 *sync, u8 *ready) {
    int bank = *g_CdRegIndexBase & 3;
    int pending;
    while ((pending = getintr()) != 0) {
        if ((pending & 4) && g_CdReadyCallback)
            g_CdReadyCallback(*ready, D_800A3468);
        if ((pending & 2) && g_CdSyncCallback)
            g_CdSyncCallback(*sync, D_800A3460);
    }
    *g_CdRegIndexBase = bank;
}

#endif
