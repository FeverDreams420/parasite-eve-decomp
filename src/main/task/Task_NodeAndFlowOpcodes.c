/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/task_node.h"
#include "pe1/field_actor.h"
#include "pe1/field_collision.h"
#include "pe1/textbox.h"

extern TaskNode *g_TaskNodePool;

int Entity_SetCurrentNodeActive(int **arg0) {
    g_TaskNodePool->active = *(unsigned short *)arg0[0];
    return 0;
}

int Entity_MarkCurrentNodeFree(void) {
    g_TaskNodePool->flags |= 0x10;
    return 0;
}

extern FieldActor *g_CurrentEntity[];
extern int g_SceneDataTable0;

int Task_JumpIfZero(int **arg0) {
    if (*arg0[0] == 0) {
        g_SceneDataTable0 = (int)(g_CurrentEntity[0]->script_base + (*arg0[1] * 2));
    }
    return 1;
}

FieldActor *Scene_LoadMap(char *arg0, FieldActor *arg1, int arg2);

int Task_SpawnEntityAt(int **arg0) {
    char local[2];
    FieldActor *entity;

    local[0] = arg0[0][0];
    local[1] = arg0[1][0];
    entity = Scene_LoadMap(local, g_CurrentEntity[0], 1);
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
