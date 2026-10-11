#include "scene_e08.h"

void func_80194988(void *arg0, void *arg1, Ovl178MotionState *state) {
    Ovl178MotionState *state_reg;
    Ovl178Position *position;
    int z;

    state_reg = state;
    position = func_800C2B50();

    state_reg->x = position->x - 0xC8;
    state_reg->y = position->y;
    z = position->z;
    state_reg->rotation.x = 0;
    state_reg->rotation.y = 0;
    state_reg->rotation.z = 0;
    state_reg->scale = 0xC04;
    state_reg->frame = 0;
    state_reg->shrink = 0;
    state_reg->depth = 0;
    state_reg->z = z;
}
