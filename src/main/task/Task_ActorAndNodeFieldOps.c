/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/task_node.h"
#include "pe1/field_sfx.h"
#include "pe1/task_state.h"
#include "pe1/textbox.h"

/* COMMON supplies small-data metadata to MASPSX. */
int g_SceneDataTable0;
TaskNode *g_TaskNodePool;

int Entity_YieldWhileMoving(void) {
    int ret;

    ret = 0;
    if ((g_CurrentEntity->flags & 2) == 0) {
        return 1;
    }

    g_SceneDataTable0 -= 8;
    g_TaskNodePool->active = 1;
    return ret;
}
int Entity_SelectFieldSetter(int **arg0) {
    int value;
    int selector;

    value = *arg0[1];
    if (value < 0) {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity->script_cursor_1a0 = 0;
            break;
        case 2:
            g_CurrentEntity->script_cursor_19c = 0;
            break;
        case 3:
            g_TaskNodePool->next_value = 0;
            break;
        }
    } else {
        selector = *arg0[0];
        switch (selector) {
        case 1:
            g_CurrentEntity->script_cursor_1a0 = (unsigned int *)(g_CurrentEntity->script_base + (value << 1));
            break;
        case 2:
            g_CurrentEntity->script_cursor_19c = (unsigned int *)(g_CurrentEntity->script_base + (value << 1));
            break;
        case 3:
            g_TaskNodePool->next_value = (int)(g_CurrentEntity->script_base + (value << 1));
            break;
        }
    }

    return 1;
}

int Entity_SetCurrentFlags(int **args)
{
    g_CurrentEntity->flags |= (*args)[0];
    return 1;
}

int Entity_GetCurrentNodeId(int **args)
{
    *args[0] = g_TaskNodePool->seq;
    return 1;
}

/* Script opcodes: random value in a range and a field sound effect on the
 * current actor. Contiguous default-profile handlers. */

int Task_GpuPackPrimColor(int start, int end);

/* Equal bounds return one raw RNG word. Otherwise the result is in [start, end). */
int Task_Random(int **arg0) {
    int start = *arg0[1];
    int end = *arg0[2];

    if (start == end) {
        *arg0[0] = Task_GpuFlushPrimQueue();
    } else {
        *arg0[0] = Task_GpuPackPrimColor(start, end);
    }

    return 1;
}

int Task_PlayFieldSfx(FieldSfxScriptArgs *arg0) {
    register int a;
    int b;
    int c;
    int d;
    void *ptr0;
    void *ptr1;
    void *ptr2;

    ptr2 = (void *)arg0->taskArgument;
    ptr1 = (void *)arg0->subId;
    a = *(unsigned char *)ptr2;
    b = *(unsigned char *)ptr1;
    ptr0 = (void *)arg0->typeId;
    c = *(unsigned short *)ptr0;
    d = g_CurrentEntity->field_sfx_id;
    Task_QueueFieldSfx(a, b, c, d, 0);
    return 1;
}

int Tbl_LookupEntry(short arg0);

int Task_GetNodeTarget14(int **arg0) {
    *arg0[0] = g_TaskNodePool->target14;
    return 1;
}

int Task_YieldWhileTableEntrySet(short **arg0) {
    if ((Tbl_LookupEntry(*arg0[0]) << 24) != 0) {
        g_SceneDataTable0 -= 0xC;
        g_TaskNodePool->active = 1;
        return 0;
    }
    return 1;
}

int Task_ClearTableEntry(short **arg0) {
    Tbl_ClearEntry(*arg0[0]);
    return 1;
}

int Task_GetNodeTargets18_1C(int **arg0) {
    *arg0[0] = g_TaskNodePool->target18.coordinate;
    *arg0[1] = g_TaskNodePool->target1c;
    return 1;
}

int Entity_FindNodeById(unsigned short **arg0) {
    TaskNode *node;
    TaskNode *cur;
    unsigned short id;
    unsigned int bucket;
    unsigned int search_id;
    FieldActor *base;

    id = *arg0[0];
    node = g_TaskNodePool;
    if (node->seq == id) {
        node->flags |= 0x10;
        return 0;
    }

    base = g_CurrentEntity;
    search_id = id;
    bucket = 0;
    do {
        cur = base->task_node_lists[bucket & 0xFF];
        while (cur != 0) {
            if (cur->seq == search_id) {
                cur->flags |= 0x10;
                return 1;
            }
            cur = cur->next;
        }
        bucket++;
    } while ((bucket & 0xFF) < 3);

    return 1;
}

int Task_GetCurrentEntityFieldD(int **arg0) {
    *arg0[0] = g_CurrentEntity->sub_id;
    return 1;
}

int Task_SetCurrentEntityFieldD(int **arg0) {
    g_CurrentEntity->sub_id = *arg0[0];
    return 1;
}

int Task_GetCurrentEntityField24(int **arg0) {
    *arg0[0] = g_CurrentEntity->field_sfx_id;
    return 1;
}

int Entity_MarkNodeFree(void) {
    u8 i;
    TaskNode *skip;
    FieldActor *entity;
    TaskNode *node;

    i = 0;
    entity = g_CurrentEntity;
    skip = g_TaskNodePool;
    do {
        node = (TaskNode *)entity->task_node_lists[i];
        while (node != 0) {
            if (node != skip) {
                node->flags |= 0x10;
            }
            node = node->next;
        }
        i++;
    } while (i < 3);

    return 1;
}
