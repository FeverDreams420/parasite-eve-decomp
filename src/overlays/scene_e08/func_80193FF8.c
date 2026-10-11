#include "scene_e08.h"
#include "pe1/room_drop_field.h"

void func_80193FF8(s32 arg0, s32 arg1, SceneE08TrailGlow *p) {
    Ovl178Position *view;
    s32 z;

    view = func_800C2B50();
    func_800C2B10(1);
    p->acceleration = 0x14;
    p->unused88 = 0x80;
    p->speed = 0;
    p->rotation[0].x = 0;
    p->rotation[0].y = 0x400;
    p->rotation[0].z = 0;
    p->trail[0].x = view->x - 0x15E;
    p->trail[0].y = view->y + 0x78;
    z = view->z;
    p->scale[0].x = 0;
    p->scale[0].y = 0x800;
    p->scale[0].z = 0;
    p->trail[0].z = z;
}
