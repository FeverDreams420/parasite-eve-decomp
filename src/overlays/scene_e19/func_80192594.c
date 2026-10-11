#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/room_m023_beacon.h"
#include "pe1/scene_grab_head.h"
#include "pe1/scene_grab_trigger.h"

extern char D_8019B31C[];
extern char D_8019B2FC[];
extern s32 D_8019B310;
extern s32 D_8019B314;
extern s32 D_8019B318;

s32 func_80192594(SceneGrabTrigger *obj) {
    obj->control.callback = SceneEffect_GrabAwaitStance;
    obj->control.started = 0;
    obj->control.grabbed = 0;
    obj->flag3 = 0xFF;
    RotMatrixYXZ(D_8019B31C, (void *)0x1F800028);
    MulMatrix0(D_8019B2FC, (void *)0x1F800028, (GteMatrix *)&obj->control.player_matrix);
    /* player_matrix.translation, stored as plain words to keep retail order. */
    *(s32 *)((char *)obj + 0x30) = D_8019B310;
    *(s32 *)((char *)obj + 0x34) = D_8019B314;
    *(s32 *)((char *)obj + 0x38) = D_8019B318;
    return 0;
}
