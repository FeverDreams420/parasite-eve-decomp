/* MASPSX_FLAGS: --expand-div */
#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_anim.h"
#include "pe1/random.h"
#include "pe1/field_actor.h"
#include "pe1/battle_runtime.h"

int func_800D5010(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, scale, palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->x += state->velocity_x;
        state->y += state->velocity_y;
        state->z += state->velocity_z;
        state->velocity_x = state->velocity_x * 31 / 32;
        state->velocity_z = state->velocity_z * 31 / 32;
        if (state->y > 0)
            state->velocity_y *= -1;
        state->velocity_y += 3;
        if (D_800E27EC >= state->duration)
            return 1;
        break;
    case 2:
        intensity = 128 - (D_800E27EC << 7) / state->duration;
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + (D_800E27EC << 3);
        scale = rsin((D_800E27EC << 10) / state->duration) + 4096;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(64, palette);
        func_800CEE20(&position, &rotation, scale, scale,
            (s16)D_800F3368.parameter02 * ((D_800E27EC << 3) / state->duration) + 128,
            clut, 1, intensity, 0);
        break;
    }
    return 0;
}


/* Scatter burst at the actor target: opens a pool of bouncing sprites
 * (func_800D5010), throws two per frame for six frames on a turning angle,
 * and draws a growing flash, a short ring and a ground ring. */
int func_800D5290(int mode, RenderArcingEmitter *state)
{
    GteRotation rotation;
    GteShortVector position;
    RenderColor color = D_800C22BC;
    RenderBouncingSprite *sprite;
    int speed;
    int intensity;
    int scale;
    int kind;
    int palette;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 16, 12,
                             (FieldAnimTaskCallback)func_800D5010);
    case 1:
        if (D_800E27EC < 6) {
            for (i = 0; i < 2; i++) {
                sprite = func_800CE610(D_800F33E0->end);
                if (sprite) {
                    sprite->x = state->position.x;
                    sprite->y = state->position.y;
                    sprite->z = state->position.z;
                    speed = (rand() & 15) + 43;
                    sprite->velocity_x = rsin(state->phase) * speed / 4096;
                    sprite->velocity_z = rcos(state->phase) * speed / 4096;
                    sprite->velocity_y = -(rand() & 7) - 12;
                    sprite->duration = (rand() & 3) + 17;
                    sprite->angle = rand();
                    state->phase += 0x955;
                }
            }
        }
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[8]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 100;
        state->position.y -= 6;
        intensity = rcos((D_800E27EC << 10) / 24) / 32;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 4;
        rotation.flags = 0;
        scale = 0x3000 - rcos((D_800E27EC << 10) / 12);
        i = (D_800E27EC << 3) / 12;
        if (i > 7)
            i = 7;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        func_800CEE20(&state->position, &rotation, scale, scale,
                      (s16)D_800F3368.parameter02 * i,
                      GetClut(0, (kind == 4 && D_800F3428) ? palette + 5 : palette + 1),
                      1, intensity, 0);
        if (D_800E27EC < 9) {
            intensity = rcos(D_800E27EC << 7) / 32;
            func_800D004C(&state->position, 250, 250, 12, 0, scale, scale,
                          &color, 0, intensity, 1);
        }
        D_800F3368.depth = 4;
        if ((unsigned)(D_800E27EC - 4) < 13) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = D_800E27EC << 7;
            rotation.flags = 1;
            intensity = 128 - ((D_800E27EC - 4) << 7) / 12;
            scale = rsin(((D_800E27EC - 4) << 10) / 12) * 2;
            position.y = g_RoomFloorY->y;
            func_800D0728(&position, 250, 400, 12, &rotation, scale, scale, 0,
                          &color, intensity, 1);
        }
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 64;
        break;
    }
    return 0;
}

int func_800D5898(int mode, RenderCosineEffect *state)
{
    RenderColor color = D_800C22C0;

    switch (mode) {
    case 1:
        state->x = state->amplitude
            * rcos((D_800E27EC << 10) / state->duration) / 4096;
        if (D_800E27EC >= state->duration)
            return 1;
        break;
    case 2:
        func_800D27FC(state->x, state->y, &color,
            rcos((D_800E27EC << 10) / state->duration) / 64 + 64, 1);
        break;
    }
    return 0;
}


