/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"
#include "pe1/boot.h"
#include "pe1/card_obj.h"
#include "pe1/display_env.h"
#include "pe1/draw_buffers.h"
#include "pe1/field_sfx.h"
#include "pe1/font.h"
#include "pe1/game_state.h"
#include "pe1/game_state_types.h"
#include "pe1/gte.h"
#include "pe1/menu_inventory.h"
#include "pe1/pad_state.h"
#include "pe1/psyq_callbacks.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_spu_api.h"
#include "pe1/scene_dispatch.h"
#include "pe1/scene_flags.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s32 g_SceneFlagBits;
s32 g_BattleControlFlags;
s32 g_FieldRenderFlags;
u32 g_FieldPadBits;
u32 D_8009D2D4;

/* Boot-time system initialization: Boot_InitSystem, the subsystem and global
 * reset it is followed by, the start-up display reset, the vsync callback and
 * the pad start-up (G0341/G0342). */

void Render_ResetScene(int arg0, int arg1);
void InitGeom(void);
void SetGeomOffset(int x, int y);
void SetGeomScreen(int h);
void MemCard_InitManager(void);
void Boot_InitMemCard(void);
void DsInit(void);

void Boot_InitSystem(void) {
    ResetCallback();
    Render_ResetScene(0x140, 0xE0);
    SpuInit();
    InitGeom();
    SetGeomOffset(0xA0, 0x70);
    SetGeomScreen(0xF0);
    MemCard_InitManager();
    Boot_InitMemCard();
    DsInit();
    DsSetDebug(0);
}

#define NULL ((void *)0)
u32 Draw_FlushPrimQueue(void);
M2C_UNK Save_InitSystem();
M2C_UNK Script_InitNodePool();
M2C_UNK Entity_ResetStateGlobals();
M2C_UNK Entity_ResetAllPools();
M2C_UNK SsInit();
void Menu_SetEquipSlotIndex(int index);
void Boot_BuildRenderFlagTable(void);
M2C_UNK Save_PostInitStub();
M2C_UNK CdRom_InitScreenState();
M2C_UNK Draw_InitGpuHwRegs();
#define g_FrameRngCounter (g_FrameRngCounter[0])

void Boot_InitSubsystems(void) {
    u32 var_s0;

    g_SceneDispatchCur = 0;
    g_SceneDispatchToken = 0;
    g_GameStateFlags = 0;
    g_FrameRngCounter = 0;
    g_ActiveDrawSlot = 0;
    Draw_InitGpuHwRegs();
    for (var_s0 = 0; var_s0 < 0x7D0U; var_s0++) {
        Draw_FlushPrimQueue();
    }
    Boot_BuildRenderFlagTable();
    SsInit();
    VSyncCallback(NULL);
    VSyncCallback(&Boot_VsyncCallback);
    Menu_SetEquipSlotIndex(0);
    Save_InitSystem();
    Save_PostInitStub();
    CdRom_InitScreenState();
    Script_InitNodePool();
    Entity_ResetStateGlobals();
    Entity_ResetAllPools();
    Script_ClearSfxTable();
    Menu_ConsumeEquipSlotFlag();
}

void ResetGraph(int arg0);
void SetGraphDebug(int level);
void ClearImage(RECT *rect, int r, int g, int b);

void Render_ResetScene(int width, int height)
{
    s16 rect[4];

    ResetGraph(0);
    SetGraphDebug(0);
    SetDispMask(0);

    rect[2] = 0x3FF;
    rect[3] = 0x200;
    rect[0] = 0;
    rect[1] = 0;
    ClearImage(rect, 0, 0, 1);

    g_RenderDispEnvArray[0].isrgb24 = 1;
    g_RenderDispEnvArray[1].isrgb24 = 1;
    SetDefDispEnv(&g_RenderDispEnvArray[0], 0, height, width, height);
    SetDefDispEnv(&g_RenderDispEnvArray[1], 0, 0, width, height);

    g_RenderDispEnvArray[1].screen.y = 8;
    g_RenderDispEnvArray[0].screen.y = 8;
    g_RenderDispEnvArray[1].screen.x = 0;
    g_RenderDispEnvArray[0].screen.x = 0;
    g_RenderDispEnvArray[0].screen.h = g_RenderDispEnvArray[1].screen.h = height;
    SetDefDrawEnv(&g_RenderDrawEnvArray[0], 0, 0, width, height);
    SetDefDrawEnv(&g_RenderDrawEnvArray[1], 0, height, width, height);

    g_RenderDrawEnvArray[1].isbg = 1;
    g_RenderDrawEnvArray[0].isbg = 1;
    g_RenderDrawEnvArray[1].dtd = 1;
    g_RenderDrawEnvArray[0].dtd = 1;
    g_RenderDrawEnvArray[1].dfe = 0;
    g_RenderDrawEnvArray[0].dfe = 0;
    g_RenderDrawEnvArray[1].tpage = 0;
    g_RenderDrawEnvArray[0].tpage = 0;
    g_RenderDrawEnvArray[1].r0 = 0;
    g_RenderDrawEnvArray[0].r0 = 0;
    g_RenderDrawEnvArray[1].g0 = 0;
    g_RenderDrawEnvArray[0].g0 = 0;
    g_RenderDrawEnvArray[1].b0 = 0;
    g_RenderDrawEnvArray[0].b0 = 0;
    g_ActiveDrawSlot = 0;

    PutDispEnv(&g_RenderDispEnvArray[0]);
}

