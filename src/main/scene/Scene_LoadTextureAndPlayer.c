#include "pe1/disc_load_state.h"
#include "common.h"
#include "pe1/game_state.h"
#include "pe1/cdrom.h"
#include "pe1/asset_tim.h"
#include "pe1/psyq_gpu.h"
#include "pe1/entity_animation.h"
#include "pe1/pe_image.h"
#include "pe1/save_tail_state.h"
#include "pe1/scene_entity_textures.h"
s32 Overlay_StreamTexturePage(void)
{
    Pe1GameState *state;
    PeSectorTableAddress ranges;
    PeSectorTableAddress nextRanges;
    PeSectorTableAddress first;
    PeSectorTableAddress last;
    s32 retry;
    s32 offset;
    u16 start;
    s32 poll;

    state = &g_GameState;
    if (D_800B0CE0 != g_LoadedTexturePageId) {
        DrawSync(0);
        /* Matching debt: the CD retry restarts from the table address;
         * a loop lets loop.c hoist it (docs/CODE_ORGANIZATION.md). */
restart:
        ranges.entry = D_800930D8;
        nextRanges.entry = ranges.entry + 1;
        retry = -1;
        do {
            offset = ((s8)state->room_type + 0x2B) * 2;
            first.word = offset + ranges.word;
            last.word = offset + nextRanges.word;
            start = *first.entry;
        } while (CdRom_ReadSectorsFromLba(state->pe_image_base_lba + start, state->scene_load_scratch,
                                          *last.entry - start) == retry);
        do {
            poll = CdRom_PollReady();
            if (poll == 0) {
                break;
            }
            if (poll == -1) {
                goto restart;
            }
        } while (1);
        Asset_LoadTimImage((TimFile *)state->scene_load_scratch);
        state->field_bg_cache_bank = (s8)state->room_type;
    }
    return 0;
}

/* Disc-change gating for scene-area transitions. */


int CdRom_DetectDiscChange(void) {
    unsigned int *state = &g_GameState.flags;
    int offset = g_SceneAreaType - 0xA;

    if ((unsigned int)offset < 5) {
        int value = g_SavedSceneAreaType;

        if (g_SceneAreaType != (unsigned char)value) {
            *state |= 0x200000;
        }

        if (((unsigned int)offset >> 1) != ((value - 0xA) / 2)) {
            g_DiscChangeFlags |= 4;
        }
    }

    return 0;
}

#include "common.h"
#include "pe1/scene_transition.h"
#include "pe1/scene_entity_textures.h"
#include "pe1/scene_assets.h"
#include "pe1/cdrom.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"

/* Matching debt: pins, empty barriers, one retry jump, and a
 * 32-byte unused stack reserve retain retail allocation and scheduling.
 * The shifted state view preserves the bank-row address calculation. */
