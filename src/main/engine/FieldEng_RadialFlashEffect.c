#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/battle_runtime.h"
#include "pe1/psyq_gpu.h"
#include "pe1/random.h"
#include "pe1/field_anim.h"
#include "pe1/field_actor.h"

int func_800DBA9C(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor color;
    int scale;
    switch (mode) {
    case 0:
        state->phase = D_800E1D60;
        D_800E1D60 += 1365;
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        state->position.y -= 100;
        state->position.x += rcos(state->phase) * 300 / 4096;
        state->position.z += rsin(state->phase) * 300 / 4096;
        return 0;
    case 1:
        if (D_800E27EC >= 8) return 1;
        break;
    case 2:
        D_800F3368.depth = 60;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        scale = 2048 + (D_800E27EC << 9);
        func_800CF3AC(D_800E1D64, &color, D_800E27EC);
        func_800D004C(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
        func_800D0728(&position, 410, 500, 20, 0, 4096, 4096, 0, &color, 128, 3);
        func_800CF3AC(D_800E1D84, &color, D_800E27EC);
        func_800D004C(&position, 500, 100, 16, 0, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}

int func_800DBCD8(int mode, GteShortVector *state)
{
    RenderColor color;
    GteRotation rotation;
    int scale, palette;
    switch (mode) {
    case 1:
        state->x += (rand() & 7) - 3;
        state->z += (rand() & 7) - 3;
        state->y += rand() & 3;
        if (D_800E27EC >= 32) return 1;
        break;
    case 2:
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC * 12;
        rotation.flags = 0;
        func_800CF3AC(D_800E1DA4, &color, D_800E27EC);
        scale = 6144 + (D_800E27EC << 7);
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428) palette += 4;
        func_800CEE20(state, &rotation, scale, scale,
                     192 + (s16)D_800F3368.parameter02 * (D_800E27EC / 8 + 2),
                     GetClut(192, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800DBE6C(int mode, GteShortVector *state)
{
    GteShortVector *particle;
    switch (mode) {
    case 0:
        state->x = D_800F32D0->actor->render_object.target_x;
        state->y = D_800F32D0->actor->render_object.target_y;
        state->z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 8, 20,
                            (FieldAnimTaskCallback)func_800DBCD8);
    case 1:
        if (D_800E27EC < 21) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->x = state->x + (rand() & 511) - 256;
                particle->y = state->y + (rand() & 511) - 256;
                particle->z = state->z + (rand() & 511) - 256;
            }
        }
        if (D_800E27EC >= 53) return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}
