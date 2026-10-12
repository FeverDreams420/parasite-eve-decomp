#include "pe1/geom_state.h"
#include "pe1/game_state.h"
#include "common.h"
#include "pe1/task_node.h"
#include "pe1/vector_types.h"
#include "pe1/render_lighting.h"
#include "pe1/scene_room_load.h"
#include "pe1/field_actor.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/render_shadow.h"

/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
/* ASPSX_VERSION: 2.70 */

typedef struct ScriptArgParts {
    s16 low;
    signed int high : 16;
} ScriptArgParts;

typedef struct TaskScriptArgs {
    int *opcode;
    ScriptArgParts *arg1;
    ScriptArgParts *arg2;
    ScriptArgParts *arg3;
    ScriptArgParts *arg4;
    ScriptArgParts *arg5;
} TaskScriptArgs;

#define ARG_WORD(ptr) (*(int *)(ptr))
#define ARG_BYTE(ptr) (*(u8 *)(ptr))

/* COMMON supplies small-data metadata to MASPSX. */
u32 *D_8009CE00;
TaskNode *D_8009D300;
u8 g_ScriptCameraBytes[3];

void Task_SetCollisionFlag(int value);
void func_800E00CC(Pe1Vec3s *position, int mode, int arg2, int arg3,
                   int arg4, int arg5, int arg6);
void Battle_DrawActiveStatus(void);
void Window_SetBoundsByMode(int mode);

int Task_DispatchScriptCmd(TaskScriptArgs *args)
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
        Render_UpdateScrollPosition(&((FieldActor *)D_8009D254)->pos_x,
                                    ARG_WORD(args->arg1), ARG_WORD(args->arg2));
        return 1;
    case 2201:
        g_GeomScroll.state.position.originX = ARG_WORD(args->arg1);
        g_GeomScroll.state.position.originY = ARG_WORD(args->arg2);
        break;
    case 2300:
        Task_SetCollisionFlag(ARG_WORD(args->arg1) != 0);
        return 1;
    case 2400:
        position.x = args->arg1->high;
        position.y = args->arg2->high;
        position.z = args->arg3->high;
        func_800E00CC(&position, 0, args->arg4->low,
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
        func_800E00CC(&position2, 1, args->arg4->low,
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
        D_800BD025.r = ARG_WORD(args->arg1);
        D_800BD025.g = ARG_WORD(args->arg2);
        D_800BD025.b = ARG_WORD(args->arg3);
        break;
    case 2800:
        Battle_DrawActiveStatus();
        return 1;
    case 2801:
        Window_SetBoundsByMode(ARG_BYTE(args->arg1));
        return 1;
    case 2900:
        g_GameState.flags |= 0x400000;
        break;
    case 3000:
        initial_entity_id = ARG_WORD(args->arg1);
        if (initial_entity_id == 0) {
            return 1;
        }
        entity = D_8009D20C;
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
            D_8009CE00 -= 10;
            D_8009D300->active = status;
            return 0;
        }
        return 1;
    case 3200:
        g_GameState.flags |= 0x8000000;
        break;
    }
    return 1;
}