int Scene_LoadEntityTexture(void)
{
    int bank = *D_800B0CE2;
    u32 lba = D_800B0DD8.baseLba;
    Pe1GameState *state = &g_GameState;
    SceneAssetBlob *blob;
    SceneAssetDirectory *directory;
    TimUploadRecord *tim;
    TimUploadRecord *firstTim;
    u32 timOffset;
    SceneBankAssetRecord *record;
    SceneBankAssetRecord *firstRecord;
    register unsigned i asm("$16");
    u32 packed;
    u32 offsetMask;
    int index;
    int result;
    u32 matchingStackReserve[8];
retry:
    switch (state->entity_texture_phase) {
    case 0:
        if (state->flags & 0x200000)
            state->entity_texture_phase = 1;
        else
            state->entity_texture_phase = 6;
        break;
    case 1:
        index = bank + 3;
        if (CdRom_ReadSectorsFromLba(lba + D_800930D8[index],
                state->scene_load_scratch,
                D_800930DA[index] - D_800930D8[index]) != -1)
            state->entity_texture_phase = 2;
        return 1;
    case 2:
        result = CdRom_PollReady();
        if (result == -1) {
            state->entity_texture_phase = 1;
            return 1;
        }
        if (result != 0)
            return 1;
        if (state->flags & 0x20000) {
            if (!(state->flags & 0x80000) && D_8009D25C < 2)
                return 1;
            state->flags |= 0x40000;
        }
        state->entity_texture_phase = 3;
        break;
    case 3:
        blob = state->scene_load_scratch;
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
        timOffset = directory->timEntries & 0x3fffff;
        firstTim = SceneAsset_ResolveOffset(blob, timOffset);
        i = 0;
        if (directory->timEntries >> 22) {
            tim = firstTim;
            do {
                Gpu_LoadTimAsset(tim, blob);
                packed = directory->timEntries;
                ++i;
                ++tim;
            } while (i < (packed >> 22));
        }
        state->entity_texture_phase = 4;
        break;
    case 4:
        index = bank + 8;
        if (CdRom_ReadSectorsFromLba(lba + D_800930D8[index],
                (void *)state->voice_bank_base_1400,
                D_800930DA[index] - D_800930D8[index]) != -1)
            state->entity_texture_phase = 5;
        return 1;
    case 5:
        result = CdRom_PollReady();
        if (result == -1) {
            state->entity_texture_phase = 4;
            return 1;
        }
        if (result != 0)
            return 1;
        state->entity_texture_phase = 6;
        break;
    case 6:
        blob = (SceneAssetBlob *)state->voice_bank_base_1400;
        directory = SceneAsset_ResolveOffset(blob, blob->directoryOffset);
        {
            SceneBankAssetRecord *root = SceneAsset_ResolveOffset(blob, directory->bankRootEntries & 0x3fffff);
            state->bank_slots[0] = SceneAsset_ResolveOffset(blob, root->source.offsetAndId & 0xffffff);
        }
        firstRecord = SceneAsset_ResolveOffset(blob, directory->bankRowEntries & 0x3fffff);
        i = 0;
        if (directory->bankRowEntries >> 22) {
            offsetMask = 0xffffff;
            record = firstRecord;
            do {
                unsigned id;
                union { Pe1GameState *state; u32 address; } destination;
                asm("" : "=r"(record) : "0"(record), "r"(offsetMask));
                id = record->source.bytes.id;
                /* The row id indexes the flattened bank row table. */
                destination.state = state;
                destination.address = id * 4 + destination.address;
                destination.state->bank_rows[0][0] = SceneAsset_ResolveOffset(
                    blob, record->source.offsetAndId & offsetMask);
                packed = directory->bankRowEntries;
                asm volatile("" : "=r"(packed) : "0"(packed));
                ++i;
                ++record;
            } while (i < (packed >> 22));
        }
        state->entity_texture_phase = 0;
        state->loaded_entity_bank = state->requested_entity_bank;
        state->flags &= ~0x200000;
        return 0;
    default:
        return 0;
    }
    goto retry;
}

/* Scene start-up steps 0x20..0x27: area type and disc swap bookkeeping,
 * the entity texture banks, then the player's model and render object. */