/* Arcing emitter: spawns three cosine sparks per frame between frames 4
 * and 10, draws the expanding ring for 13 frames and anchors the effect at
 * its position under the current view matrix. */
int func_800D5A00(int mode, RenderArcingEmitter *state)
{
    GteShortVector position;
    RenderColor color = D_800C22C4;
    RenderCosineEffect *effect;
    int scale;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->radius = 0;
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 8, 24,
                             (FieldAnimTaskCallback)func_800D5898);
    case 1:
        if (D_800E27EC >= 4 && D_800E27EC < 11) {
            for (i = 0; i < 3; i++) {
                effect = func_800CE610(D_800F33E0->end);
                if (effect) {
                    effect->amplitude = state->radius;
                    effect->duration = (rand() & 3) + 12;
                    effect->y = state->phase;
                    state->phase += 0x8AA + (rand() & 31);
                }
            }
        }
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 64;
        if (D_800E27EC < 13) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = rsin((D_800E27EC << 10) / 12);
            state->radius = scale / 8;
            func_800CF3AC(D_800E166C, &color, D_800E27EC);
            func_800D0728(&position, 300, 512, 12, 0, scale, scale, 0,
                          &color, 128, 1);
        }
        D_800F3368.depth = 64;
        gte_ldrotmatrix((const GteMatrixWords *)D_800BCFA4.value);
        gte_ldtransmatrix((const GteMatrixWords *)D_800BCFA4.value);
        FieldEng_TransformTranslation(&state->position, 0);
        break;
    }
    return 0;
}

static inline int TextureIndex(unsigned int factor, unsigned int step)
{
    return factor * step + 128;
}

int func_800D5CE4(int mode, RenderRisingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale;
    u16 clut;
    int y;
    int random;
    u16 velocity;
    unsigned int frame;
    int phase;
    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        random = rand();
        velocity = state->vy + 2;
        state->vy = velocity + (random & 1);
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1694, &color, D_800E27EC);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        frame = D_800E27EC;
        rotation.z = state->angle + (frame << 5);
        phase = frame << 10;
        scale = rsin(phase / 24) + 0x1000;
        y = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            y += 4;
        clut = GetClut(32, y);
        func_800CEE20(&position, &rotation, scale, scale,
            TextureIndex((s16)D_800F3368.parameter02, D_800E27EC / 3),
            clut, 1, 128, &color);
        break;
    }
    return 0;
}

/* Rising effect emitter at the actor target: plays sound 0x487, opens a pool
 * of rising sprites (func_800D5CE4), scatters one per frame for eight frames
 * and draws a slowly turning glow while the anchor climbs. */
int func_800D5EB4(int mode, RenderArcingEmitter *state)
{
    GteRotation rotation;
    RenderRisingEffect *effect;
    int speed;
    int intensity;
    int scale;
    int kind;
    int palette;
    u16 clut;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        Asset_Find08Alt(0x487, 0, state->position.x, state->position.y,
                        state->position.z);
        return func_800CE560(D_800F33E0->end, 16, 8,
                             (FieldAnimTaskCallback)func_800D5CE4);
    case 1:
        if (D_800E27EC < 8) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                effect->x = state->position.x + (rand() & 0x7F) - 64;
                effect->y = state->position.y + (rand() & 0xFF) - 128;
                effect->z = state->position.z + (rand() & 0x7F) - 64;
                speed = (rand() & 7) + 16;
                effect->vx = rsin(state->phase) * speed / 4096;
                effect->vz = rcos(state->phase) * speed / 4096;
                effect->vy = -(rand() & 7) - 12;
                effect->angle = rand();
                state->phase += 0x955;
            }
        }
        if (D_800E27EC >= 30)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 64;
        state->position.y -= 4;
        intensity = rcos((D_800E27EC << 10) / 30) / 32;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = -0x200;
        rotation.flags = 0;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        scale = 0x2400;
        if (kind == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(32, palette);
        func_800CEE20(&state->position, &rotation, scale, scale, 0x6C, clut,
                      1, intensity, 0);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 64;
        break;
    }
    return 0;
}
