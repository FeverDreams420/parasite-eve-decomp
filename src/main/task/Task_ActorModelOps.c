/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
/* Script opcodes: draw the current actor's model through a caller-supplied
 * GTE matrix, the render-mode switch wait that pops the script cursor, and
 * the angle from the current actor to a named actor. Contiguous handlers;
 * the angle opcode reaches its actors through array views, absolute under
 * -G8 as well. */
#include "pe1/render_lighting.h"
#include "pe1/global_slot.h"
#include "common.h"
#include "pe1/task_node.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/boot_disc_check.h"
#include "pe1/entity_frame_update.h"
#include "pe1/geom_state.h"
#include "pe1/player_entity.h"
#include "pe1/battle_entity_anim.h"

#define U8_AT(ptr, off) (*(u8 *)((u8 *)(ptr) + (off)))
#define U16_AT(ptr, off) (*(u16 *)((u8 *)(ptr) + (off)))
#define U32_AT(ptr, off) (*(u32 *)((u8 *)(ptr) + (off)))
#define S32_AT(ptr, off) (*(s32 *)((u8 *)(ptr) + (off)))
#define PTR_AT(ptr, off) (*(u8 **)((u8 *)(ptr) + (off)))

/* COMMON supplies small-data metadata to MASPSX. */
u32 *D_8009CE00;
TaskNode *D_8009D300;
TaskNode *g_TaskNodePool;

GteMatrix *RotMatrix(GteShortVector *rotation, GteMatrix *matrix);
int Anim_BuildRotationMatrices(RenderObjectEntity *object, void *action, int frame, int mode);
void Render_TransformVertices(RenderObjectEntity *object);

int Task_SetGteMatrix(int **args) {
    FieldActor *setup_actor;
    FieldActor *matrix_actor;
    FieldActor *color_actor;
    FieldActor *render_actor;
    FieldActor *flag_actor;
    GteMatrix scale_matrix;
    register GteMatrix *first_matrix asm("$3");
    GteMatrix *second_matrix;
    GteMatrix *third_matrix;
    GteMatrix *translation_matrix;
    int first_draw_slot;
    int second_draw_slot;
    int result;
    u32 *script_ptr;
    TaskNode *task_state;
    u32 flags;

    setup_actor = g_CurrentEntity;
    setup_actor->render_object.model_matrix.translation[0] = Pe1Fixed_Integer(&setup_actor->pos_x);
    setup_actor->render_object.model_matrix.translation[1] = Pe1Fixed_Integer(&setup_actor->pos_y);
    setup_actor->render_object.model_matrix.translation[2] = Pe1Fixed_Integer(&setup_actor->pos_z);
    setup_actor->render_object.table_value2c = (u16)setup_actor->rot_x;
    setup_actor->render_object.table_value2e = (u16)setup_actor->rot_y;
    setup_actor->render_object.table_value30 = (u16)setup_actor->rot_z;
    RotMatrix((GteShortVector *)&setup_actor->render_object.table_value2c,
              (GteMatrix *)&setup_actor->render_object.model_matrix);

    matrix_actor = g_CurrentEntity;
    scale_matrix.m[0][0] = matrix_actor->move_speed;
    scale_matrix.m[1][1] = matrix_actor->move_speed;
    scale_matrix.m[2][2] = matrix_actor->move_speed;
    first_matrix = (GteMatrix *)&matrix_actor->render_object.model_matrix;
    scale_matrix.t[2] = 0;
    scale_matrix.t[1] = 0;
    scale_matrix.t[0] = 0;
    scale_matrix.m[2][1] = 0;
    scale_matrix.m[2][0] = 0;
    scale_matrix.m[1][2] = 0;
    scale_matrix.m[1][0] = 0;
    scale_matrix.m[0][2] = 0;
    scale_matrix.m[0][1] = 0;

    gte_ldrotmatrix(first_matrix);

    gte_ldrtir12_matrix_column(&scale_matrix.m[0][0]);
    gte_stir123_column(first_matrix);

    gte_ldrtir12_matrix_column(&scale_matrix.m[0][1]);
    second_matrix = (GteMatrix *)&g_CurrentEntity->render_object.model_matrix;
    gte_stir123_column_at(&second_matrix->m[0][1]);

    gte_ldrtir12_matrix_column(&scale_matrix.m[0][2]);
    third_matrix = (GteMatrix *)&g_CurrentEntity->render_object.model_matrix;
    gte_stir123_column_at(&third_matrix->m[0][2]);

    translation_matrix = (GteMatrix *)&g_CurrentEntity->render_object.model_matrix;
    gte_ldtransmatrix(translation_matrix);
    gte_ldv0_word3_at(scale_matrix.t);
    gte_cop2_hazard_slot();
    gte_cop2_hazard_slot();
    gte_rtv0tr_sf0();
    gte_swc2_9_0(translation_matrix->t);
    gte_swc2_10_4(translation_matrix->t);
    gte_swc2_11_8(translation_matrix->t);

    color_actor = g_CurrentEntity;
    color_actor->render_object.shade = *args[0];
    g_CurrentEntity->render_object.lightNegativeY = *args[1];
    g_CurrentEntity->render_object.lightPositiveY = *args[2];

    Render_InitRoomPrimState(&g_CurrentEntity->render_object);
    render_actor = g_CurrentEntity;
    Anim_BuildRotationMatrices(&render_actor->render_object,
                               render_actor->action_data, 0, 1);
    Render_TransformVertices(&g_CurrentEntity->render_object);
    Render_TransformSkinnedVertices(&g_CurrentEntity->render_object, D_800B89F8);
    Render_DrawObject(&g_CurrentEntity->render_object, D_800BEA40.words);
    Render_UpdateClutTable(&g_CurrentEntity->render_object, 1,
                           (s16)D_8009CDDC);

    flags = (flag_actor = g_CurrentEntity)->flags;
    if ((flags & 0x10000000) != 0) {
        first_draw_slot = D_8009CDDC;
        first_draw_slot ^= 1;
        D_8009CDDC = first_draw_slot;
        Render_DrawObject(&flag_actor->render_object, D_800BEA40.words);
        Render_UpdateClutTable(&g_CurrentEntity->render_object, 1,
                               (s16)D_8009CDDC);
        second_draw_slot = D_8009CDDC;
        second_draw_slot ^= 1;
        D_8009CDDC = second_draw_slot;
    } else {
        if ((flags & 0x08000000) == 0) {
        result = 0;
        script_ptr = D_8009CE00;
        flag_actor->flags = flags | 0x08000000;
        task_state = D_8009D300;
        D_8009CE00 = script_ptr - 5;
        task_state->active = 1;
        return result;
        }
        flag_actor->flags = flags & 0xF7FFFFFF;
    }
success:
    return 1;
}

