#include "common.h"
#include "pe1/field_actor.h"

int Task_RotateEntityAngle(int **arg0) {
    int *src;
    FieldActor *actor;
    int value;
    int delta;
    char frame[8];
    src = arg0[0];
    actor = g_CurrentEntity;
    delta = src[0];
    value = (u16)actor->rot_y + delta;
    actor->rot_y = value;
    if ((short)value >= 0x1001) {
        int adjusted;
        adjusted = value - 0x1000;
        actor->rot_y = adjusted;
    }
    actor = g_CurrentEntity;
    {
        int signed_value;
        signed_value = actor->rot_y;
        value = signed_value;
        if (signed_value < 0) {
            signed_value = 0x1000;
            signed_value = value + signed_value;
            actor->rot_y = signed_value;
        }
    }
    return 1;
}
