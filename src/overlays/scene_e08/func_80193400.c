#include "pe1/scene_glow_model.h"

void func_80193400(void *arg0, RoomFxControl *signal, SceneGlowModelState *work) {
    work->seed.vector.y += 0xC8;
    if (signal->frame < 0x10) {
        u16 depth = work->depth;
        u16 height = work->height;

        depth += 8;
        height += 0x100;
        work->depth = depth;
        work->height = height;
    } else {
        u16 depth = work->depth;

        depth -= 4;
        work->depth = depth;
        work->scaleX += 0x50;
        work->scaleZ += 0x50;
    }
    if ((s16)work->depth <= 0) {
        signal->state = 2;
    }
}
