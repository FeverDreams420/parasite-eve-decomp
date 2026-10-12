#include "common.h"
#include "pe1/draw_buffers.h"
#include "pe1/psyq_gpu.h"
#include "pe1/game_state.h"
#include "pe1/font.h"
#include "pe1/cdrom.h"
#include "pe1/scene_assets.h"
#include "pe1/cdrom_buffers.h"
#include "pe1/render_buffers.h"
#include "pe1/asset_find.h"
#include "pe1/akao.h"
#include "pe1/sys.h"
#include "pe1/task_queue.h"
#include "pe1/boot_disc_check.h"
#include "pe1/render_animation_frame.h"

/* End-of-frame overlay pass (draw the game-state primitives into the
 * current draw buffer with background clear off and send the queued AKAO
 * commands), soft-reset shutdown and screen vertical-offset update. These
 * contiguous routines share the default compiler profile and display state. */

void Sys_FinishFrame(void) {
    s32 one;

    if (!(g_GameState.flags & 0x200) && (g_GameStateFlags & 0x10)) {
        one = 1;
        g_RenderDrawEnvArray[g_ActiveDrawSlot].isbg = 0;
        g_RenderDrawEnvArray[g_ActiveDrawSlot].dfe = one;
        PutDrawEnv(&g_RenderDrawEnvArray[g_ActiveDrawSlot]);
        DrawPrim(&g_GameState.fade_tpage);
        DrawPrim(&g_GameState.fade_tile);
        g_RenderDrawEnvArray[g_ActiveDrawSlot].isbg = one;
        g_RenderDrawEnvArray[g_ActiveDrawSlot].dfe = 0;
        Akao_Cmd_99_9B_9D(0);
    }
    if (g_GameStateFlags & 0x20) {
        Akao_Cmd_98_9A_9C(0);
    }
}

void Sys_Shutdown(void) {
    int old;

    DsReadBreak();
    Akao_Cmd_D8(0);
    Akao_Cmd_F0();
    Akao_Cmd_F1();
    Spu_Shutdown();

    if (Menu_IsEquipSlotActive() & 0xFF) {
        Menu_ResetEquipSlotState();
    }

    old = g_SceneDispatchToken;
    g_SceneDispatchToken = 0xA9400048;
    D_800A77F4 = old;
    g_GameState.flags |= 0x100;
}

/* Shared draw-environment vertical offset (0..15). */

int Draw_SetBaseY(int unused, u32 y)
{
  register u32 v;
  if (y < 16)
  {
    v = y;
    g_DrawBaseY = (g_RenderDispEnvArray[0].screen.y = (g_RenderDispEnvArray[1].screen.y = v));
  }
  return 0;
}


typedef struct AkaoVoiceActor {
    u8 pad_00[0x0C];
    u8 animGroup;
    u8 animSet;
    u8 animIndex;
    u8 frameSkip;
    u8 pad_10[0x06];
    u16 frame;
    u8 pad_18[0x02];
    u16 targetFrame;
    int frameStep;
    u8 pad_20[0x0A];
    s16 posXInteger; /* integer halves of the 16.16 position */
    u8 pad_2c[0x02];
    s16 posYInteger;
    u8 pad_30[0x02];
    s16 posZInteger;
} AkaoVoiceActor;

typedef struct AkaoVoiceBankState {
    int flags;
    u8 pad_04[0x0D];
    u8 first_dynamic_entry;
    u8 active_id_slot;
} AkaoVoiceBankState;


