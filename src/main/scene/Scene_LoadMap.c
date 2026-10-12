#include "common.h"
#include "pe1/player_entity.h"
#include "pe1/render_setup.h"
#include "pe1/scene_assets.h"
#include "pe1/scene_room_load.h"
#include "pe1/scene_archive.h"
#include "pe1/entity_animation.h"
#include "pe1/scene_entity_textures.h"
#include "pe1/render_camera.h"
#include "pe1/game_state.h"
#include "pe1/render_shadow.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */


#include "pe1/entity_allocation.h"

/* COMMON gives MASPSX the small-data size of the free-list head, so it
 * keeps the load delay before the $gp store as ASPSX did. */
FieldActor *g_EntityFreeListHead;

FieldActor *Scene_LoadMap(u8 *scene, FieldActor *after, int allocateFull)
{
    FieldActor *actor;
    FieldActor *available;
    FieldActor *afterNext;
    FieldActor *nextFree;
    u8 *allocation;
    u8 *areaType;
    u32 entry;
    u32 i;
    u32 count;
    u32 value;
    u8 sceneType;
    u32 serial;
    u32 *row;
    u8 **table;
    SceneAssetBlob *archive;
    SceneAssetDirectory *directory;
    SceneBankAssetRecord *bankRows;
    SceneBankAssetRecord *walk;
    struct { u8 *primData; u32 unknown; u32 mode; } renderSetup;

    available = g_EntityFreeListHead;
    if (!available) return 0;
    actor = available;
    nextFree = actor->next;
    g_EntityFreeListHead = nextFree;
    if (after) {
        afterNext = after->next;
        actor->prev = after;
        actor->next = afterNext;
        after->next = actor;
        if (actor->next) actor->next->prev = actor;
    } else {
        if (g_FieldActorListHead) g_FieldActorListHead->prev = actor;
        actor->prev = 0;
        actor->next = g_FieldActorListHead;
        g_FieldActorListHead = actor;
    }

    actor->gravity_x = 0;
    actor->gravity_y = 0x4000;
    actor->gravity_z = 0;
    actor->motion_x = 0;
    actor->motion_y = 0;
    actor->motion_z = 0;
    actor->accel_x = 0;
    actor->accel_y = 0;
    actor->accel_z = 0;
    actor->delta_x = 0;
    actor->delta_y = 0;
    actor->delta_z = 0;

    actor->frame_callback = g_FieldActorTypeCallbacks[scene[0]].frame;
    actor->contact_callback = g_FieldActorTypeCallbacks[scene[0]].contact;
    if (scene[0] == 0) {
        g_PlayerEntity = actor;
        actor->move_factor = 0x10000;
        Battle_InitEquipLists((struct AyaBattleState **)&actor->state);
    } else {
        actor->move_factor = 0x50000;
        actor->state = 0;
    }
    actor->type_id = scene[0];
    actor->sub_id = scene[1];
    if (scene[0] < 10) {
        actor->model_state.allocation_active = g_SceneMapPrimBaseTable[scene[0]];
    } else {
        actor->model_state.allocation_active = 0;
    }
    serial = g_EntitySerialCounter;
    actor->flags = 0x10000000;
    actor->anim_step = 0x10000;
    actor->pad_010 = 0xC8;
    actor->action_data = 0;
    actor->anim.fixed = 0;
    actor->parent = 0;
    actor->collision_face = 0;
    actor->script_cursor_19c = 0;
    actor->script_cursor_1a0 = 0;
    actor->pad_198 = 0;
    actor->move_speed = 0x1000;
    g_EntitySerialCounter = serial + 1;
    actor->field_sfx_id = serial;
    actor->mode = 0;
    ((RenderShadowActor *)actor)->shadow_matrix_index = 0;
    ((RenderShadowActor *)actor)->shadow_intensity = 0x80;
    for (i = 0; i < 0x38; ++i) actor->pad_0AC[i] = 0;
    for (i = 0; i < 3; ++i) actor->task_node_lists[i] = 0;

    table = g_GameState.bank_reset_944[0];
    actor->script_base = table[2 + scene[0]];
    actor->task_node_lists[2] = Task_AllocNode((unsigned int *)actor->script_base, 0);
    actor->rot_x = 0;
    actor->rot_y = 0;
    actor->rot_z = 0;
    g_EntityFreePoolCount++;

    if (actor->model_state.allocation_active) {
    if (actor == g_PlayerEntity) {
        Entity_SetActionMode((BattleEntity *)actor, 0x15);
    } else {
        void *(*rows)[48] = g_GameState.bank_rows;
        row = (u32 *)rows[actor->type_id];
        for (i = 0; i < 0x30; ++i) {
            if (row[i]) {
                Entity_SetActionMode((BattleEntity *)actor, i & 0xFFFF);
                break;
            }
        }
    }

    if (allocateFull) {
        allocation = Entity_AllocBlock(actor->model_state.model_header->prim_block_count * 8 + actor->model_state.model_header->animation_entry_count * 12 + actor->model_state.model_header->part_count * 32 + 0x50);
    } else {
        allocation = Entity_AllocBlock(actor->model_state.model_header->animation_entry_count * 12 + actor->model_state.model_header->part_count * 32);
        actor->flags |= 0x600000A0;
    }
    actor->allocation_block = (int)allocation;

    if (scene[0] == 0 && (areaType = D_800B0CE2)) {
        Render_SetupEntityPrims(&actor->render_object, actor->model_state.model_header, allocation + 0x50, 0x3C0,
                                0x100, 0, 0x1C0, 2, (s8 **)&renderSetup,
                                allocateFull);
    } else {
        archive = *D_800B0E64;
        directory = SceneAsset_ResolveOffset(archive, archive->directoryOffset);
        entry = directory->bankRootEntries;
        bankRows = SceneAsset_ResolveOffset(archive, entry & 0x3FFFFF);
        i = 0;
        if ((entry >> 22) != 0) {
        sceneType = scene[0];
        count = entry >> 22;
        walk = bankRows;
scan_loop:
        if (walk->source.bytes.id != sceneType) {
        ++i;
        if (i < count) { ++walk; goto scan_loop; }
        }
        }

        allocation = (u8 *)actor->allocation_block;
        value = bankRows[i].trailing;
        Render_SetupEntityPrims(&actor->render_object, actor->model_state.model_header, allocation + 0x50,
                                (value >> 6) & 0x3C0, (value >> 9) & 0x180,
                                0, ((bankRows[i].trailing >> 18) & 0xFF) + 0x1C0,
                                (bankRows[i].trailing >> 8) & 0xF, (s8 **)&renderSetup,
                                allocateFull);
    }

    if (actor->action_data) {
        actor->render_object.shade = 0x80;
        actor->render_object.lightNegativeY = 0xC;
        actor->render_object.lightPositiveY = 0x18;
        Render_InitRoomPrimState(&actor->render_object);
        Render_DrawWithAnim(&actor->render_object, actor->action_data,
                            (s16)actor->anim.parts.integer, &D_800BEA40, D_800B89F8);
        actor->render_object.header->shadow_radius = actor->render_object.hit_cylinder.radius * 2;
    }
    } else {
        actor->flags |= 0xE0;
    }
    return actor;
}
