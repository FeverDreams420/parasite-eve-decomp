#include "pe1/gte_types.h"
#include "pe1/room_m023_beacon.h"
#include "pe1/scene_grab_head.h"
#include "pe1/field_actor.h"
#include "room_m273.h"
extern char D_8019A8B8;
extern int D_8019A8CC;
extern int D_8019A8D0;
extern int D_8019A8D4;
extern char D_8019A8D8;


int func_8019250C(RoomSelectionState *obj) {
    int ret;

    obj->callback = (void (*)(RoomSelectionState *))SceneEffect_GrabAwaitStance;
    obj->started = 0;
    obj->placementChecked = 0;
    obj->verticalTicks = 0;
    obj->activated = 0;
    obj->flag3 = 0xFF;
    RotMatrixYXZ(&D_8019A8D8, (void *)0x1F800028);

    ret = MulMatrix0(&D_8019A8B8, (void *)0x1F800028, (GteMatrix *)&obj->matrix);
    /* matrix.translation, stored as plain words to keep retail order. */
    *(int *)((char *)obj + 0x30) = D_8019A8CC;
    *(int *)((char *)obj + 0x34) = D_8019A8D0;
    *(int *)((char *)obj + 0x38) = D_8019A8D4;
    /* Keep the call result live so GCC allocates the final data load to v1. */
    return ret - ret;
}