void Scene_TickTimers(void);

void PadInitDirect(char *arg0, char *arg1);
void PadStartCom(void);

void Boot_VsyncCallback(void) {
    Draw_FlushPrimQueue();
    Scene_TickTimers();
}

void Boot_InitMemCard(void) {
    PadInitDirect(g_AnalogStickState, g_AnalogStickState + 0x22);
    PadStartCom();
}

/* Field render-flag table setup and the helper that stores one table entry
 * at the bit index of a single-bit mask. The helper's only caller is the
 * table builder that precedes it (main_tu_evidence G0344). */

void Boot_StoreTableEntry(u32 mask, int value);

void Boot_BuildRenderFlagTable(void) {
    s32 *entry;
    u32 i;

    i = 0;
    entry = g_FieldRenderFlagTable;
    g_SceneFlagBits = 0;
    g_BattleControlFlags = 0;
    D_8009D2D4 = 0;
    g_FieldRenderFlags = 0;
    g_FieldPadBits = 0;
    do {
        *entry = 0;
        i += 1;
        entry += 1;
    } while (i < 0x20U);
    Boot_StoreTableEntry(1, 0x4000);
    Boot_StoreTableEntry(0x80, 0x1000);
    Boot_StoreTableEntry(0x100, 0x2000);
    Boot_StoreTableEntry(8, 0x10);
    Boot_StoreTableEntry(0x20, 0x40);
    Boot_StoreTableEntry(0x40, 0x80);
    Boot_StoreTableEntry(0x10, 0x20);
    Boot_StoreTableEntry(2, 1);
    Boot_StoreTableEntry(4, 8);
    Boot_StoreTableEntry(0x200, 0x2000);
    Boot_StoreTableEntry(0x400, 0x4000);
    Boot_StoreTableEntry(0x2000, 0x8000);
    Boot_StoreTableEntry(0x01000000, 0x100);
    Boot_StoreTableEntry(0x02000000, 0x200);
    Boot_StoreTableEntry(0x04000000, 0x400);
    Boot_StoreTableEntry(0x08000000, 0x800);
    Boot_StoreTableEntry(0x10000000, 0x1000);
    Boot_StoreTableEntry(0x20000000, 0x2000);
    Boot_StoreTableEntry(0x40000000, 0x4000);
    Boot_StoreTableEntry(0x80000000, 0x8000);
    g_GameStateFlags = g_GameStateFlags | 0x4000;
}

void Boot_StoreTableEntry(u32 mask, int value) {
    int index;

    gte_ldlzcs(mask);
    index = 31;
    if (mask != 0x80000000) {
        gte_stlzcr(&mask);
        index -= mask;
    }
    D_800A76F0[index] = value;
}

u32 D_8009D1F4;
unsigned int D_8009D1E4;
u32 D_8009D238;
u32 D_8009D2A8;

int PadGetState();
int PadInfoMode(int arg0, int mode, int index);
void PadSetMainMode(int arg0, int byte1, int byte2);

