/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_update.h"
#include "pe1/entity_allocation.h"
#include "pe1/entity_animation.h"
#include "pe1/field_actor.h"
#include "pe1/field_collision.h"
#include "pe1/field_sfx.h"
#include "pe1/game_state.h"
#include "pe1/game_state_types.h"
#include "pe1/geom_state.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_state.h"
#include "pe1/player_entity.h"
#include "pe1/render_lighting.h"
#include "pe1/render_shadow.h"
#include "pe1/scene_room_load.h"
#include "pe1/scene_transition.h"
#include "pe1/script_node.h"
#include "pe1/script_queue.h"
#include "pe1/script_state.h"
#include "pe1/textbox.h"
#include "pe1/vector_types.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
u32 *g_ScriptCursor;
ScriptNode *g_ScriptNodePool;
u8 g_ScriptCameraBytes[3];
int g_ScriptCursorAddress;

typedef struct ScriptArgParts {
    s16 low;
    signed int high : 16;
} ScriptArgParts;

typedef struct ScriptCmdArgs {
    int *opcode;
    ScriptArgParts *arg1;
    ScriptArgParts *arg2;
    ScriptArgParts *arg3;
    ScriptArgParts *arg4;
    ScriptArgParts *arg5;
} ScriptCmdArgs;

#define ARG_WORD(ptr) (*(int *)(ptr))
#define ARG_BYTE(ptr) (*(u8 *)(ptr))

void FieldFx_SpawnScriptSprite(Pe1Vec3s *position, int mode, int arg2, int arg3,
                   int arg4, int arg5, int arg6);

int Script_DispatchScriptCmd(ScriptCmdArgs *args)
{
    Pe1Vec3s position;
    Pe1Vec3s position2;
    char map_id[2];
    FieldActor *entity;
    FieldActor *loaded;
    int status;
    int entity_id;
    int initial_entity_id;

    /* Opcode 2100 is accepted and does nothing; its case node is what
     * makes the compiler split the dispatch tree at 2500 as retail does. */
    switch (*args->opcode) {
    case 2100:
        break;
    case 2101:
        g_GameState.flags |= 0x800;
        break;
    case 2200:
        Render_UpdateScrollPosition(&(g_PlayerEntity)->pos_x,
                                    ARG_WORD(args->arg1), ARG_WORD(args->arg2));
        return 1;
    case 2201:
        g_GeomScroll.state.position.originX = ARG_WORD(args->arg1);
        g_GeomScroll.state.position.originY = ARG_WORD(args->arg2);
        break;
    case 2300:
        Script_SetCollisionFlag(ARG_WORD(args->arg1) != 0);
        return 1;
    case 2400:
        position.x = args->arg1->high;
        position.y = args->arg2->high;
        position.z = args->arg3->high;
        FieldFx_SpawnScriptSprite(&position, 0, args->arg4->low,
                      ARG_BYTE(args->arg5), 0, 0, 0);
        return 1;
    case 2401:
        g_ScriptCameraBytes[0] = ARG_WORD(args->arg1);
        g_ScriptCameraBytes[1] = ARG_WORD(args->arg2);
        g_ScriptCameraBytes[2] = ARG_WORD(args->arg3);
        break;
    case 2402:
        position2.x = args->arg1->high;
        position2.y = args->arg2->high;
        position2.z = args->arg3->high;
        FieldFx_SpawnScriptSprite(&position2, 1, args->arg4->low,
                      ARG_BYTE(args->arg5), g_ScriptCameraBytes[0],
                      g_ScriptCameraBytes[1], g_ScriptCameraBytes[2]);
        return 1;
    case 2500:
        map_id[0] = ARG_WORD(args->arg1);
        map_id[1] = ARG_WORD(args->arg2);
        loaded = Scene_LoadMap((u8 *)map_id, g_CurrentEntity, 0);
        loaded->pos_x = ARG_WORD(args->arg3);
        loaded->pos_y = ARG_WORD(args->arg4);
        loaded->pos_z = ARG_WORD(args->arg5);
        break;
    case 2600:
        ((RenderShadowActor *)g_CurrentEntity)->shadow_matrix_index = ARG_WORD(args->arg1);
        break;
    case 2601:
        ((RenderShadowActor *)g_CurrentEntity)->shadow_intensity = ARG_WORD(args->arg1);
        break;
    case 2700:
        g_RoomLightTint.r = ARG_WORD(args->arg1);
        g_RoomLightTint.g = ARG_WORD(args->arg2);
        g_RoomLightTint.b = ARG_WORD(args->arg3);
        break;
    case 2800:
        Battle_DrawActiveStatus();
        return 1;
    case 2801:
        Battle_SetWindowBoundsByMode(ARG_BYTE(args->arg1));
        return 1;
    case 2900:
        g_GameState.flags |= 0x400000;
        break;
    case 3000:
        initial_entity_id = ARG_WORD(args->arg1);
        if (initial_entity_id == 0) {
            return 1;
        }
        entity = g_FieldActorListHead;
        entity_id = initial_entity_id;
        if (entity == 0) {
            return 1;
        }
        while (1) {
            if (entity->type_id == entity_id) {
                if (entity->sub_id == ARG_WORD(args->arg2)) {
                    if ((entity->flags & 0x10) == 0) {
                        return 1;
                    }
                }
            }
            entity = entity->next;
            if (entity == 0) {
                break;
            }
        }
        return 1;
    case 3100:
        status = Asset_LoadTimTextures(1);
        if (status == 1) {
            g_ScriptCursor -= 10;
            g_ScriptNodePool->active = status;
            return 0;
        }
        return 1;
    case 3200:
        g_GameState.flags |= 0x8000000;
        break;
    }
    return 1;
}

