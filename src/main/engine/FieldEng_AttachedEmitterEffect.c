#include "pe1/field_anim.h"
#include "pe1/render_object.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/field_actor.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_effect_pool.h"

/* The second spinning model flare and the emitter that spawns it on the
 * attached actor's target point. */
#define SPINNING_MODEL_FUNC func_800DA1FC
#define SPINNING_MODEL_ANCHOR D_800E2214
#define SPINNING_MODEL_PALETTE 3
#define SPINNING_MODEL_COLOR(size) (u8)((size) / 3), 0, (u8)((size) / 2)
#include "FieldEng_SpinningModel.inc"

int func_800DA5D4(int mode, FieldAnimEmitter *state) {
    FieldAnimEmittedPoint *point;
    int angle;

    switch (mode) {
    case 0:
        state->angle = rand();
        func_800C6D5C((union FieldTexturedModel *)D_800F32D8, 0, 0);
        return func_800CE560(D_800F33E0->end, 16, 24,
                             (FieldAnimCallbackListCallback)func_800DA1FC);
    case 1:
        if (D_800E27EC < 40 && D_800E27EC % 6 == 0) {
            point = func_800CE610(D_800F33E0->end);
            if (point) {
                angle = state->angle;
                point->state = 0;
                point->phase = 0;
                point->angle = angle;
                state->angle -= 1365 + (rand() & 255);
            }
        }
        if (D_800E27EC >= 70)
            return 1;
        break;
    case 2:
        D_800E2214[0] = D_800F32D0->actor->render_object.target_x;
        D_800E2214[1] = D_800F32D0->actor->render_object.target_y;
        D_800E2214[2] = D_800F32D0->actor->render_object.target_z;
        D_800E2214[1] = g_RoomFloorY->y;
        break;
    }
    return 0;
}

int func_800DA780(int mode, RenderArcingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, palette;
    switch (mode) {
    case 1:
        state->y += state->velocity_y;
        if (D_800E27EC < 19)
            state->velocity_y -= 1;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->y + state->x + (D_800E27EC << 5);
        rotation.flags = 0;
        intensity = rsin((D_800E27EC << 11) / 24) / 128;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, 4096, 4096,
                     96 + (s16)D_800F3368.parameter02 * (D_800E27EC / 6),
                     GetClut(96, palette), 1, intensity, 0);
        break;
    }
    return 0;
}

int func_800DA934(int mode, GteShortVector *state)
{
    RenderArcingEffect *particle;
    switch (mode) {
    case 0:
        state->x = D_800F32D0->actor->render_object.target_x;
        state->y = D_800F32D0->actor->render_object.target_y;
        state->z = D_800F32D0->actor->render_object.target_z;
        state->y = g_RoomFloorY->y;
        return func_800CE560(D_800F33E0->end, 8, 12,
                             (FieldEffectCallback)func_800DA780);
    case 1:
        if (D_800E27EC < 40 && (D_800E27EC & 1)) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->x = state->x + rand() % 400 - 200;
                particle->y = state->y - 800;
                particle->z = state->z + rand() % 400 - 200;
                particle->velocity_y = (rand() & 7) + 42;
            }
        }
        if (D_800E27EC >= 70)
            return 1;
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
        D_800F3368.depth = 32;
        break;
    }
    return 0;
}
