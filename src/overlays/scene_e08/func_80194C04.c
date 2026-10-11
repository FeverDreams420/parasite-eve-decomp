#include "scene_e08.h"

void func_80194C04(void *arg0, void *arg1, SceneE08ExpandingGlow *glow) {
    Ovl178Position *position = func_800C2B50();

    glow->x = position->x - 0x15E;
    glow->y = position->y;
    glow->z = position->z;
    glow->scale = 0;
    glow->growth = 0;
    glow->depth = 0xFF;
}
