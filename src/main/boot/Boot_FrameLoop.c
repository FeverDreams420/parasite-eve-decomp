/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/scene_transition.h"
#include "pe1/render_camera.h"
#include "pe1/psyq_gpu.h"
#include "pe1/game_state.h"
#include "pe1/room_floor.h"
#include "pe1/scene_room_load.h"
#include "pe1/scene_dispatch.h"
#include "pe1/render_tint.h"
#include "pe1/render_animation_frame.h"
#include "pe1/boot_disc_check.h"
#include "pe1/battle_runtime.h"
#include "pe1/pad_state.h"
#include "pe1/battle_status.h"
#include "pe1/field_movement.h"

/* Field frame loop and its scene flush: Boot_FlushSceneFast is the exit tail
 * that Boot_RunFrame repeats after its loop. Contiguous -G8 pair. */
/* COMMON supplies small-data metadata to MASPSX; MASPSX_FORCE_G0 keeps
 * every other word absolute. The frame counter D_8009CDA4 is read
 * absolutely by CD_TrackReadFlow, so only its size is given here. */
u32 D_8009D280;
int D_8009D1C4;
unsigned int g_GameStateFlags;
u32 D_8009D1A0;
u32 D_8009CDA4[1];

#include "pe1/boot.h"

void Gpu_InitPipeline(void);
void Field_HandleStateTransition(void);
void ClearOTagR(u32 *address, int length);
void Scene_UpdateEntityList(void);
void Entity_FrameUpdate(void);
int Gpu_CheckDrawStatus(void);
int func_80122040(void);
void func_80121A00(void);
void Gpu_ClearOnFlag(void);
void Render_Update(void);
void Menu_DrawTextboxEntries(void);
void Render_SetGteScreenOffset(void);
void func_800E01BC(void);
void Render_ResetGteScreenOffset(void);
int Render_SetCDDCSlot(void);
void Gpu_RenderFrame(void);
int Render_SetFadeColour(unsigned int arg0);
void Sys_FinishFrame(void);
void ClearImage(RECT *rect, int r, int g, int b);
void Akao_Cmd_F1(void);
void Render_Noop(int mode);
int Asset_UnloadTableEntries(void);

void Boot_FlushSceneFast(void) {
    RECT rect;
    u32 status;
    u32 first_status;

    if (g_GameState.flags & 0x200) {
        rect.w = 0x140;
        rect.x = 0;
        rect.y = 0;
        rect.h = 0x1C0;
        ClearImage(&rect, 0, 0, 1);
    }
    DrawSync(0);
    Akao_Cmd_F1();
    Render_Noop(1);
    Asset_UnloadTableEntries();
    status = g_GameState.flags | 2;
    g_GameStateFlags = (g_GameStateFlags | 0x40) & ~0x3800;
    first_status = status & ~0x800;
    g_GameState.flags = first_status;
    if (status & 0x200) {
        g_GameState.flags = (first_status | 2) & 0xFFFF7DFF;
    }
}

void Boot_RunFrame(void)
{
    RECT clear_rect;
    u32 flags;
    u32 status;
    u32 *game_flags;
    u8 *scene_flags;
    u8 *fade;

    Gpu_InitPipeline();
    if (D_8009D1C4 == D_8009D280) {
        scene_flags = D_800B0CEA;
        game_flags = (u32 *)(scene_flags - 0x12);
        fade = (u8 *)&D_800BCFE8;
        do {
            D_8009CDD8 = 0;
            *scene_flags = 0;
            Field_HandleStateTransition();
            flags = D_8009D1A0;
            D_8009D1A0 = flags & ~0x30u;

            if (!(*(u32 *)(scene_flags - 0x12) & 0x8000) && D_8009CDA4[0] &&
                (D_8009D1F4 & 4) &&
                !(D_8009D238 & 0xB0002380u)) {
                D_8009D1A0 =
                    ((flags & 1) ? (flags & ~0x30u) | 0x20u
                                 : (flags & ~0x30u) | 0x10u) ^ 1u;
            }

            if (!(D_8009D1A0 & 1)) {
            if (!(*game_flags & 0x200)) {
                ClearOTagR(((u32 **)((u8 *)game_flags + (D_8009CDDC << 2)))[0x58],
                           0x1000);
            }
            D_8009D250 = D_8009D250 + 1;
            Scene_UpdateEntityList();
            Entity_FrameUpdate();

            if (!(*game_flags & 0x100)) {
                if ((s8)Gpu_CheckDrawStatus() != 0) {
                    if ((s8)func_80122040() == 0) {
                        func_80121A00();
                        Gpu_ClearOnFlag();
                        continue;
                    }
                } else if (!(*game_flags & 0x200)) {
                    Render_Update();
                    Menu_DrawTextboxEntries();
                    Render_SetGteScreenOffset();
                    func_800E01BC();
                    Render_ResetGteScreenOffset();
                    Render_SetCDDCSlot();
                }
                Gpu_RenderFrame();
                if (!D_8009CDA4[0] && *(u32 *)fade == 0xFF00FFu &&
                    *(s16 *)(fade + 4) == 0xFF && (fade[6] & 0x40)) {
                    Render_SetFadeColour(15);
                }
            }
            } else {
            VSync(2);
            }

            if (!(D_800B0CD8 & 0x4200) &&
                (D_8009D26C & 0x0F000006u) == 0x0F000006u) {
                Sys_Shutdown();
            }
            if (!(*game_flags & 0x100)) {
                Sys_FinishFrame();
            }
            D_8009CDA4[0]++;
            if ((D_8009D1A0 & 0x2000) && (*game_flags & 0x800)) {
                break;
            }
loop_end_check:
            ;
        } while (D_8009D1C4 == D_8009D280);
    }

    if (D_800B0CD8 & 0x200) {
        clear_rect.x = 0;
        clear_rect.y = 0;
        clear_rect.w = 0x140;
        clear_rect.h = 0x1C0;
        ClearImage(&clear_rect, 0, 0, 1);
    }
    DrawSync(0);
    Akao_Cmd_F1();
    Render_Noop(1);
    Asset_UnloadTableEntries();
    {
        u32 mask = ~0x3800u;
        register u32 final_flags asm("$2");
        final_flags = (D_8009D1A0 | 0x40) & mask;
        status = D_800B0CD8 | 2;
        flags = final_flags;
    }
    D_8009D1A0 = flags;
    {
        u32 first_status = status & ~0x800u;
        D_800B0CD8 = first_status;
        if (status & 0x200)
            D_800B0CD8 = (first_status | 2u) & ~0x8200u;
    }
}
