#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
#include "pe1/global_slot.h"
#include "pe1/task_node.h"
#include "pe1/render_setup.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/game_state.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

/* COMMON supplies small-data metadata to MASPSX. */
u32 *D_8009CE00;
TaskNode *D_8009D300;

int Task_PlayEntityAnim(s16 **args) {
    TaskNode *task_state;
    FieldActor *actor;
    FieldActor *post_actor;
    FieldActor *player;
    u32 control_flags;
    u16 progress;

    task_state = D_8009D300;
    progress = task_state->flags;
    if (progress & 0x20) {
        if (!(g_CurrentEntity->render_object.flags_9C & 4)) {
            task_state->flags = progress & ~0x20;
            return 1;
        }
    } else {
        actor = g_CurrentEntity;
        actor->flags &= ~0x40;
        task_state->flags |= 0x20;
        Anim_SetInterpRate(&actor->render_object, *args[0]);
        post_actor = g_CurrentEntity;
        player = g_PlayerEntity;
        post_actor->render_object.flags_9C |= 4;

        if (post_actor == player) {
            control_flags = g_FieldMoveLock;
            control_flags &= ~2;
            g_FieldMoveLock = control_flags;
            Anim_SetInterpRate(&D_800B0CEC, *args[0]);
            D_800B0CEC.flags_9C |= 4;
        }
    }

    D_8009CE00 -= 3;
    D_8009D300->active = 1;
    return 0;
}
