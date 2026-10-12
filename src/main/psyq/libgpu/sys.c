/* ASSEMBLER: GNU */
/* GCC_VERSION: 2.8.1 */
/* CC1_FLAGS: -mno-split-addresses */
/* Psy-Q LIBGPU SYS.OBJ, part 1 of 11: ResetGraph, SetGraphDebug,
 * SetGraphQueue, GetGraphDebug, DrawSyncCallback, SetDispMask, DrawSync,
 * ClearImage, ClearImage2, LoadImage, StoreImage, MoveImage, ClearOTag,
 * ClearOTagR, DrawPrim, DrawOTag, PutDrawEnv, DrawOTagEnv and GetDrawEnv. SYS
 * stays in eleven units because neighbouring parts need different compilers:
 * sys.c, sys_3.c, sys_6.c, sys_8.c and sys_10.c only match with GCC 2.8.1 and
 * -mno-split-addresses, sys_2.c, sys_4.c, sys_7.c, sys_9.c and sys_11.c only
 * with GCC 2.7.2 (the return delay slot), and sys_5.c only with -O1 or -fno-
 * schedule-insns. */
#include "pe1/psyq_callbacks.h"
#include "pe1/gpu_callbacks.h"
#include "common.h"
#include "pe1/gpu_state.h"
#include "include_asm.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_bios.h"

#include "sys_externs.h"

void GPU_cw(unsigned int);
int Gpu_InitDmaQueue(int);

int ResetGraph(int mode) {
    GpuSystemState *state;
    void *drawCache;
    volatile unsigned char *gpuType;
    switch (mode & 7) {
    case 0:
    case 3:
        {
            char *format;
            unsigned char *version;

            format = D_800117E0;
            version = D_80095704;
            printf(format, version, (GpuSystemState *)&D_8009574C);
        }
    case 5:
        state = (GpuSystemState *)&D_8009574C;
        gpuType = &state->status.type;
        GPU_memset((unsigned char *)state, 0, sizeof(*state));
        ResetCallback();
        GPU_cw((unsigned int)D_80095744 & 0xFFFFFF);
        *gpuType = Gpu_InitDmaQueue(mode);
        drawCache = &state->drawCache;
        state->status.queueState.queue = 1;
        state->status.width = D_800957CC[*gpuType][0];
        state->status.height = D_800957D8[*gpuType][0];
        GPU_memset(drawCache, -1, sizeof(state->drawCache));
        GPU_memset((unsigned char *)&state->displayCache, -1, sizeof(state->displayCache));
        return *gpuType;
    default:
        if (D_8009574E[0] >= 2) D_80095748(D_80011800, mode);
        return D_80095744->reset(1);
    }
}

int SetGraphDebug(int debugLevel) {
    u8 *currentDebugLevel;
    int result;
    GpuDebugPrintf debugPrint;
    int currentLevel;
    int type;
    int reverse;
    int oldDebugLevel;

    currentDebugLevel = &D_8009574C.queueState.debugLevel;
    /* Preserve the shared base used for the adjacent GPU state bytes. */
        oldDebugLevel = *currentDebugLevel;
    *currentDebugLevel = debugLevel;
    result = oldDebugLevel;

    if ((u8)debugLevel == 0) {
        return result;
    }

    debugPrint = D_80095748;
    /* Keep the callback load ahead of its arguments. */
        currentLevel = currentDebugLevel[0];
    type = currentDebugLevel[-2];
    reverse = currentDebugLevel[1];
    /* Materialize the byte arguments before the format string address. */
        debugPrint(D_80011814, currentLevel, type, reverse);
    result = oldDebugLevel;
    return result;
}

int SetGraphQueue(int mode)
{
    GpuQueueState *state = &D_8009574C.queueState;
    int previous = state->queue;
    if (state->debugLevel >= 2) {
        /* Keep the two prior-state paths: stock GCC merges the calls after
         * choosing the retail register lifetimes. Both log exactly once. */
        if (previous) {
            D_80095748(D_80011840, mode);
        } else {
            D_80095748(D_80011840, mode);
        }
    }
    if (mode != state->queue) {
        D_80095744->reset(1);
        state->queue = mode;
        DMACallback(2, 0);
    }
    return previous;
}