void Field_HandleStateTransition(void) {
    u32 flags;
    u32 pad_bits;
    register u32 base_bits asm("$3");
    u32 old_pad_bits;
    register u32 pad_mask asm("$2");
    u32 active_pad;
    u32 save_pad;
    u32 final_pad;
    u32 final_old;
    u32 changed;
    u32 mask;
    u32 sequence_mask;
    u32 menu_open;
    u32 pad;
    u32 inverted;
    int mode;
    u16 i;
    u8 analog;

    if (PadGetState(0) == 0) {
        u32 initial_flags = g_GameStateFlags;
        if ((initial_flags & 0x4001) == 0) {
            D_8009D1F4 = 4;
            g_FieldPadBits = 4;
            D_8009D1E4 = 0;
            g_GameStateFlags = initial_flags | 0x4000;
            return;
        }
        g_GameStateFlags = initial_flags | 0x4000;
    }

    mode = D_800BE9A0 & 0xF000;
    if (!((mode != 0x4000) && (mode != 0x7000))) {

    if (g_GameStateFlags & 0x4000) {
        mode = PadGetState(0);
        if (mode == 2) {
            g_GameStateFlags = g_GameStateFlags & ~0x4000;
        } else if (mode == 1) {
            PadSetAct(0, D_8009D1C0, 2);
        } else if (mode == 6 && PadInfoMode(0, 2, 0) != 0) {
            if ((g_GameStateFlags & 0x8000) == 0) {
                PadSetMainMode(0, 1, 0);
                g_GameStateFlags = g_GameStateFlags | 0x8000;
            } else {
                PadSetActAlign(0, D_800921F8);
                g_GameStateFlags = g_GameStateFlags & ~0x4000;
            }
        }
    }

    pad = D_800BE9A2;
    pad_mask = 0xFFFF9FFF;
    base_bits = (~pad) & pad_mask;
    asm volatile("" : "=r"(pad) : "0"(pad));
    old_pad_bits = g_FieldPadBits;
    D_8009D238 = old_pad_bits;
    inverted = ~pad;
    pad_bits = base_bits;
    if (inverted & 0x2000) {
        pad_bits = base_bits | 0x4000;
    }
    if (inverted & 0x4000) {
        pad_bits |= 0x2000;
    }

    g_FieldPadBits = 0;
    for (i = 0; i < 0x20; i++) {
        if (((u16)pad_bits & D_800A76F0[i]) != 0) {
            g_FieldPadBits |= 1u << i;
            D_800A7770[i]++;
        } else {
            D_800A7770[i] = 0;
        }
    }

    flags = g_GameStateFlags;
    if ((flags & 1) != 0) {
        active_pad = g_FieldPadBits;
        if ((int)active_pad < 0) {
            mask = ((active_pad ^ D_8009D2D4) & active_pad) & 0x7000007E;
            if (mask != 0) {
                sequence_mask = D_80092200[D_8009D2A8];
                if ((mask & sequence_mask) == sequence_mask) {
                    D_8009D2A8++;
                    if (D_8009D2A8 == 9) {
                        g_SceneDispatchToken = 0xAA108448;
                        g_GameStateFlags = flags | 0x12000;
                    }
                } else {
                    D_8009D2A8 = 0;
                }
            }
        } else {
            D_8009D2A8 = 0;
        }
        save_pad = g_FieldPadBits;
        D_8009D2D4 = save_pad;
    }

    menu_open = g_GameState.flags;
    if (menu_open & 0x400) {
        g_FieldPadBits &= 0x40FFDC7F;
    }
    if (menu_open & 0x200) {
        g_FieldPadBits &= 0x60FFDB04;
        if (g_GameState.scene_audio.tracks.movie_index != 1) {
            g_FieldPadBits &= 0xDFFFFCFF;
        }
    }
    if (g_FieldPadBits & 0x10000080) {
        g_FieldPadBits &= 0x7FFFFFFF;
    }
    if (g_FieldPadBits & 0x40000401) {
        g_FieldPadBits &= 0xEFFFFF7F;
        g_FieldPadBits &= 0x7FFFFFFF;
    }
    if (g_FieldPadBits & 0x20000300) {
        g_FieldPadBits &= 0xBFFFFBFE;
        g_FieldPadBits &= 0xEFFFFF7F;
        g_FieldPadBits &= 0x7FFFFFFF;
    }

    if ((D_800BE9A0 & 0xF000) == 0x7000) {
        g_FieldPadBits &= ~0x79;
        menu_open = MenuWidget_FindByModeAndSelectedBase(1, 0) != 0;

        if (menu_open) {
            analog = D_800BE9A7;
            if (analog < 0x14) {
                g_FieldPadBits |= 0x8;
            } else if (analog >= 0xE7) {
                g_FieldPadBits |= 0x20;
            }

            analog = D_800BE9A6;
            if (analog < 0x14) {
                g_FieldPadBits |= 0x40;
            } else if (analog >= 0xE7) {
                g_FieldPadBits |= 0x10;
            }
        } else {
            analog = D_800BE9A7;
            if (analog < 0x5A) {
                g_FieldPadBits |= 0x8;
                if (analog < 0x14) {
                    g_FieldPadBits |= 0x1;
                }
            } else if (analog >= 0xA1) {
                g_FieldPadBits |= 0x20;
                if (analog >= 0xE7) {
                    g_FieldPadBits |= 0x1;
                }
            }

            analog = D_800BE9A6;
            if (analog < 0x5A) {
                g_FieldPadBits |= 0x40;
                if (analog < 0x14) {
                    g_FieldPadBits |= 0x1;
                }
            } else if (analog >= 0xA1) {
                g_FieldPadBits |= 0x10;
                if (analog >= 0xE7) {
                    g_FieldPadBits |= 0x1;
                }
            }
        }
    }

    final_pad = g_FieldPadBits;
    final_old = D_8009D238;
    changed = final_pad ^ final_old;
    D_8009D1F4 = changed & final_pad;
    D_8009D1E4 = changed & final_old;
    return;
    }

    g_FieldPadBits = 0;
    D_8009D1F4 = 0;
    D_8009D1E4 = 0;
}