int Script_SetEntityShadowFlag(void) {
    g_CurrentEntity->flags |= 0x20000000;
    return 1;
}

int Script_ClearEntityShadowFlag(void) {
    g_CurrentEntity->flags &= 0xDFFFFFFF;
    return 1;
}

int Script_BeginSceneTransition(int **arg0) {
    Pe1GameState *state = &g_GameState;
    int **saved = arg0;
    ScriptNode *node;
    unsigned short *node_flags;
    int flags;
    int cursor;

    if (!(state->flags & 0x1000)) {

    node = g_ScriptNodePool;
    node_flags = &node->flags;
    flags = *node_flags;
    if ((flags & 0x20) == 0) {
        *node_flags = flags | 0x20;
        cursor = g_ScriptCursorAddress;
        node->active = 1;
        cursor -= 0xC;
        g_ScriptCursorAddress = cursor;
        return 0;
    }

    Render_BeginSceneLoad();
    Menu_OpenEquipScreen(**saved);
    {
        ScriptNode *tail_node = g_ScriptNodePool;
        int tail_flags;

        state->flags |= 0x9000;
        tail_flags = tail_node->flags;
        g_GameStateFlags |= 4;
        tail_flags &= 0xFFDF;
        tail_node->flags = tail_flags;
    }

    }
ret_one:
    return 1;
}

void Menu_OpenSaveLoadEntryPoint(int arg0);

/* Script interpreter main loop and the jump-to-actor-script opcode; a
 * contiguous -G8 pair sharing the script cursor g_ScriptCursor. */

PE1_STATIC_ASSERT(sizeof(ScriptNode) == 0x2C, interpreter_script_node_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ScriptNode, current) == 0, interpreter_script_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(ScriptNode, active) == 0x10, interpreter_wait_offset);

/* Tentative COMMON declarations preserve small-data metadata for stock
 * maspsx. Storage remains at the existing data/linker addresses. */
ScriptNode *g_ScriptNodePool;
u32 *g_ScriptCursor;

typedef struct {
    int value;
} ScriptJumpOffsetEntry;

int Math_FixedMul(int arg0, int arg1);

/* Script opcodes: random value in a range and a field sound effect on the
 * current actor. Contiguous default-profile handlers. */

int Draw_PackPrimColor(int start, int end);

int Draw_GetTextboxPageState(short arg0);