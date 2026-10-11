#include "pe1/scene_ring_sprites.h"
#include "pe1/room_fx.h"

void func_80191348(s32 arg0, RoomFxControl *rec, SceneRingSpritesState *p) {
    SceneRingSpritesState *q;
    s16 zero;
    s32 step;

    q = p;
    step = rcos(rec->frame << 5) >> 6;
    p->size += step;
    p->ticks++;
    p->depth -= 4;
    zero = 0;
    if (p->depth < 0) {
        p->depth = zero;
    }
    if (q->depth == zero) {
        rec->state = 2;
    }
}