s32 Scene_InitEntityPlayer(s32 resume) {
    Pe1GameState *gameState = &g_GameState;
    s8 *renderSetup;
    u32 area;
    u8 saved;

    switch (g_GameState.scene_init_phase) {
    case 32:
        g_GameState.flags |= 0x20000;
        if (resume) {
            saved = g_SceneAreaType;
            g_SceneAreaType = 0xE;
            g_GameState.scene_area_type_backup = saved;
        } else {
            g_SceneAreaType = g_GameState.scene_area_type_backup;
        }
        gameState->scene_init_phase = 0x21;
        return 1;
    case 33:
        gameState->scene_init_phase = 0x22;
        return 1;
    case 34:
        gameState->scene_init_phase = 0x23;
        return 1;
    case 35:
        area = g_SceneAreaType - 0xA;
        if (area < 5) {
            saved = g_SavedSceneAreaType;
            if (g_SceneAreaType != saved) {
                D_800B0CD8 = g_GameState.flags | 0x200000;
            }
            if ((area >> 1) != ((saved - 0xA) / 2)) {
                g_DiscChangeFlags |= 4;
            }
        }
        gameState->scene_init_phase = 0x24;
        return 1;
    case 36:
        if (Scene_LoadEntityTexture() == 1) {
            return 1;
        }
        if (resume) {
            gameState->scene_init_phase = 0x27;
            return 1;
        }
        gameState->scene_init_phase = 0x25;
        return 1;
    case 37:
        g_GameState.story_day_flags |= 4;
        Scene_SetStoryDay(D_800B0CE4[0]);
        gameState->scene_init_phase = 0x26;
        return 1;
    case 38:
        if (Scene_LoadEntityTextures() == 1 && gameState->scene_init_subphase < 0xB) {
            return 1;
        }
        gameState->scene_init_phase = 0x27;
        return 1;
    case 39:
        {
            FieldActor *player = g_PlayerEntity;
            player->model_state.model_header = g_GameState.bank_slots[0];
            player->action_data = g_GameState.bank_rows[0][0x15];
            Entity_SetActionMode((BattleEntity *)player, 0x15);
        }
        {
            FieldActor *player = g_PlayerEntity;
            Render_SetupEntityPrims(&player->render_object, player->model_state.model_header,
                                    (u8 *)(player->allocation_block + 0x50), 0x3C0, 0x100, 0,
                                    0x1C0, 2, &renderSetup, 1);
        }
        {
            FieldActor *player = g_PlayerEntity;
            Render_InitRoomPrimState(&player->render_object);
        }
        {
            FieldActor *player = g_PlayerEntity;
            Render_DrawWithAnim(&player->render_object, player->action_data, 0,
                                &D_800BEA40, g_EntityRenderScratch);
        }
        {
            FieldActor *player = g_PlayerEntity;
            player->render_object.header->shadow_radius = player->render_object.hit_cylinder.radius * 2;
        }
        D_800B0CD8 = g_GameState.flags & ~0x60000;
        if (resume) {
            gameState->flags |= 0x80000;
        } else {
            gameState->flags &= ~0x80000;
        }
        gameState->scene_init_phase = 0x20;
        /* fallthrough */
    default:
        return 0;
    }
}



/* Existing byte symbols for the shared state's pending/current day and flags.
 * Preserve the unsigned pending-byte read before its signed-day conversion. */

int Scene_SetStoryDay(s32 storyDay) {
    Pe1GameState *gameState = &g_GameState;
    u8 flags;

    if (storyDay == -1) {
        u8 pending;
        u8 storyFlags;
        pending = D_800B0CE5;
        storyFlags = D_800B0CE6;
        storyDay = (s8)pending;
        D_800B0CE4[0] = pending;
        D_800B0CE6 = storyFlags | 3;
    }

    if (((g_GameStateFlags & 2) != 0) || ((gameState->flags & 2) != 0)) {
        D_800B0CE6 |= 2;
        g_FieldMoveLock &= ~2U;
    }

    flags = gameState->story_day_flags;
    if ((flags & 4) != 0) {
        gameState->story_day_flags = (flags | 3) & ~4;
    }

    if ((storyDay - 1U) < 8U) {
        if (storyDay != (s8)gameState->pending_story_day) {
            s8 newStoryDay;

            newStoryDay = storyDay;
            gameState->pending_story_day = newStoryDay;
            gameState->current_story_day = newStoryDay;
            gameState->story_day_flags |= 1;
        }
    }

    return 0;
}