int Akao_LoadVoiceBank(AkaoVoiceActor *actor) {
    AkaoVoiceActor *actor_reg = actor;
    int frame;
    int step;
    int target;
    int i;
    int entry;
    AkaoVoiceBankState *state;
    int id;

    if (actor_reg == 0) {
        return -1;
    }

    state = (AkaoVoiceBankState *)&g_GameState;
    frame = actor_reg->frame;
    step = actor_reg->frameStep;
    target = actor_reg->targetFrame;

    if (step > 0 && frame < target) {
        int next_frame;
        int extra;
        next_frame = frame + 1;
        extra = actor_reg->frameSkip;
        frame = next_frame + extra;
    } else if (step < 0 && target < frame) {
        int next_frame;
        int extra;
        next_frame = frame - 1;
        extra = actor_reg->frameSkip;
        frame = next_frame - extra;
    }

    if (actor_reg == ((AkaoVoiceActor *)D_8009D254) && (D_8009D1A0 & 2) == 0 && (state->flags & 0x800000) == 0) {
        u16 *entry_ids;
        entry_ids = D_80094488[0].ids;
        entry = 0;
        for (i = 0; i < 4; entry_ids += 4, i++, entry++) {
            if (D_80094488[entry].index == actor_reg->animIndex) {
                int entry_frame = D_80094488[entry].frame;
                if ((step > 0 && entry_frame >= target && entry_frame < frame) ||
                    (step < 0 && frame < entry_frame && entry_frame <= target)) {
                    id = entry_ids[state->active_id_slot];
                    if (id != 0) {
                        Asset_Find08w(id, 0, actor_reg->posXInteger,
                                      actor_reg->posYInteger, actor_reg->posZInteger);
                    }
                }
            }
        }
    }

    i = 4;
    if (i >= state->first_dynamic_entry + 4) {
        return 0;
    }

    entry = 4;
    do {
        if (D_80094488[entry].group == actor_reg->animGroup &&
            D_80094488[entry].set == actor_reg->animSet &&
            D_80094488[entry].index == actor_reg->animIndex) {
            int entry_frame = D_80094488[entry].frame;
            if ((step > 0 && entry_frame >= target && entry_frame < frame) ||
                (step < 0 && frame < entry_frame && entry_frame <= target)) {
                id = D_80094488[entry].ids[state->active_id_slot];
                if (id != 0) {
                    Asset_Find08w(id, 0, actor_reg->posXInteger,
                                  actor_reg->posYInteger, actor_reg->posZInteger);
                }
            }
        }

        entry++;
        i++;
    } while (i < state->first_dynamic_entry + 4);

    return 0;
}



void Sys_SyncShutdown(void)
{
    Spu_InitHardware();
    Akao_Cmd_F0();
    Akao_Cmd_F1();
    Akao_Cmd_98_9A_9C(0);

    while (DsReset() != 1) {
        VSync(0);
    }

    while (DsSystemStatus() != 1) {
        VSync(0);
    }

    D_800B0DD4 = DsShellOpen();
}

/* Lay out the memory buffers, then reset the scene voice-bank state. */
void Boot_InitGameState(void)
{
    Boot_InitMemoryLayout();
    GameState_Reset();
}

