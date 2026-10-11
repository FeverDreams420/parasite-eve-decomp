/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/task_node.h"
#include "pe1/menu_inventory.h"
#include "pe1/field_actor.h"
#include "pe1/inventory.h"
#include "pe1/scene_transition.h"
#include "pe1/entity_frame_update.h"

extern unsigned char g_ScreenTransitionState[];
extern int g_SceneDataTable0;

int Entity_YieldOnLoading(void) {
    if ((g_ScreenTransitionState[0] & 3) >= 2) {
        g_SceneDataTable0 -= 8;
        g_TaskNodePool->active = 1;
        return 0;
    }

    return 1;
}

extern FieldActor *g_CurrentEntity[];

int Inv_CountByValue(int arg0);

int Task_ScaleAnimValue(int **arg0) {
    int value = *arg0[0];
    int scale = (s16)g_CurrentEntity[0]->render_object.hit_cylinder.radius * 2;
    RenderObjectHeader *dst = g_CurrentEntity[0]->render_object.header;

    dst->shadow_radius = (scale * value) >> 16;
    return 1;
}

int Task_ClampMenuRange(int **arg0) {
    Menu_ClampRange(*arg0[0]);
    return 1;
}

int Task_GiveItem(int **arg0) {
    *arg0[1] = Inv_AddItem(*arg0[0]);
    return 1;
}

int Task_CountItemsByValue(int **arg0) {
    *arg0[1] = Inv_CountByValue(*arg0[0]);
    return 1;
}


void Menu_OpenEquipSlotSwap(int arg0);

int Entity_InitWithTimeout(int **arg0) {
    TaskNode *state = g_TaskNodePool;
    unsigned short flags = state->flags;

    if ((flags & 0x20) == 0) {
        state->flags = flags | 0x20;
        Render_BeginSceneLoad();
        g_SceneDataTable0 -= 0x10;
        g_TaskNodePool->active = 1;
        return 0;
    }

    if ((unsigned short)(D_8009D2A4[0] - 3) < 0x180) {
        *arg0[1] = (short)D_8009D2A4[0] - 3;
        g_TaskNodePool->flags &= 0xFFDF;
        return 1;
    }

    if ((short)D_8009D2A4[0] == -1) {
        *arg0[1] = -1;
        g_TaskNodePool->flags &= 0xFFDF;
        return 1;
    }

    Menu_OpenEquipSlotSwap(*arg0[0]);
    g_SceneDataTable0 -= 0x10;
    g_TaskNodePool->active = 1;
    return 0;
}