/* Streams the area entity bank and rebuilds the scene render object. */
int Scene_LoadEntityTextures(void)
{
    int area = g_SceneAreaType;
    int day = D_800B0CE4[0];
    u32 lba = g_PeImageBaseLba;
    Pe1GameState *state = &g_GameState;
    SceneAssetView *view;
    SceneAssetDirectory *directory;
    TimUploadRecord *tim;
    TimUploadRecord *firstTim;
    SceneBankAssetRecord *firstRecord;
    SceneBankAssetRecord *root;
    RenderObjectHeader *model;
    u32 streamEntries, packed;
    u32 i;
    int result;
    /* Render_SetupEntityPrims output; word 0 doubles as the sector index. */
    int setup[16];

    if (area - 10U >= 5)
        return 0;
    /*
     * Recorded crutch debt: retail's state re-dispatch is a goto restart
     * loop, not reproducible with a structured loop in stock GCC 2.7.2
     * (loop depth would reweight register priorities).
     */
retry:
    switch (state->scene_init_subphase) {
    case 0:
        if (state->story_day_flags & 1) {
            state->scene_init_subphase = 1;
            break;
        }
        if (state->story_day_flags & 2) {
            state->scene_init_subphase = 11;
            break;
        }
        Scene_UpdateBgDraw();
        return 0;
    case 1:
        if (day - 2U >= 5) {
            state->scene_init_subphase = 4;
            break;
        }
        setup[0] = day + 0x26;
        result = CdRom_ReadSectorsFromLba(lba + D_800930D8[setup[0]],
            state->scene_load_scratch,
            D_800930DA[setup[0]] - D_800930D8[setup[0]]);
        Scene_UpdateBgDraw();
        if (result != -1)
            state->scene_init_subphase = 2;
        return 1;
    case 2:
        result = CdRom_PollReady();
        if (result == -1) {
            state->scene_init_subphase = 1;
            return 1;
        }
        if (result != 0) {
            if (!(g_GameStateFlags & 2))
                Scene_UpdateBgDraw();
            return 1;
        }
        state->scene_init_subphase = 3;
        break;
    case 3:
        view = state->scene_load_scratch;
        directory = SCENE_ASSET_AT(view, view->header.directoryOffset);
        firstTim = SCENE_ASSET_AT(view, directory->timEntries & 0x3fffff);
        i = 0;
        if (directory->timEntries >> 22) {
            tim = firstTim;
            do {
                Gpu_LoadTimAsset(tim, view);
                packed = directory->timEntries;
                i++;
                tim++;
            } while (i < packed >> 22);
        }
        state->scene_init_subphase = 4;
        break;
    case 4:
        setup[0] = (state->requested_entity_bank - 10) / 2 * 8 + day + 0x16;
        if (CdRom_ReadSectorsFromLba(lba + D_800930D8[setup[0]],
                state->entity_texture_blob,
                D_800930DA[setup[0]] - D_800930D8[setup[0]]) != -1)
            state->scene_init_subphase = 5;
        if (!(g_GameStateFlags & 2))
            Scene_UpdateBgDraw();
        return 1;
    case 5:
        result = CdRom_PollReady();
        if (result == -1) {
            state->scene_init_subphase = 4;
            return 1;
        }
        if (result != 0) {
            if (!(g_GameStateFlags & 2))
                Scene_UpdateBgDraw();
            return 1;
        }
        state->scene_init_subphase = 6;
        break;
    case 6:
        view = state->entity_texture_blob;
        directory = SCENE_ASSET_AT(view, view->header.directoryOffset);
        firstRecord = SCENE_ASSET_AT(view, directory->bankRowEntries & 0x3fffff);
        i = 0;
        if (directory->bankRowEntries >> 22) {
            do {
                state->bank_rows[0][firstRecord[i].source.bytes.id] = SCENE_ASSET_AT(view, firstRecord[i].source.offsetAndId & 0xffffff);
                packed = directory->bankRowEntries;
                i++;
            } while (i < packed >> 22);
        }
        state->cd_range_read_mode = 0;
        for (i = 0; i < 3; i++)
            state->scene_object_tables[i] = 0;
        streamEntries = directory->streamEntries;
        if (streamEntries & 0xffc00000) {
            SceneCdStreamRecord *first = SCENE_ASSET_AT(view, streamEntries & 0x3fffff);
            i = 0;
            if (streamEntries >> 22) {
                do {
                    state->scene_object_tables[i] = SCENE_ASSET_AT(view, first[i].offset & 0xffffff);
                    i++;
                } while (i < directory->streamEntries >> 22);
            }
            state->cd_range_read_mode = first->cdIndex;
        }
        if (g_GameStateFlags & 2) {
            state->scene_init_subphase = 7;
            return 1;
        }
        if (state->story_day_flags & 2) {
            state->scene_init_subphase = 11;
            break;
        }
        Scene_UpdateBgDraw();
        state->scene_init_subphase = 0;
        state->story_day_flags &= ~3;
        return 0;
    case 7:
        if (CD_ReadSectors(1, state->cd_range_read_mode, 0,
                state->scene_load_scratch, 0x21, 0) == 0) {
            state->scene_init_subphase = 11;
            break;
        }
        return 1;
    case 11:
        state->story_day_flags |= 1;
        state->scene_init_subphase = 12;
        return 1;
    case 12:
        state->scene_init_subphase = 13;
        return 1;
    case 13:
        view = state->entity_texture_blob;
        directory = SCENE_ASSET_AT(view, view->header.directoryOffset);
        firstRecord = SCENE_ASSET_AT(view, directory->bankRowEntries & 0x3fffff);
        i = 0;
        if (directory->bankRowEntries >> 22) {
            do {
                state->bank_rows[0][firstRecord[i].source.bytes.id] = SCENE_ASSET_AT(view, firstRecord[i].source.offsetAndId & 0xffffff);
                packed = directory->bankRowEntries;
                i++;
            } while (i < packed >> 22);
        }
        state->cd_range_read_mode = 0;
        for (i = 0; i < 3; i++)
            state->scene_object_tables[i] = 0;
        streamEntries = directory->streamEntries;
        if (streamEntries & 0xffc00000) {
            SceneCdStreamRecord *first = SCENE_ASSET_AT(view, streamEntries & 0x3fffff);
            i = 0;
            if (streamEntries >> 22) {
                do {
                    state->scene_object_tables[i] = SCENE_ASSET_AT(view, first[i].offset & 0xffffff);
                    i++;
                } while (i < directory->streamEntries >> 22);
            }
            state->cd_range_read_mode = first->cdIndex;
        }
        if (g_PlayerEntity == 0) {
            u32 sceneFlag = state->flags & 2;
            return sceneFlag == 0;
        }
        if (g_GameStateFlags & 2) {
            root = SCENE_ASSET_AT(view, directory->bankRootEntries & 0x3fffff);
            model = SCENE_ASSET_AT(view, root->source.offsetAndId & 0xffffff);
        } else {
            model = state->scene_object_model;
        }
        Render_SetupEntityPrims(&state->scene_object, model,
            state->scene_object_work, 0x2C0, 0x80, 0, 0x1C2, 0, (s8 **)setup, 1);
        if (state->scene_object.animation_source == 0 && g_PlayerEntity != 0) {
            state->scene_object.animation_source = &g_PlayerEntity->render_object;
            state->scene_object.animation_state = 3;
            state->scene_object.animation_id = 0x12;
        }
        state->scene_object.shade = 0x80;
        state->scene_object.lightNegativeY = 0xC;
        state->scene_object.lightPositiveY = 0x18;
        Render_InitRoomPrimState(&state->scene_object);
        Render_DrawWithAnim(&state->scene_object, 0, 0, &D_800BEA40,
            g_EntityRenderScratch);
        state->scene_init_subphase = 0;
        state->story_day_flags &= ~3;
        return 1;
    default:
        return 0;
    }
    goto retry;
}