void GameState_Reset(void)
{
    int i;
    int j;
    Pe1GameState *base = &g_GameState;
    u32 *p;
    int minusOne;
    AkaoVoiceBankEntry *pair;
    int pairOffset;
    u8 flags;
    register int fill asm("$3");
    u32 voiceBase;
    u32 drawMode;
    i = 0;
    p = (u32 *)base;
    base->flags = 3;
    base->saved_state_004 = 10;
    base->field_bg_cache_id = -1;
    base->room_type = 2;
    PE1_COMPILER_LAUNDER_AFTER_MEM(minusOne, -1, base->room_type);
    base->requested_entity_bank = 11;
    base->field_bg_cache_bank = minusOne;
    base->loaded_entity_bank = 0;
    base->current_story_day = minusOne;
    base->pending_story_day = 1;
    base->story_day_flags = 0;
    base->unk_00f = 0;
    base->cd_range_read_mode = 0;
    base->bank_state_11 = 0;
    base->bank_state_12 = 0;
    base->scene_area_type_backup = 0;
    /* Clear the embedded scene object and its tail, 49 words from 0x14. */
    for (; (unsigned int)i < 49; i++, p++)
        p[5] = 0;
    base->scene_audio.tracks.reserved[1] = 8;
    base->scene_audio.tracks.reserved[0] = 0;
    base->scene_audio.tracks.banks[1] = -1;
    base->scene_audio.tracks.banks[0] = -1;
    j = 0;
    do {
        base->scene_audio.tracks.keys[j][1] = -1;
        base->scene_audio.tracks.keys[j][0] = -1;
        j++;
    } while (j < 2);
    pair = D_80094488;
    pairOffset = 0;
    base->track_volumes[1] = 0x7F;
    base->track_volumes[0] = 0x7F;
    base->scene_audio.tracks.pending_key = 0x27;
    base->scene_audio.tracks.pending_bank = 0x0D;
    base->scene_audio.tracks.movie_wide = 1;
    base->scene_audio.tracks.movie_active = 0;
    base->scene_audio.tracks.movie_frame = -1;
    base->scene_audio.tracks.movie_volume = 0x98;
    base->scene_audio.tracks.movie_index = -1;
    base->pending_sample_bank = -1;
    base->pending_stream_banks[1] = 0;
    base->pending_stream_banks[0] = 0;
    for (; pairOffset < 4; pairOffset++, pair++) {
        pair->ids[1] = 0;
        D_80094488[pairOffset].ids[0] = 0;
    }
    drawMode = 0xE1000440;
    j = 2;
    base->bank_value_f6 = 0x30;
    base->bank_value_f7 = 0x7F;
    base->bank_value_f8 = 0x100;
    base->bank_value_fa = 0x800;
    base->fade_tile.tag.len = 3;
    base->fade_tile.code = 0x60;
    base->fade_tile.w = 0x140;
    base->fade_tile.h = 0xE0;
    base->fade_tpage.tag.len = 1;
    flags = base->fade_tile.code;
    fill = 0x20;
    base->scene_init_phase = fill;
    base->fade_tile.r = fill;
    base->fade_tile.g = fill;
    base->fade_tile.b = fill;
    voiceBase = base->voice_bank_base;
    base->entity_texture_phase = 0;
    base->scene_init_subphase = 0;
    base->tim_load_state = 0;
    base->cd_read_phase = 0;
    base->cd_track_phase = 0;
    base->memcard_init_phase = 0;
    base->cd_transition_phase = 0;
    base->memcard_init_phase = 0;
    base->fade_tile.x = 0;
    base->fade_tile.y = 0;
    base->fade_tpage.mode = drawMode;
    base->scene_object_model = 0;
    base->room_geometry_table = 0;
    base->bank_asset_table = 0;
    base->fade_tile.code = flags | 2;
    base->bank_work_base = voiceBase;
    base->bank_work_end = voiceBase + 0x1400;
    base->bank_work_far_end = voiceBase + 0x2800;
    for (; j >= 0; j--)
        base->scene_object_tables[j] = 0;
    for (j = 1; j >= 0; j--)
        base->startup_resource_tables[j] = 0;
    base->startup_resource_blob = 0;
}



void Boot_InitMemoryLayout(void) {
    register u8 *ptr asm("$2");
    u8 *ptr2;
    register s32 const_8000 asm("$4");
    s32 const_48000;
    u8 *tmp;

    const_48000 = 0x48000;

    ptr = D_800F34F8;
    ptr2 = ptr + 0x1800;
    D_800B0E24 = ptr;
    ptr += 0x6000;
    D_800B0E28 = ptr2;
    D_800B0E2C = ptr;
    ptr += 0xE000;
    D_800B0E30 = ptr;

    ptr = D_8010BD00;
    ptr2 = D_80120D08;
    D_800B0E40 = ptr;
    ptr = ptr2 + 0x1C98;
    D_800B0E34 = ptr2;
    ptr2 += 0x5C98;
    const_8000 = 0x8000;
    g_OtBufferTable[0] = ptr;
    ptr += const_8000;
    g_RenderOtBufferBaseAlt[0] = ptr2;
    ptr2 = ptr + 0x2400;
    D_800B0E44 = ptr;
    ptr += 0x4800;
    g_RenderScratchBufferBase[0] = ptr;
    ptr += const_48000;
    D_800B0E48 = ptr2;
    ptr2 = ptr + 0x4000;
    g_RenderFrontBufferBase[0] = ptr;
    ptr += const_8000;
    g_RenderBackBufferBase[0] = ptr2;
    ptr2 = ptr + 0x3800;
    D_800B0E5C = ptr2;
    tmp = g_StrFileDirBuffer;
    g_RenderAnimBufferBase[0] = ptr;
    ptr += 0x7000;
    D_800B0E60 = ptr;
    g_SceneLoadScratchBuffer[0] = D_801ED800;
    g_LoadedSceneAssetBlock[0] = tmp - 8;
    D_800B0E68 = tmp;
}
