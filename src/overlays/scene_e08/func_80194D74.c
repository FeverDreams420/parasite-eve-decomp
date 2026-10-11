#include "scene_e08.h"

void func_80194D74(s32 arg0, RoomFxControl *rec, SceneE08ExpandingGlow *p) {
    s16 v;

    p->scale += p->growth;
    p->growth += 10;
    v = p->depth;
    if (v >= 9) {
        p->depth = v - 8;
    }
    if (rec->frame == 0x3C) {
        rec->status = 2;
    }
}
