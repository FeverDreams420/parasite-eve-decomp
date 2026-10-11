#include "pe1/draw_buffers.h"
#include "pe1/psyq_gpu.h"
#include "pe1/boot_disc_check.h"
#include "pe1/render_tint.h"
#include "pe1/cdrom_buffers.h"
#include "common.h"
#include "pe1/asset_tim.h"
#include "pe1/font.h"
#include "pe1/render_buffers.h"

void SetDispMask(int arg0);
void ClearImage(RECT *rect, int r, int g, int b);
void Draw_InitBuffers(void);
int CdRom_SetSeekPos(unsigned int arg0);
void Gpu_RenderFrame(void);

/* The draw-enabled byte is read signed by the display queries and unsigned
 * by the sequence-elapsed query. */
typedef union GpuDrawEnabledFlag {
    signed char value;
    unsigned char bits;
} GpuDrawEnabledFlag;

extern GpuDrawEnabledFlag g_DrawEnabled;
extern short g_SeqElapsed;
extern char D_800F34F8[];
extern char D_8010BD00[];
extern char D_80120D08[];
extern char D_801ED800[];
extern int g_ActiveDrawSlot;
extern unsigned char g_ScreenTransitionState[];
extern volatile unsigned char D_800B0DC6;
extern int g_SceneDispatchToken;

extern volatile int D_800B0E24;
extern volatile int D_800B0E28;
extern volatile int D_800B0E2C;
extern volatile int D_800B0E30;
extern volatile int D_800B0E34;
extern volatile int D_800B0E40;
extern volatile int D_800B0E44;
extern volatile int D_800B0E48;
extern volatile int D_800B0E5C;
extern volatile int D_800B0E60;
extern volatile int D_800B0E68;

int Gpu_InitDisplay(int mode) {
    int mode_reg;
    int one;
    register volatile int *slots asm("$18");
    register int ptr asm("$2");
    int ptr2;
    register int const_8000 asm("$4");
    int const_48000;
    RECT rect;
    int tmp;

    mode_reg = mode;
    VSync(0);
    SetDispMask(0);
    PutDispEnv(g_RenderDispEnvArray);

    rect.x = 0;
    rect.y = 0;
    rect.w = 0x140;
    rect.h = 0x1C0;
    one = 1;
    ClearImage(&rect, 0, 0, one);
    DrawSync(0);

    const_48000 = 0x48000;
    slots = (volatile int *)g_OtBufferTable;
    asm volatile("" : "=r"(slots) : "0"(slots));

    ptr = (int)D_800F34F8;
    ptr2 = ptr + 0x1800;
    D_800B0E24 = ptr;
    ptr += 0x6000;
    D_800B0E28 = ptr2;
    D_800B0E2C = ptr;
    ptr += 0xE000;
    D_800B0E30 = ptr;

    ptr = (int)D_8010BD00;
    ptr2 = (int)D_80120D08;
    D_800B0E40 = ptr;
    ptr = ptr2 + 0x1C98;
    D_800B0E34 = ptr2;
    ptr2 += 0x5C98;
    const_8000 = 0x8000;
    g_OtBufferTable[0] = (u8 *)ptr;
    ptr += const_8000;
    g_RenderOtBufferBaseAlt[0] = ptr2;
    ptr2 = ptr + 0x2400;
    D_800B0E44 = ptr;
    ptr += 0x4800;
    g_RenderScratchBufferBase[0] = ptr;
    asm volatile("" : "=r"(ptr) : "0"(ptr));
    ptr += const_48000;
    D_800B0E48 = ptr2;
    ptr2 = ptr + 0x4000;
    g_RenderFrontBufferBase[0] = ptr;
    ptr += const_8000;
    g_RenderBackBufferBase[0] = ptr2;
    ptr2 = ptr + 0x3800;
    D_800B0E5C = ptr2;
    tmp = g_StrFileDirBuffer;
    g_RenderAnimBufferBase[0] = (u8 *)ptr;
    ptr += 0x7000;
    D_800B0E60 = ptr;
    g_SceneLoadScratchBuffer[0] = (int)D_801ED800;
    g_LoadedSceneAssetBlock[0] = tmp - 8;
    D_800B0E68 = tmp;

    Draw_InitBuffers();
    CdRom_SetSeekPos(2);

    do {
        ClearOTagR(slots[g_ActiveDrawSlot], 0x1000);
        Render_SetCDDCSlot();
        Gpu_RenderFrame();
    } while ((g_ScreenTransitionState[0] & 3) != one);

    D_800B0DC6 = 0;
    Menu_ConsumeEquipSlotFlag();

    if (mode_reg == 1) {
        g_SceneDispatchToken = 0xA80830C8;
    } else if (mode_reg == 3) {
        g_SceneDispatchToken = 0xA80651C8;
    }
    return 0;
}

inline int Draw_GetDrawEnabled(void) {
    return g_DrawEnabled.value;
}

inline short Seq_GetElapsed(void) {
    if (g_DrawEnabled.bits != 0) {
        return g_SeqElapsed;
    }
    return -1;
}

extern signed char D_800B0DBB;

/* Both queries are integrated here; the inlined calls leave retail's
 * otherwise empty eight-byte frame and the repeated enabled test. */
int Gpu_CheckDrawStatus(void) {
    if (Draw_GetDrawEnabled()) {
        if (Seq_GetElapsed() > 0) {
            if (D_800B0DBB != 0) {
                return 2;
            }
            return 1;
        }
    }
    return 0;
}

TimFile *Gpu_GetTimTableEntry(TimRelativeTable *table, int index) {
    return (TimFile *)((u8 *)table + table->offsets[(s16)index]);
}

void Gpu_LoadTimTable(TimRelativeTable *table, int count) {
    int i;

    for (i = 0; i < count; i++) {
        u32 offset = table->offsets[(s16)i];
        Asset_LoadTimImage((TimFile *)((u8 *)table + offset));
    }
}
