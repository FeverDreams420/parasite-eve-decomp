#include "scene_e08.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_sprite_state.h"
#include "pe1/psyq_libc.h"

typedef struct SceneShiftedPosition {
    char reserved[0x14];
    s32 x, y, z;
} SceneShiftedPosition;

typedef struct SceneFadeOwner {
    u8 reserved[4];
    SceneShiftedPosition position;
    u8 drawMode;
} SceneFadeOwner;

typedef union SceneFadeSeed {
    RoomFxSeed8 raw;
    struct {
        u8 initial[4];
        s16 angle;
        u8 tail[2];
    } fields;
} SceneFadeSeed;

#include "Scene_DrawFadeLayers_externs.h"
void func_800C2FF0(int, int);
void func_800C3098(int);

void func_8018F640(void *unused, s16 *args, Ovl178FadeState *state) {
    RoomSpriteMatrix matrix;
    SceneFadeSeed seed;
    RoomFxVec4 scale, sourceScale;
    SceneFadeOwner *owner;
    SceneShiftedPosition *ownerCursor;
    unsigned i;

    owner = (SceneFadeOwner *)func_800C2B50();
    seed.raw = D_8018EFF4;

    ownerCursor = &owner->position;
    func_800C2EAC(owner->drawMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);
    func_800C3134(D_801986F8, args[1], (u8 *)&D_80199670);
    for (i = 0; i < 4; ++i) {
        seed.fields.angle = state->glowAngle[i];
        RotMatrix(&seed.raw.vector, &matrix);
        memset(&sourceScale, 0, 0x10);
        sourceScale.x = state->glowScale[i];
        sourceScale.y = state->glowScale[i];
        sourceScale.z = state->glowScale[i];
        scale = sourceScale;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = ownerCursor->x;
        matrix.t[1] = ownerCursor->y;
        matrix.t[2] = ownerCursor->z;
        D_80199670.depth = state->glowDepth;
        func_800C42A4(&D_80199670, &matrix, 1);
    }
    for (i = 0; i < 2; ++i) {
        matrix.m[2][2] = 0x1000;
        matrix.m[1][1] = 0x1000;
        matrix.m[0][0] = 0x1000;
        matrix.t[2] = 0;
        matrix.t[1] = 0;
        matrix.t[0] = 0;
        matrix.m[2][1] = 0;
        matrix.m[2][0] = 0;
        matrix.m[1][2] = 0;
        matrix.m[1][0] = 0;
        matrix.m[0][2] = 0;
        matrix.m[0][1] = 0;
        memset(&sourceScale, 0, 0x10);
        sourceScale.x = state->ringScale[i];
        sourceScale.y = state->ringScale[i];
        sourceScale.z = state->ringScale[i];
        scale = sourceScale;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = ownerCursor->x;
        matrix.t[1] = ownerCursor->y;
        matrix.t[2] = ownerCursor->z;
        func_800C3134(D_80198708, args[1], &state->emitters[i].color1[0]);
        func_800C4FC4(&state->emitters[i], &matrix, 1);
    }
}
