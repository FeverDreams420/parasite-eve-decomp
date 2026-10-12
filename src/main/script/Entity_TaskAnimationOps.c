#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/task_node.h"
#include "pe1/battle_entity_anim.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

/* COMMON supplies small-data metadata to MASPSX. */
int g_SceneDataTable0;
TaskNode *g_TaskNodePool;

int Entity_MarkAnimPending(void) {
    unsigned int i;
    FieldActor *entity;
    TaskNode *node;
    TaskNode *skip;

    i = 0;
    skip = g_TaskNodePool;
    entity = g_CurrentEntity;
    do {
        node = entity->task_node_lists[i];
        if (node != 0) {
            do {
                if (node != skip) {
                    node->flags |= 0x40;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
    } while (i < 3U);
    return 1;
}


int Entity_AdvancePendingAnim(void) {
    unsigned int i;
    FieldActor *entity;
    TaskNode *node;
    int active;

    i = 0;
    active = 1;
    entity = g_CurrentEntity;
    do {
        node = entity->task_node_lists[i];
        if (node != 0) {
            do {
                if (node->flags & 0x40) {
                    node->flags &= ~0x40;
                    if (node->next_value != 0) {
                        node->current.value = node->next_value;
                        node->active = active;
                        node->flags &= ~0x20;
                    }
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
    } while (i < 3U);
    return 1;
}


int Entity_MarkNodeAnimPendingById(int **arg0) {
    unsigned int i;
    FieldActor *entity;
    TaskNode *node;

    i = 0;
    entity = g_CurrentEntity;
    do {
        node = entity->task_node_lists[i];
        if (node != 0) {
            do {
                if (node->seq == **arg0) {
                    node->flags |= 0x40;
                    return 1;
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
    } while (i < 3U);

    return 1;
}


int Entity_AdvanceNodeById(int **arg0) {
    unsigned int i;
    FieldActor *entity;
    TaskNode *node;
    int next_value;
    int active_value;

    i = 0;
    active_value = 1;
    entity = g_CurrentEntity;
    do {
        node = entity->task_node_lists[i];
        if (node != 0) {
            do {
                if (node->seq == **arg0) {
                    node->flags &= ~0x40;
                    next_value = node->next_value;
                    if (next_value != 0) {
                        node->current.value = next_value;
                        node->active = active_value;
                        node->flags &= ~0x20;
                    }
                }
                node = node->next;
            } while (node != 0);
        }
        i++;
    } while (i < 3U);

    return 1;
}

int Task_RunEntityCommand(int **arg0) {
    int result;

    result = (signed char)Entity_TriggerAnimEvent((BattleEntity *)g_CurrentEntity, *(unsigned char *)arg0[0]);
    *arg0[1] = result;
    if (*arg0[1] != 0) {
        return 1;
    }

    g_TaskNodePool->active = 1;
    g_SceneDataTable0 -= 0x10;
    return 0;
}