int g_SceneDataTable0;

int Boot_CheckModeSwitch(void) {
    int mode = g_RenderStateFlags & 7;

    do {
        if (mode != 4) {
            if (mode != 0) {
                break;
            }
        }
        return 1;
    } while (0);
    {
        int cursor = g_SceneDataTable0;
        TaskNode *node = g_TaskNodePool;
        cursor -= 8;
        g_SceneDataTable0 = cursor;
        node->active = 1;
        return 0;
    }
}

#define NULL ((void *)0)


/* Script op: angle from the current entity to the actor named by
 * (args[0], args[1]), relative to the current entity's yaw, into args[2]. */
s32 Task_GetAngleToEntity(s32 *args[]) {
    s32 key;
    s32 key2;
    FieldActor *node;
    s32 angle;
    s32 dx;
    s32 dz;

    key = *args[0];
    if (key == 0) {
        FieldActor *tmp;

        tmp = g_PlayerEntity;
        if (tmp == NULL) {
            *args[2] = -1;
            return 1;
        }
        node = tmp;
    } else {
        key2 = key;
        node = g_FieldActorListHead;
        if (node == NULL) {
            *args[2] = -1;
            return 1;
        }
        do {
            if ((node->type_id != key2) ||
                (node->sub_id != *args[1]) ||
                (node->flags & 0x10)) {
                node = node->next;
            } else {
                break;
            }
        } while (node != NULL);
        if (node == NULL) {
            *args[2] = -1;
            return 1;
        }
    }

    {
        FieldActor *state = g_CurrentEntity;
        dx = state->pos_x - node->pos_x;
        dz = (state->pos_z - node->pos_z) >> 16;
    }
    angle = 0x1400 - ratan2(dz, dx >> 16);
    if (angle >= 0x1001) {
        angle -= 0x1000;
    }
    angle -= (s16)g_CurrentEntity->rot_y;
    if (angle < 0) {
        angle += 0x1000;
    }
    *args[2] = angle;
    return 1;
}