int GetGraphDebug(void) {
    return D_8009574C.queueState.debugLevel;
}

void *DrawSyncCallback(void *callback) {
    void *previous;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011854, callback);
    }

    previous = (void *)D_8009574C.drawSyncCallback;
    D_8009574C.drawSyncCallback = (void (*)())callback;
    return previous;
}

void SetDispMask(int mask) {
    u8 *debug = g_GraphDebug;
    u8 *clearPointer;
    GpuCallbacks *callbacks;
    int command;

    if (*debug >= 2) {
        g_GpuDebugPrintf[0](D_80011870, mask);
    }

    clearPointer = debug + 0x6A;
    if (mask == 0) {
        GPU_memset(clearPointer, -1, 20);
    }

    command = 0x03000000;
    asm("" : "+r"(command));
    callbacks = g_GpuCallbacks[0];
    if (mask == 0) {
        command |= 1;
    } else {
        command = 0x03000000;
    }
    callbacks->callback10(command);
}

int DrawSync(int arg0) {
    void (*fn)(char *, int);
    GpuCallbacks *callbacks;

    if (D_8009574C.queueState.debugLevel >= 2) {
        fn = D_80095748;
        fn(D_80011884, arg0);
    }

    callbacks = D_80095744;
    return callbacks->callback3C(arg0);
}

void checkRECT(char *name, RECT *rect) {
    char *format;
    switch (D_8009574C.queueState.debugLevel) {
    case 1:
        if (rect->w > D_8009574C.width || rect->w + rect->x > D_8009574C.width ||
            rect->y > D_8009574C.height || rect->y + rect->h > D_8009574C.height ||
            rect->w <= 0 || rect->x < 0 || rect->y < 0 || rect->h <= 0) {
            format = D_80011898;
            break;
        }
        return;
    case 2:
        format = D_800118B8;
        break;
    default:
        return;
    }
    D_80095748(format, name);
    {
        int x = rect->x;
        int y = rect->y;
        int width = rect->w;
        register int height asm("$3") = rect->h;
        D_80095748(D_800118A4, x, y, width, height);
    }
}

void checkRECT(char *message, RECT *rect);

int ClearImage(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118BC, rect);
    color = (b << 16) | (g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}

int ClearImage2(RECT *rect, u8 r, u8 g, u8 b) {
    GpuCallbacks *callbacks;
    u32 color;

    checkRECT(D_800118C8, rect);
    color = 0x80000000 | (b << 16) | (g << 8) | r;
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->clr, rect, 8, color);
}

void checkRECT(char *message, RECT *rect);

int LoadImage(RECT *rect, void *pixels) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118D4, rect);
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->u20.load, rect, 8, pixels);
}

int StoreImage(RECT *rect, void *pixels) {
    GpuCallbacks *callbacks;

    checkRECT(D_800118E0, rect);
    callbacks = D_80095744;
    return callbacks->addque2(callbacks->u1c.store, rect, 8, pixels);
}

int MoveImage(RECT *rect, int x, int y) {
    checkRECT(D_800118EC, rect);
    if (rect->w == 0 || rect->h == 0) {
        return -1;
    }
    {
        register u32 *packetData asm("$3");
        GpuCallbacks *callbacks;
        register u32 sourcePosition asm("$5");
        u32 *packet;
        int packetSize;
        u32 destinationPosition;
        register u32 destinationX asm("$4");
        u32 size;

        destinationPosition = y << 16;
        packetData = D_800957EC;
        asm("" : "+r"(packetData));
        destinationX = x & 0xFFFF;
        destinationPosition |= destinationX;
        sourcePosition = ((GpuRectWords *)rect)->position;
        callbacks = D_80095744;
        asm("" : "+r"(sourcePosition), "+r"(callbacks));
        packetSize = 20;
        asm("" : "+r"(packetSize));
        packetData[1] = destinationPosition;
        *packetData = sourcePosition;
        size = ((GpuRectWords *)rect)->size;
        packet = packetData - 2;
        packetData[2] = size;
        asm("" : : "r"(size) : "memory");
        return callbacks->addque2(callbacks->u18.moveImage, packet, packetSize,
                                  0);
    }
}

