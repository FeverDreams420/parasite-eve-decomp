#include "pe1/field_anim_callback_list.h"
#include "pe1/field_anim_pool.h"
#include "pe1/field_actor.h"
#include "scene_e22_shared.h"
typedef struct { char reserved[0x14]; short complete, signaled; } WaitState;

int func_80198F54(int mode) {
    switch (mode) {
    case 0:
        ANIM_TASK_AS(WaitState)->complete = 0;
        ANIM_TASK_AS(WaitState)->signaled = 0;
        RoomMain_ActorPtr->state->flags &= ~0x10000000u;
        return 0;
    case 1:
        if (RoomMain_ActorPtr->state->flags & 0x10000000u) {
            ANIM_TASK_AS(WaitState)->complete = 1;
            ANIM_TASK_AS(WaitState)->signaled = 1;
            return 1;
        }
        if (D_800E27EC >= 100) {
            ANIM_TASK_AS(WaitState)->complete = 1;
            ANIM_TASK_AS(WaitState)->signaled = 0;
            return 1;
        }
        break;
    case 2:
        break;
    }
    return 0;
}
