#include "common.h"
#include "pe1/field_actor.h"
#include "pe1/entity_animation.h"

extern int g_FieldMoveLock;

int Task_ClearBit(int **arg0) {
    *arg0[0] &= ~(1 << *arg0[1]);
    return 1;
}

int Task_TestBit(int **arg0) {
    *arg0[2] = *arg0[0] & (1 << *arg0[1]);
    return 1;
}

int Task_SetBit(int **arg0) {
    *arg0[0] |= 1 << *arg0[1];
    return 1;
}

int Entity_ClearGlobalFlags(int **arg0) {
    g_FieldMoveLock &= ~*arg0[0];
    return 1;
}

int Entity_GetGlobalFlags(int **arg0) {
    *arg0[0] = g_FieldMoveLock;
    return 1;
}

int Entity_SetGlobalFlags(int **arg0) {
    g_FieldMoveLock |= *arg0[0];
    return 1;
}

extern FieldActor *g_CurrentEntity;

int Entity_SetCurrentActionMode(u16 **arg0) {
    FieldActor *entity;

    Entity_SetActionMode((BattleEntity *)g_CurrentEntity, *arg0[0]);
    entity = g_CurrentEntity;
    entity->flags &= ~0x100;
    return 1;
}

int Task_SetEntityAnimFrame(u16 **arg0) {
    FieldActor *entity = g_CurrentEntity;
    u16 value = *arg0[0];

    if (entity->action < value) {
        value = entity->action;
    }

    entity->anim_frame_target = value;
    entity->flags |= 0x200;
    return 1;
}
