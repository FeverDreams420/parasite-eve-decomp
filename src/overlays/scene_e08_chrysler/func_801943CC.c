#include "scene_e08.h"
#include "pe1/scene_e08_layered_sprite.h"

void func_801943CC(void *arg0, void *arg1, SceneLayeredSpriteState *out)
{
    Ovl178Position *position = func_800C2B50();

    out->center[0].fixed = position->x << 16;
    out->center[1].fixed = position->y << 16;
    out->center[2].fixed = position->z << 16;
    out->initialSeed.vector.x = 0;
    out->initialSeed.vector.y = 0;
    out->initialSeed.vector.z = 0;
    out->phase = *FieldEng_RegPtr(2);
    out->direction = *FieldEng_RegPtr(3);
    out->drift = *FieldEng_VarPtr(1);
    if (out->direction == 0)
        out->velocity[2] = 0x124F80;
    else
        out->velocity[2] = 0xFFEDB080;
    out->velocity[0] = 0x7A120;
    out->velocity[1] = 0xFFF551A0;
}
