#include "scene_e08.h"
#include "pe1/floor_polygon.h"
#include "pe1/field_engine_calls.h"

void func_80194214(void *unused, RoomFxControl *phase, SceneE08TrailGlow *effect) {
    Ovl178Position *owner;
    s16 point[3];
    int i;
    int angle;
    u16 preserved;
    s32 x, z;

    owner = func_800C2B50();
    for (i = 2; i != 0; i--) {
        __builtin_memcpy(&effect->rotation[i], &effect->rotation[i - 1], 8);
        effect->trail[i] = effect->trail[i - 1];
        __builtin_memcpy(&effect->scale[i], &effect->scale[i - 1], 8);
    }

    if (phase->frame >= 0x33) {
        angle = effect->speed + effect->acceleration;
        preserved = effect->rotation[0].z;
        effect->speed = angle;
        effect->rotation[0].z = preserved;
        effect->trail[0].x = effect->trail[0].x - (s16)angle;
    }
    if ((u16)(phase->frame - 0x22) < 9) {
        effect->scale[0].x += 0x12C;
    }
    if (phase->frame >= 0x4C) {
        effect->scale[0].y += 0x96;
    }
    if (phase->frame == 0x78) {
        phase->status = 2;
    }

    x = effect->trail[0].x;
    point[0] = x;
    point[1] = effect->trail[0].y;
    z = effect->trail[0].z;
    point[2] = z;
    if (Geo_PointInPoly(x << 16, z << 16, D_8009D248, D_8009D1CC) == 0) {
        phase->status = 2;
    }
    if (func_800C6B90(point, 0x320) != 0) {
        owner->playerHit = 1;
    }
}
