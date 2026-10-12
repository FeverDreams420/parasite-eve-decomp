/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "pe1/field_actor.h"
#include "pe1/task_node.h"

/* COMMON supplies small-data metadata to MASPSX. */
int g_SceneDataTable0;
TaskNode *g_TaskNodePool;

int Task_WaitEntityFlags8(void) {
    TaskNode *state;
    unsigned short flags;

    state = g_TaskNodePool;
    flags = state->flags;

    if (flags & 0x20) {
        if (g_CurrentEntity->flags & 8) {
            state->flags = flags & 0xFFDF;
            return 1;
        }
    } else {
        state->flags = flags | 0x20;
    }

    g_SceneDataTable0 -= 8;
    g_TaskNodePool->active = 1;
    return 0;
}
