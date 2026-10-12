/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/memcard.h"
#include "pe1/task_node.h"
#include "pe1/game_state_types.h"
#include "pe1/scene_room_load.h"
#include "../../../tools/m2c/m2c_macros.h"
#include "pe1/field_actor.h"
#include "pe1/global_slot.h"
#include "pe1/gte_types.h"
#include "pe1/game_state.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_entity_anim.h"

#define NULL ((void *)0)
void Battle_StartEncounter(int mode);
M2C_UNK func_80042F20();
int CD_StepReadState(int active);
s32 g_SceneDataTable0;

PE1_STATIC_ASSERT(PE1_OFFSETOF(Pe1GameState, memcard_init_phase) == 0xF4,
                  memcard_init_phase_offset);

s32 Boot_MemCardStepInit(u8 **arg0) {
    Pe1GameState *base;

    base = &g_GameState;
    loop_1:
    switch (base->memcard_init_phase) {
    case 0x0:
        if (!(base->story_day_flags & 3)) {
            base->memcard_init_phase = 0x37U;
        }
        base->flags = base->flags | 0x800000;
        break;
    case 0x37:
        if (!(g_GameState.flags & 0x400000)) {
            MemCard_InitSlotState();
        }
        base->memcard_init_phase = 0x38U;
        goto loop_1;
    case 0x38:
        if (CD_StepReadState(1) != 1) {
            if (!(g_GameState.flags & 0x400000)) {
                func_80042F20();
            }
            base->memcard_init_phase = 0x39U;
            goto loop_1;
        }
        break;
    case 0x39:
        if (Asset_LoadTimTextures(1) != 1) {
            base->memcard_init_phase = 0x3AU;
            goto loop_1;
        }
        break;
    case 0x3A:
        {
            s32 t1a0 = g_GameStateFlags | 2;
            g_GameStateFlags = t1a0;
        }
        Battle_StartEncounter(**arg0);
        base->memcard_init_phase = 0x3BU;
        break;
    case 0x3B:
        if (!(base->story_day_flags & 3)) {
            base->memcard_init_phase = 0U;
            base->flags = base->flags & 0xFF7FFFFF;
            __asm__ volatile("");
            return 1;
        }
        break;
    default:
        return 1;
    }
block_19:
    g_SceneDataTable0 -= 0xC;
    g_TaskNodePool->active = 1;
    return 0;
}


int Entity_GetPositionByType(int **args) {
    FieldActor *entity;
    FieldActor *player;
    int selector;
    int entity_id;

    selector = *args[1];
    if (selector == 0) {
        player = (FieldActor *)D_8009D254;
        if (player == 0) {
            *args[6] = -1;
            return 1;
        }
        entity = player;
    } else {
        entity = D_8009D20C;
        entity_id = selector;
        while (entity != 0) {
            if (entity->type_id == entity_id &&
                entity->sub_id == *args[2] &&
                (entity->flags & 0x10) == 0) {
                break;
            }
            entity = entity->next;
        }
        if (entity == 0) {
            *args[6] = -1;
            return 1;
        }
    }

    *args[6] = 1;
    switch (*args[0]) {
    case 0:
        *args[3] = entity->pos_x;
        *args[4] = entity->pos_y;
        *args[5] = entity->pos_z;
        break;
    case 1:
        *args[3] = entity->base_x;
        *args[4] = entity->base_y;
        *args[5] = entity->base_z;
        break;
    case 2:
        *args[3] = entity->motion_x;
        *args[4] = entity->motion_y;
        *args[5] = entity->motion_z;
        break;
    case 3:
        *args[3] = entity->accel_x;
        *args[4] = entity->accel_y;
        *args[5] = entity->accel_z;
        break;
    case 4:
        *args[3] = entity->gravity_x;
        *args[4] = entity->gravity_y;
        *args[5] = entity->gravity_z;
        break;
    case 5:
        *args[3] = entity->rot_x;
        *args[4] = entity->rot_y;
        *args[5] = entity->rot_z;
        break;
    case 6:
        *args[3] = entity->delta_x;
        *args[4] = entity->delta_y;
        *args[5] = entity->delta_z;
        break;
    }
    return 1;
}

PE1_STATIC_ASSERT(sizeof(TaskNode) == 0x2C, camera_task_node_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, flags) == 0x08,
                  camera_task_node_flags_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, active) == 0x10,
                  camera_task_node_active_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, target14) == 0x14,
                  camera_task_node_target14_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, target18) == 0x18,
                  camera_task_node_target18_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(TaskNode, target1c) == 0x1C,
                  camera_task_node_target1c_offset);

/* COMMON supplies small-data metadata to MASPSX. */
u32 *D_8009CE00;
TaskNode *D_8009D300;
TaskNode *g_TaskNodePool;

int Math_FixedMul(int a, int b);

