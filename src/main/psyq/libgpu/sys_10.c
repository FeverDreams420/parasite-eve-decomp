/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBGPU SYS.OBJ, part 10 of 11: LoadImage2, StoreImage2, MoveImage2
 * and DrawOTag2. */
#include "pe1/psyq_callbacks.h"
#include "pe1/gpu_dma_register_window.h"
#include "pe1/gpu_callbacks.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gpu_state.h"
#include "pe1/gpu_dma_timeout_state.h"
#include "sys_10_externs.h"
void checkRECT(char *, RECT *);
/* GPU status read through the GP1 port; it must be re-read on every poll. */
#define GPU_STATUS_READY() (*(volatile unsigned int *)g_GpuGp1Ptr & 0x04000000)
int Gpu_DmaTimeoutCheck(void);
void Gpu_RestoreDmaCallback(void);

int LoadImage2(RECT *rect, unsigned int *data)
{
    checkRECT(D_800119BC, rect);
    g_GpuDmaTimeoutDeadline = VSync(-1) + 240;
    g_GpuDmaWaitLoopCounter = 0;
    /* Read GPU readiness only after the volatile DMA status reports idle. */
    while ((*g_GpuDmaChcrPtr & 0x01000000) || !GPU_STATUS_READY()) {
        if (Gpu_DmaTimeoutCheck()) return -1;
    }
    DMACallback(2, Gpu_RestoreDmaCallback);
    D_80095744->u20.load(rect, data);
    return 0;
}

#include "sys_10_externs_1.h"

int StoreImage2(RECT *rect, unsigned int *data)
{
    checkRECT(D_800118E0, rect);
    g_GpuDmaTimeoutDeadline = VSync(-1) + 240;
    g_GpuDmaWaitLoopCounter = 0;
    /* Read GPU readiness only after the volatile DMA status reports idle. */
    while ((*g_GpuDmaChcrPtr & 0x01000000) || !GPU_STATUS_READY()) {
        if (Gpu_DmaTimeoutCheck()) return -1;
    }
    DMACallback(2, Gpu_RestoreDmaCallback);
    D_80095744->u1c.store(rect, data);
    return 0;
}

#include "sys_10_externs_2.h"

int MoveImage2(RECT *rect, int x, int y)
{
    checkRECT(D_800118EC, rect);
    g_GpuDmaTimeoutDeadline = VSync(-1) + 240;
    g_GpuDmaWaitLoopCounter = 0;
    /* Read GPU readiness only after the volatile DMA status reports idle. */
    while ((*g_GpuDmaChcrPtr & 0x01000000) || !GPU_STATUS_READY()) {
        if (Gpu_DmaTimeoutCheck()) return -1;
    }
    DMACallback(2, Gpu_RestoreDmaCallback);
    if (!rect->w || !rect->h) return -1;
    D_800957EC[0] = *(unsigned int *)&rect->x;
    D_800957EC[1] = ((unsigned int)y << 16) | (x & 0xFFFF);
    D_800957EC[2] = *(unsigned int *)&rect->w;
    D_80095744->u18.moveImage((char *)D_800957EC - 8);
    return 0;
}

#include "sys_10_externs_3.h"

int DrawOTag2(void *packet)
{
    if (D_8009574C.queueState.debugLevel >= 2) D_80095748(D_80011928, packet);
    g_GpuDmaTimeoutDeadline = VSync(-1) + 240;
    g_GpuDmaWaitLoopCounter = 0;
    /* Read GPU readiness only after the volatile DMA status reports idle. */
    while ((*g_GpuDmaChcrPtr & 0x01000000) || !GPU_STATUS_READY()) {
        if (Gpu_DmaTimeoutCheck()) return -1;
    }
    DMACallback(2, Gpu_RestoreDmaCallback);
    D_80095744->u18.moveImage(packet);
    return 0;
}
