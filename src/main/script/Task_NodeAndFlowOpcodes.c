/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/task_node.h"
#include "pe1/field_actor.h"
#include "pe1/field_collision.h"
#include "pe1/textbox.h"


int Entity_SetCurrentNodeActive(int **arg0) {
    g_TaskNodePool->active = *(unsigned short *)arg0[0];
    return 0;
}

int Entity_MarkCurrentNodeFree(void) {
    g_TaskNodePool->flags |= 0x10;
    return 0;
}

/* COMMON supplies small-data metadata to MASPSX. */
int g_SceneDataTable0;
TaskNode *g_TaskNodePool;

int Task_JumpIfZero(int **arg0) {
    if (*arg0[0] == 0) {
        g_SceneDataTable0 = (int)(g_CurrentEntity->script_base + (*arg0[1] * 2));
    }
    return 1;
}


int Task_SpawnEntityAt(int **arg0) {
    char local[2];
    FieldActor *entity;

    local[0] = arg0[0][0];
    local[1] = arg0[1][0];
    entity = Scene_LoadMap((u8 *)local, g_CurrentEntity, 1);
    entity->pos_x = arg0[2][0];
    entity->pos_y = arg0[3][0];
    entity->pos_z = arg0[4][0];
    Entity_FindFloor((struct BattleEntity *)entity);
    return 1;
}
int Task_AssignValue(int **arg0) {
    *arg0[0] = *arg0[1];
    return 1;
}

int Task_StopEntityAnim(s16 **arg0) {
    s16 value[5];

    value[0] = -1;
    Render_SetupColorTable(*arg0[0], 0, value);
    return 1;
}
