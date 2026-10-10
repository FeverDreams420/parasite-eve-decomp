#include "pe1/geom_state.h"
#include "pe1/field_actor.h"

extern int g_GameStateFlags;
extern int g_SceneDispatchToken;
extern int g_PlayTimeFrameCounter;

int Str_EncodeBase32(char *out, unsigned int value);
void Sys_Shutdown(int *arg0);

int Task_EncodeBarcode(int **arg0) {
    int tmp[2];
    int value;
    int *counter;

    Str_EncodeBase32(&tmp[0], *arg0[0]);
    value = *arg0[0];
    g_GameStateFlags |= 0x2000;
    g_SceneDispatchToken = value;

    if (*arg0[0] == 0xA9400048) {
        counter = &g_PlayTimeFrameCounter;
        if (*counter == 0x7D0) {
            *counter = 0;
        } else {
            Sys_Shutdown(counter);
        }
    }
    return 0;
}

void Render_SetScrollMode(int arg0, int arg1, unsigned int arg2, int arg3);

int Task_SetFogParams(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 0);
    return 1;
}

int Task_SetFogParamsAlt(short **arg0) {
    Render_SetScrollMode(*arg0[0], *arg0[1], *(unsigned short *)arg0[2], 8);
    return 1;
}

int Task_IsBattlePhase4(int **arg0) {
    *arg0[0] = ((g_RenderStateFlags & 7) == 4);
    return 1;
}

extern FieldActor *g_PlayerEntity[];

int Task_ResetPlayerCameraPos(void) {
    Render_UpdateScrollPosition(&g_PlayerEntity[0]->pos_x, -1, -1);
    return 1;
}

int Task_SetBattleStateMode(void) {
    int *ptr = &g_RenderStateFlags;

    *ptr = (*ptr & -8) | 0x80;
    return 1;
}

extern FieldActor *g_CurrentEntity[];

int Task_SetEntityField224(short **arg0) {
    FieldActor *entity;
    int value;

    entity = g_CurrentEntity[0];
    value = (*arg0)[1];
    entity->render_object.hit_cylinder.radius = value;
    return 1;
}