int Camera_TrackRelativeOffset(int **args) {
    FieldActor *speed_entity;
    FieldActor *motion_entity;
    TaskNode *init_state;
    TaskNode *camera_state;
    int movement_speed;
    int turn_speed;
    int target_angle;
    int desired_angle;
    int current_angle;
    int angle_difference;
    int wrap_difference;
    int current_x;
    int current_z;
    int target_x;
    int target_z;
    int movement_z;
    int remaining_distance;
    int movement_distance;

    speed_entity = g_CurrentEntity;
    current_x = speed_entity->pos_x;
    current_z = speed_entity->pos_z;
    if (speed_entity != (FieldActor *)D_8009D254) {
        movement_speed = speed_entity->move_factor;
    } else {
        movement_speed = Math_FixedMul(0x50000, speed_entity->move_factor);
    }
    movement_speed = Math_FixedMul(movement_speed, g_CurrentEntity->move_speed << 4);

    init_state = D_8009D300;
    if ((init_state->flags & 0x20) == 0) {
        turn_speed = *args[2];
        target_x = current_x + *args[0];
        target_z = current_z + *args[1];
        init_state->flags |= 0x20;
        init_state->target20 = 0;
        init_state->target14 = turn_speed;
        init_state->target1c = target_z;
        init_state->target18.coordinate = target_x;
    } else {
        turn_speed = init_state->target14;
        target_x = init_state->target18.coordinate;
        target_z = init_state->target1c;
    }

    target_angle = (u16)(0x1400 - ratan2(current_z - target_z,
                                             current_x - target_x));
    target_angle &= 0xFFF;
    desired_angle = target_angle;
    if (turn_speed != 0) {
        current_angle = g_CurrentEntity->rot_y;
        if (current_angle < desired_angle) {
            angle_difference = desired_angle - current_angle;
            if (angle_difference < 0x800) {
                if (turn_speed >= angle_difference) {
                    target_angle = desired_angle;
                } else {
                    target_angle = current_angle + turn_speed;
                }
            } else if (turn_speed >= angle_difference) {
                target_angle = desired_angle;
            } else {
                target_angle = current_angle - turn_speed;
                if (target_angle < 0) {
                    wrap_difference = current_angle + 0x1000;
                    wrap_difference -= desired_angle;
                    if (wrap_difference < turn_speed) {
                        target_angle = desired_angle;
                    }
                }
            }
        } else {
            angle_difference = current_angle - desired_angle;
            if (angle_difference < 0x800) {
                if (turn_speed >= angle_difference) {
                    target_angle = desired_angle;
                } else {
                    target_angle = current_angle - turn_speed;
                }
            } else if (turn_speed >= angle_difference) {
                target_angle = desired_angle;
            } else {
                target_angle = current_angle + turn_speed;
                if (target_angle >= 0x1001) {
                    wrap_difference = desired_angle + 0x1000;
                    wrap_difference -= current_angle;
                    if (wrap_difference < turn_speed) {
                        target_angle = desired_angle;
                    }
                }
            }
        }
        target_angle &= 0xFFF;
        g_CurrentEntity->rot_y = target_angle;
    }

    g_CurrentEntity->motion_x = Math_FixedMul(-movement_speed, rsin(target_angle) << 4);
    movement_z = Math_FixedMul(-movement_speed, rcos(target_angle) << 4);
    current_x = (target_x - current_x) >> 16;
    current_z = (target_z - current_z) >> 16;
    remaining_distance = current_x * current_x + current_z * current_z;
    motion_entity = g_CurrentEntity;
    current_x = ((s16 *)&motion_entity->motion_x)[1];
    current_z = movement_z >> 16;
    movement_distance = current_x * current_x + current_z * current_z;
    motion_entity->motion_z = movement_z;
    if (movement_distance >= remaining_distance) {
        camera_state = D_8009D300;
        motion_entity->pos_x = target_x;
        motion_entity->pos_z = target_z;
        motion_entity->motion_x = 0;
        motion_entity->motion_z = 0;
        camera_state->flags &= 0xFFDF;
        return 1;
    } else {
        D_8009CE00 -= 5;
        D_8009D300->active = 1;
        return 0;
    }
}

/* Script op: turn the current actor's yaw to the angle in arg0[0] by the
 * step in arg0[1], picking the direction of the shorter arc on the first
 * call; yields until the next step would reach or pass the target. The
 * angles are shorts in retail: their extensions survive at every compare. */
s32 Camera_StepAngleFade(s32 **arg0) {
    short current;
    short target;
    short step;
    TaskNode *node;
    int flags;

    current = g_CurrentEntity->rot_y & 0xFFF;
    node = g_TaskNodePool;
    flags = node->flags;
    if (!(flags & 0x20)) {
        short distance;

        target = *arg0[0] & 0xFFF;
        step = *(u16 *)arg0[1];
        if (current == target) {
            return 1;
        }
        node->flags = flags | 0x20;
        if (target - current >= 0) {
            distance = target - current;
        } else {
            distance = current - target;
        }
        if (target < current) {
            if (distance >= 0x801) {
                g_TaskNodePool->target14 = step;
            } else {
                g_TaskNodePool->target14 = -step;
            }
        } else if (distance < 0x801) {
            g_TaskNodePool->target14 = step;
        } else {
            g_TaskNodePool->target14 = -step;
        }
        g_TaskNodePool->target18.coordinate = target;
    }
    step = g_TaskNodePool->target14;
    target = g_TaskNodePool->target18.coordinate;
    {
        int angle = current;
        int goal = target;

        if ((angle >= goal && goal >= angle + step) ||
            (goal >= angle && angle + step >= goal)) {
            g_CurrentEntity->rot_y = target;
            g_TaskNodePool->flags &= 0xFFDF;
            return 1;
        }
    }
    if ((step > 0 && current + step >= 0x1001 && ((current + step) & 0xFFF) >= target) ||
        (step < 0 && current + step < 0 && ((current + step) & 0xFFF) <= target)) {
        g_CurrentEntity->rot_y = target;
        g_TaskNodePool->flags &= 0xFFDF;
        return 1;
    }
    g_SceneDataTable0 -= 0x10;
    g_CurrentEntity->rot_y += step;
    g_TaskNodePool->active = 1;
    return 0;
}
