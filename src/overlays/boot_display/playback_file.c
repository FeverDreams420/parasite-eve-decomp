#include "pe1/boot_display.h"
#include "pe1/psyq_ds.h"
#include "pe1/psyq_ds_queue.h"

s32 BootDisplay_InitPlaybackFile(void) {
    DslFILE file;
    s32 found;
    /* Preserve separate zero/-1 retries and the ready-constant reload.
     * Pins and empty barriers are tracked in crutch debt. */
    register s32 ready asm("$16");
    s32 failure;
    volatile s32 *start;
    DsInit();
    DsSetDebug(0);
    for (;;) {
        ready = 1;
        asm volatile("" : : "r"(ready));
        if (DsSystemStatus() != ready || DsQueueLen() != 0)
            return VSync(0);
        failure = 0;
        found = (s32)DsSearchFile(&file, D_80125B48);
        if (found == failure) continue;
        failure = -1;
        asm volatile("" : : "r"(ready));
        if (found != failure) break;
    }
    start = &D_80172CE8;
    *start = DsPosToInt(&file.pos);
    D_80172CEC = DsPosToInt(&file.pos) + (file.size >> 11);
    D_80172CF0 = *start;
    D_80172CF4 = 0;
    D_80172CF8 = 0;
    D_80172CFA = 0;
    D_80172D00 = g_CdLoadAreaBase + (((g_MainAssetLbaTbl[0] - g_BootDisplayDataLbaTbl) << 11) + 0x34800);
    return 0;
}
