/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBCD BIOS.OBJ, part 7 of 7: CD_getsector, CD_getsector2,
 * CD_set_test_parmnum and callback. */
#include "pe1/psyq_cd.h"

int CD_getsector(void *destination, int words) {
    volatile u8 *status;
    u32 ready;

    *D_8009B27C = 0;
    *D_8009B288 = 0x80;
    *D_8009B2B0 = 0x20943;
    *D_8009B28C = 0x1323;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = destination;
    *D_8009B2BC = words | 0x10000;

    status = D_8009B27C;
    do {
        ready = *status & 0x40;
    } while (!ready);

    *g_CdRegDmaControl = 0x11000000;
    {
        volatile u32 *control = g_CdRegDmaControl;
        if (*control & 0x1000000) {
            volatile u32 *pending = control;
            u32 mask = 0x1000000;
            do {
            } while (*pending & mask);
        }
    }

    {
        volatile u32 *request = D_8009B28C;

        *request = 0x1325;
    }
    return 0;
}

int CD_getsector2(void *destination, int words) {
    volatile u32 readback;
    volatile u8 *index;

    *g_CdRegIndexBase = 0;
    *g_CdRegResponse = 0x80;
    *D_8009B2B0 = 0x21020843;
    *(volatile u32 *)g_CdRegRequest = 0x1325;
    *D_8009B2B4 |= 0x8000;
    *D_8009B2B8 = destination;
    *D_8009B2BC = words | 0x10000;
    index = g_CdRegIndexBase;
    if (!(*index & 0x40)) {
        do {
        } while (!(*index & 0x40));
    }
    *g_CdRegDmaControl = 0x11400100;
    readback = *g_CdRegDmaControl;
    return 0;
}

void CD_set_test_parmnum(int status) {
    D_8009B260 = status;
}

int getintr(void);

void callback(void) {
    int pending;
    volatile u8 *ready = &g_CdReadyEvent;
    int bank = *D_8009B27C & 3;
    volatile u8 *complete = ready - 1;

    while ((pending = getintr()) != 0) {
        if ((pending & 4) && D_8009AFB8) {
            D_8009AFB8(*ready, D_800A3468);
        }
        if ((pending & 2) && D_8009AFB4) {
            D_8009AFB4(*complete, D_800A3460);
        }
    }

    *D_8009B27C = bank;
}