u32 *ClearOTag(u32 *ot, int count) {
    u32 mask;
    u32 linkMask;
    u32 *result;
    u32 *terminator;
    u32 address;
    u32 command;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_800118F8, ot, count);
    }

    if (--count) {
        do {
            --count;
            linkMask = 0xFFFFFF;
            ((u8 *)ot)[3] = 0;
            mask = 0xFF000000;
            *ot = (*ot & mask) | (PSX_ADDRESS(ot + 1) & linkMask);
            ++ot;
        } while (count);
    }

    mask = 0xFFFFFF;
    result = ot;
    PE1_COMPILER_LAUNDER(result);
    terminator = &D_8009580C;
    address = PSX_ADDRESS(&D_800957F8);
    address &= mask;
    command = 0x04000000;
    address |= command;
    *terminator = address;
    terminator = (u32 *)(PSX_ADDRESS(terminator) & mask);
    *result = PSX_ADDRESS(terminator);
    return result;
}

u32 *ClearOTagR(u32 *ot, int count) {
    u32 mask;
    u32 *result;
    u32 *terminator;
    u32 address;
    u32 command;

    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011910, ot, count);
    }

    D_80095744->clearOTag(ot, count);

    mask = 0xFFFFFF;
    result = ot;
    PE1_COMPILER_LAUNDER(result);
    terminator = &D_8009580C;
    address = PSX_ADDRESS(&D_800957F8);
    address &= mask;
    command = 0x04000000;
    address |= command;
    *terminator = address;
    terminator = (u32 *)(PSX_ADDRESS(terminator) & mask);
    *result = PSX_ADDRESS(terminator);
    return result;
}

int DrawPrim(void *primitive) {
    u8 *bytes = primitive;
    int word_count = bytes[3];

    D_80095744->callback3C(0);
    return D_80095744->cb14(bytes + 4, word_count);
}

void Gpu_SetDrawEnvBack(DR_ENV *, DRAWENV *);
void *memcpy(void *, const void *, unsigned int);

int DrawOTag(void *table)
{
    if (D_8009574C.queueState.debugLevel >= 2) {
        D_80095748(D_80011928, table);
    }
    {
        GpuCallbacks *callbacks = D_80095744;
        register void *argument asm("$5") = table;
        int count;
        /* Keep the dispatch load before the independently materialized zeros. */
        asm volatile("" : "+r"(callbacks) : "r"(argument));
        count = 0;
        asm volatile("" : "+r"(count));
        return callbacks->addque2(callbacks->u18.packet, argument, count, 0);
    }
}

DRAWENV *PutDrawEnv(DRAWENV *environment)
{
    unsigned char *state = D_8009574E;
    DR_ENV *packet;
    void *(*copy)(void *, const void *, unsigned int) = memcpy;
    if (state[0] >= 2) {
        D_80095748(D_8001193C, environment);
    }
    packet = &environment->dr_env;
    Gpu_SetDrawEnvBack(packet, environment);
    packet->tag |= 0xFFFFFF;
    D_80095744->addque2(D_80095744->u18.packet, packet, 0x40, 0);
    copy(state + 14, environment, sizeof(DRAWENV));
    return environment;
}

void DrawOTagEnv(void *next, DRAWENV *environment)
{
    unsigned char *state = D_8009574E;
    DR_ENV *packet;
    void *(*copy)(void *, const void *, unsigned int) = memcpy;
    if (state[0] >= 2) {
        D_80095748(D_80011954, next, environment);
    }
    packet = &environment->dr_env;
    Gpu_SetDrawEnvBack(packet, environment);
    packet->tag = (packet->tag & 0xFF000000) | (PSX_ADDRESS(next) & 0xFFFFFF);
    D_80095744->addque2(D_80095744->u18.packet, packet, 0x40, 0);
    copy(state + 14, environment, sizeof(DRAWENV));
}

DRAWENV *GetDrawEnv(DRAWENV *arg0) {
    void *(*fn)(void *, const void *, unsigned int);

    fn = memcpy;
    fn(arg0, &D_8009575C, sizeof(D_8009575C));
    return arg0;
}
