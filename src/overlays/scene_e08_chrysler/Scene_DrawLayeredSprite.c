#include "scene_e08.h"
#include "pe1/scene_e08_layered_sprite.h"
#include "pe1/room_floor.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_sprite_state.h"

#include "Scene_DrawLayeredSprite_externs.h"

void func_800C2FF0(int, int);
void func_800C3098(int);

void func_80194498(void *unused, u16 *phase, SceneLayeredSpriteState *state) {
    RoomSpriteMatrix matrix;
    RoomFxVec4 firstScale, layerScale;
    char *owner;
    s16 *alpha;
    unsigned i;
    s16 *alphaBase;
    u8 *red;

    owner = (char *)func_800C2B50();
    func_800C2EAC(((Ovl178Position *)owner)->spriteMode);
    func_800C2FF0(0x20, 0x20);
    func_800C3098(0x10);
    func_800C3238(2);

    RotMatrix(&state->initialSeed.vector, &matrix);
    firstScale = D_8018F050;
    ScaleMatrix(&matrix, &firstScale);
    if (phase[1] & 1) {
        D_80199560 = 0xFF;
        D_80199561 = 0xA0;
    } else {
        D_80199560 = 0xA0;
        D_80199561 = 0xFF;
    }
    D_80199562 = 0x60;
    asm("" : : "m"(D_80199562));

    alphaBase = &D_8019956A;
    alpha = alphaBase;
    matrix.t[0] = state->center[0].parts.integer;
    red = (u8 *)((char *)alpha - 10);
    matrix.t[1] = state->center[1].parts.integer;
    matrix.t[2] = state->center[2].parts.integer;
    *alpha = 0x80;
    func_800C42A4((FieldGlowSprite *)((char *)alphaBase - 10), &matrix, 0);

    matrix.t[0] = state->center[0].parts.integer;
    matrix.t[1] = g_RoomFloorY->y;
    matrix.t[2] = state->center[2].parts.integer;
    *alpha = 0x20;
    func_800C42A4((FieldGlowSprite *)((char *)alpha - 10), &matrix, 0);

    *red = 0x40;
    D_80199561 = 0x20;
    D_80199562 = 0;
    for (i = 0; i < 5; ++i) {
        RotMatrix(&state->layerSeeds[i].vector, &matrix);
        layerScale = D_8018F050;
        ScaleMatrix(&matrix, &layerScale);
        owner = (char *)alpha;
        matrix.t[0] = state->layers[i].x;
        matrix.t[1] = state->layers[i].y;
        matrix.t[2] = state->layers[i].z;
        *alpha = (5 - i) * 31;
        func_800C42A4((FieldGlowSprite *)(owner - 10), &matrix, 0);
    }
}
