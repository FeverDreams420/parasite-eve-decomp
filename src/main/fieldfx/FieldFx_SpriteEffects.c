#include "pe1/render_object.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_actor.h"
#include "pe1/field_effect_pool.h"
#include "pe1/room_ember_burst.h"
#include "pe1/field_sprite_draw.h"

/* One falling sprite: drifts with damped sideways speed, bounces off the
 * floor and spins through a seven-frame strip tinted by the scene track. */
int func_800D4928(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    u8 *track;
    int scale;
    int palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->x += state->velocity_x;
        state->y += state->velocity_y;
        state->z += state->velocity_z;
        state->velocity_x = state->velocity_x * 6 / 7;
        state->velocity_z = state->velocity_z * 6 / 7;
        if (state->y > 0)
            state->velocity_y *= -1;
        state->velocity_y += 3;
        if (D_800E27EC >= state->duration)
            return 1;
        break;
    case 2:
        switch (D_800E2368->variables[6]) {
        case 0:
            track = D_800E141C;
            break;
        case 1:
            track = D_800E1444;
            break;
        default:
            track = D_800E146C;
            break;
        }
        func_800CF3AC(track, &color, D_800E27EC * 48 / state->duration);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + (D_800E27EC << 7);
        rotation.flags = 0;
        scale = (D_800E27EC << 11) / state->duration + 0x400;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(32, palette);
        func_800CEE20(&position, &rotation, scale, scale,
                      (s16)D_800F3368.parameter02 * (D_800E27EC % 7) + 0xE8, clut, 1, 128, &color);
        break;
    }
    return 0;
}

/* Drops one falling sprite per frame for eight frames from above the
 * player (lower in scene variant 0) on an angle that turns by 0x200. */
int func_800D4C24(int mode, RenderPhaseEmitter *state)
{
    GteShortVector position;
    RenderBouncingSprite *sprite;
    int speed;
    int i;

    switch (mode) {
    case 0:
        state->phase = rand();
        return func_800CE560(D_800F33E0->end, 16, 16,
                             (FieldAnimTaskCallback)func_800D4928);
    case 1:
        if (D_800E27EC < 8) {
            for (i = 0; i < 1; i++) {
                sprite = func_800CE610(D_800F33E0->end);
                if (sprite) {
                    func_800CE870((char *)D_8009D254, 1, &position.x);
                    sprite->x = position.x;
                    if (D_800E2368->variables[6] == 0)
                        sprite->y = position.y - 600;
                    else
                        sprite->y = position.y - 400;
                    sprite->z = position.z;
                    speed = (rand() & 7) + 32;
                    sprite->velocity_x = rsin(state->phase) * speed / 4096;
                    sprite->velocity_z = rcos(state->phase) * speed / 4096;
                    sprite->velocity_y = -(rand() & 7) - 12;
                    sprite->duration = (rand() & 3) + 26;
                    sprite->angle = rand();
                    state->phase += 0x200;
                }
            }
        }
        if (D_800E27EC >= 40)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.extent_x *= 2;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[2]];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 32;
        break;
    }
    return 0;
}

int func_800D4EA4(int mode)
{
    GteShortVector position;
    RenderColor color;
    u8 *track;
    int scale;

    switch (mode) {
    case 0:
        break;
    case 1:
        if (D_800E27EC >= 20)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 200;
        func_800CE870((char *)D_8009D254, 1, &position.x);
        position.y -= 300;
        track = D_800E2368->variables[6] == 1 ? D_800E1518 : D_800E1540;
        func_800CF3AC(track, &color, D_800E27EC * 24 / 20);
        scale = rsin((D_800E27EC << 10) / 20) / 2 + 2048;
        func_800D004C(&position, 400, 400, 8, 0, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}

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
        FieldFx_TransformTranslation(&state->position, 0);
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

/* One sprite of the spiral: circles the shared anchor D_800E21D8 at radius
 * D_800E21D4 while sinking, and flickers through an eight-frame strip. */
int func_800D629C(int mode, RenderSpiralSprite *state)
{
    RenderColor color = D_800C22C8;
    int scale;
    int intensity;
    int palette;
    u16 clut;

    switch (mode) {
    case 1:
        state->position.y = state->height;
        state->position.x = D_800E21D8.x + D_800E21D4 * rcos(state->angle) / 4096;
        state->position.z = D_800E21D8.z + D_800E21D4 * rsin(state->angle) / 4096;
        state->height -= 7;
        state->angle += 2;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        scale = (D_800E27EC << 11) / 24 + 0x800;
        intensity = 128 - (D_800E27EC << 7) / 24;
        state->position.y = D_800E21D8.y - (rand() & 31);
        scale = scale * (((state->phase + D_800E27EC / 2) & 7) * 128 + 0x1000) / 4096;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        clut = GetClut(64, palette);
        func_800CEE20(&state->position, 0, scale * 2, scale,
                      (s16)D_800F3368.parameter02 * ((state->phase + D_800E27EC / 2) & 7) + 128,
                      clut, 1, intensity, &color);
        break;
    }
    return 0;
}

/* Spiral emitter on the ground under the actor: releases 24 circling
 * sprites (func_800D629C) on the first frame, publishes the anchor and the
 * swelling radius they follow, and draws a spinning model in two sizes. */
int func_800D6514(int mode, RenderArcingEmitter *state)
{
    GteMatrix matrix;
    GteVector scale;
    GteShortVector angles;
    RenderSpiralSprite *sprite;
    u16 *slots;
    u16 *tpages;
    int intensity;
    int size;
    int angle;
    int i;

    switch (mode) {
    case 0:
        state->phase = 0;
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        state->position.y = g_RoomFloorY->y;
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F32D4, 0, 0xC0);
        return func_800CE560(D_800F33E0->end, 16, 24,
                             (FieldEffectCallback)func_800D629C);
    case 1:
        if (D_800E27EC == 0) {
            for (i = 0, angle = 0; i < 24; i++, angle += 0xAA) {
                sprite = func_800CE610(D_800F33E0->end);
                if (sprite) {
                    sprite->height = state->position.y;
                    sprite->phase = rand();
                    sprite->angle = angle;
                }
            }
        }
        if (D_800E27EC >= 24)
            return 1;
        D_800E21D4 = rsin((D_800E27EC << 10) / 24) / 3;
        D_800E21D8.x = state->position.x;
        D_800E21D8.y = state->position.y;
        D_800E21D8.z = state->position.z;
        break;
    case 2:
        size = rsin((D_800E27EC << 10) / 24) / 2;
        intensity = rcos((D_800E27EC << 10) / 24) / 32;
        scale.x = scale.y = scale.z = size;
        angles.x = 0;
        angles.y = D_800E27EC << 5;
        angles.z = 0;
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        slots = D_800E11E4;
        D_800F3368.tpage = D_800E2850[slots[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        tpages = D_800E2850;
        D_800F3368.parameter06 = 0;
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        {
            int tpage = (u16)(tpages[slots[0]] | GetTPage(0, 1, 0, 0));
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428)
                palette += 4;
            FieldFx_ModelSetTexture(tpage, GetClut(0xC0, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(D_800F32D4);
        FieldFx_ModelScaleColors(D_800F32D4, intensity);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_800F32D4, &matrix);
        scale.x /= 2;
        scale.y /= 2;
        scale.z /= 2;
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        FieldFx_ModelDraw((union FieldTexturedModel *)D_800F32D4, &matrix);
        FieldFx_ModelRestoreColors(D_800F32D4);
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

/* Ring pulse at the actor target: a breathing disc for 32 frames and an
 * expanding ground ring over the first 17. */
int func_800D6A1C(int mode, RenderRingPulse *state)
{
    GteRotation rotation;
    RenderColor color = D_800C22CC;
    int scale;
    int intensity;

    switch (mode) {
    case 0:
        state->phase = rand();
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        state->raised.x = state->position.x;
        state->raised.y = state->position.y;
        state->raised.z = state->position.z;
        state->raised.y -= 0x1B8;
        return 0;
    case 1:
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 100;
        scale = rcos(D_800E27EC << 5) / 2 + 0x800;
        intensity = rcos(D_800E27EC << 5) / 32;
        func_800D004C(&state->position, 700, 700, 12, 0, scale, scale,
                      &color, 0, intensity, 1);
        D_800F3368.depth = 0;
        if (D_800E27EC < 17) {
            intensity = 128 - (D_800E27EC << 3);
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = D_800E27EC << 7;
            rotation.flags = 1;
            scale = rsin(D_800E27EC << 6) * 2;
            func_800D0728(&state->position, 800, 1000, 16, &rotation, scale,
                          scale, 0, &color, intensity, 1);
        }
        break;
    }
    return 0;
}

int func_800D6C58(int mode, RenderBouncingSprite *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22D0;
    int scale, intensity, palette;
    unsigned int frame, angle;
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
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        frame = D_800E27EC;
        intensity = 128;
        angle = state->angle;
        rotation.z = (angle << 8) + (frame << 7);
        palette = D_800E1204[D_800F3368.palette];
        scale = (state->angle & 0x7FF) + 0x400;
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale, 0xBC,
            GetClut(128, palette), 255, intensity, &color);
        break;
    }
    return 0;
}

int func_800D6E3C(int mode, RenderPhaseEmitter *state) {
    RenderBouncingSprite *particle;
    GteShortVector position;
    int count;
    int amplitude;

    switch (mode) {
    case 0:
        state->phase = rand();
        return func_800CE560(D_800F33E0->end, 16, 24,
                             (FieldAnimTaskCallback)func_800D6C58);
    case 1:
        if (D_800E27EC < 8) {
            for (count = 0; count < 3; count++) {
                particle = func_800CE610(D_800F33E0->end);
                if (particle != 0) {
                    position.x = D_800F32D0->actor->render_object.target_x;
                    position.y = D_800F32D0->actor->render_object.target_y;
                    position.z = D_800F32D0->actor->render_object.target_z;
                    particle->x = position.x;
                    particle->y = position.y;
                    particle->z = position.z;
                    state->phase += 0x955 + (rand() & 31);
                    amplitude = (rand() & 127) + 70;
                    particle->velocity_x = rsin(state->phase) * amplitude / 4096;
                    particle->velocity_z = rcos(state->phase) * amplitude / 4096;
                    particle->velocity_y = -(rand() & 31) - 36;
                    particle->duration = (rand() & 7) + 26;
                    particle->angle = rand();
                }
            }
        }
        if (D_800E27EC >= 40)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[0]];
        D_800F3368.palette = 0;
        func_800CEDA8(0);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}

/* Rising ember sparks: the drifting ember callback and the fan-sweep
 * spark template instance. */

int func_800D70C0(int mode, s16 *state) {
    s16 sp10[3];
    char sp18[8];

    switch (mode) {
    case 1:
        state[0] += (rand() & 7) - 3;
        state[1] += (rand() & 3) - 7;
        state[2] += (rand() & 7) - 3;

        if (D_800E27EC < 0x12) {
            break;
        }
        return 1;
    case 2:
        sp10[0] = state[0];
        sp10[1] = state[1];
        sp10[2] = state[2];
        func_800CF3AC(g_EmberColorTrack, sp18, D_800E27EC);
        func_800D1DEC(sp10, sp18, 0x80, 1);
        break;
    }

    return 0;
}

#define ROOM_FAN_SWEEP_SPARK_NAME func_800D71B8
#define ROOM_FAN_SWEEP_SPARK_COLOR D_800C22D4
#define ROOM_FAN_SWEEP_SPARK_ANCHOR D_800E21E0
#define ROOM_FAN_SWEEP_SPARK_TRAIL D_800E21E8
#define ROOM_FAN_SWEEP_SPARK_COS rcos
#define ROOM_FAN_SWEEP_SPARK_SIN rsin
#define ROOM_FAN_SWEEP_SPARK_RAND rand
#define ROOM_FAN_SWEEP_SPARK_PALETTE_LIT 7
#define ROOM_FAN_SWEEP_SPARK_PALETTE_BASE 3
#include "../../overlays/room_lib/RoomEffect_FanSweepSpark.inc"

/* Fan sweep controller: opens a spark pool and a trail pool, launches a
 * spark on a turning angle every other frame for 32 frames, updates the
 * trail for 70 frames and publishes its pool for the sparks.
 * Matrix loads are C; each GTE transfer uses its individual macro. */
int func_800D751C(int mode, FieldFanSweep *sweep)
{
    RoomFanSweepSpark *spark;
    int handles;

    switch (mode) {
    case 0:
        sweep->angle = rand();
        handles = func_800CE560(ANIM_POOL_AS(RoomEmberBurstChannel)->pool, 0x10, 0x18,
                                (FieldAnimCallbackListCallback)func_800D71B8);
        handles += func_800CE5AC(&sweep->pool, handles, 8, 0x12, func_800D70C0);
        return handles;
    case 1:
        if (D_800E27EC < 0x20 && (D_800E27EC & 1)) {
            spark = (RoomFanSweepSpark *)func_800CE610(ANIM_POOL_AS(RoomEmberBurstChannel)->pool);
            if (spark) {
                spark->angle = sweep->angle;
                spark->radius = 0xC8;
                spark->state = 0;
                spark->timer = 0;
                sweep->angle -= 0x555;
            }
        }
        if (D_800E27EC >= 0x46)
            return 1;
        func_800CE688(sweep->pool);
        D_800E21E8 = sweep->pool;
        break;
    case 2:
        {
            GteMatrix **slot;
            slot = &D_800BCFA4.value;
            gte_ldrotmatrix((const GteMatrixWords *)*slot);
            gte_ldtransmatrix((const GteMatrixWords *)*slot);
        }
        D_800F3368.depth = 8;
        func_800CE78C(sweep->pool);
        func_800CE870((char *)g_PlayerEntity, 1, &D_800E21E0.x);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

/* Glow under the fan sweep: a pulsing sprite at the actor for 60 frames,
 * plus a flickering ring while the event is not in stage 11. */
int func_800D7764(int mode, FieldFanSweepGlow *glow)
{
    GteRotation rotation;
    GteShortVector position;
    RenderColor color = D_800C22D8;
    int intensity;
    int scale;
    int kind;
    int palette;
    int ring;

    switch (mode) {
    case 0:
        glow->state = 0;
        glow->timer = 0;
        func_800CE870((char *)g_PlayerEntity, 0, &glow->position.x);
        break;
    case 1:
        if (D_800E27EC >= 60)
            return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 100;
        intensity = rsin((D_800E27EC << 11) / 60) / 32;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        scale = 0x2800;
        func_800CEE20(&glow->position, 0, scale, scale, 4,
                      GetClut(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2),
                      3, intensity, 0);
        if (ANIM_TASK_AS(FieldFanSweepEvent)->stage != 11) {
            position.x = glow->position.x;
            position.y = glow->position.y;
            position.z = glow->position.z;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC * 8;
            rotation.flags = 0;
            scale = (D_800E27EC & 1) * 60 + 0x1000;
            ring = ANIM_TASK_AS(FieldFanSweepEvent)->stage == 10 ? 1 : 3;
            func_800D0728(&position, 400, 500, 20, &rotation, scale, scale, 0,
                          &color, intensity, ring);
        }
        break;
    }
    return 0;
}

int func_800D7A1C(int mode, RenderArcingEffect *state)
{
    RenderColor color = D_800C22DC;
    GteShortVector position;
    int palette;
    switch (mode) {
    case 1:
        state->y -= state->velocity_y;
        state->velocity_y--;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E18C0, &color, D_800E27EC);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, 0, 4096, 4096, 0x8B,
                     GetClut(0x20, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800D7B70(int mode, RenderArcingEmitter *state)
{
    RenderColor color;
    GteShortVector position;
    GteRotation rotation;
    RenderArcingEffect *effect;
    int scale;
    switch (mode) {
    case 0:
        state->phase = rand();
        state->radius = 0;
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 8, 24,
                             (FieldAnimTaskCallback)func_800D7A1C);
    case 1:
        if (D_800E27EC < 51) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                effect->x = state->position.x + rcos(state->phase) * state->radius / 4096;
                effect->z = state->position.z + rsin(state->phase) * state->radius / 4096;
                effect->y = state->position.y;
                effect->velocity_y = rand() & 3;
                state->phase += 0x8AA + (rand() & 31);
            }
        }
        if (D_800E27EC >= 74)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 8;
        if (D_800E27EC < 51) {
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            rotation.x = 1024;
            rotation.y = 0;
            rotation.z = D_800E27EC << 5;
            rotation.flags = 1;
            scale = rsin((D_800E27EC << 10) / 50);
            state->radius = scale / 8;
            func_800CF3AC(D_800E18F0, &color, D_800E27EC);
            func_800D0728(&position, 350, 500, 20, &rotation,
                          scale, scale, 0, &color, 128, 1);
        }
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        break;
    }
    return 0;
}

int func_800D7E78(int mode, GteShortVector *state)
{
    GteShortVector position;
    RenderColor color;
    int scale, clut_y;
    switch (mode) {
    case 1:
        state->y--;
        if (D_800E27EC >= 6)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        func_800CF3AC(D_800E1988, &color, D_800E27EC);
        scale = 4096 - (D_800E27EC << 12) / 6;
        clut_y = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            clut_y += 4;
        func_800CEE20(&position, 0, scale, scale, 0x8A,
                     GetClut(0x70, clut_y), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800D7FBC(int mode, RenderOrbitingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22E0;
    GteShortVector *particle;
    int scale, intensity, palette, interval;
    switch (mode) {
    case 1:
        state->x = D_800E21EC.x + rcos(state->angle) * state->radius / 4096;
        state->z = D_800E21EC.z + rsin(state->angle) * state->radius / 4096;
        state->angle += 96;
        state->radius = rcos((D_800E27EC << 10) / 36) * 700 / 4096;
        interval = D_800E2368->variables[6] == 11 ? 1 : 3;
        if (D_800E27EC % interval == 0) {
            particle = func_800CE610(D_800E21F4);
            if (particle) {
                particle->x = state->x + (rand() & 7) - 3;
                particle->y = state->y + (rand() & 7) - 3;
                particle->z = state->z + (rand() & 7) - 3;
            }
        }
        state->timer++;
        if (D_800E27EC >= 36)
            return 1;
        break;
    case 2:
        switch (state->stage) {
        case 0:
            intensity = rsin((state->timer << 10) / 12) / 32;
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 1;
            }
            break;
        case 1:
            scale = 4096;
            intensity = 128;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            intensity = 128;
            scale = rcos((state->timer << 10) / 12);
            state->radius += state->timer * 4;
            break;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        rotation.flags = 0;
        scale *= 2;
        interval = D_800F3368.palette;
        palette = D_800E1204[interval];
        func_800CEE20(&position, &rotation, scale, scale, 36,
                     GetClut(0, (interval == 4 && D_800F3428) ? palette + 7
                                                               : palette + 3),
                     1, intensity, &color);
        break;
    }
    return 0;
}

int func_800D8388(int mode, RenderOrbitingEmitter *state)
{
    GteShortVector position;
    RenderOrbitingEffect *effect;
    int count;
    int phase;

    switch (mode) {
    case 0:
        state->phase = rand();
        count = func_800CE560(D_800F33E0->end, 16, 12,
                              (FieldAnimTaskCallback)func_800D7FBC);
        count += func_800CE5AC(&state->particles, count, 8, 32,
                               func_800D7E78);
        return count;
    case 1:
        if (D_800E27EC < 44 && D_800E27EC % 3 == 0) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                phase = state->phase;
                effect->stage = 0;
                effect->timer = 0;
                effect->radius = 700;
                effect->angle = phase;
                func_800CE870((char *)D_8009D254, 1, &position.x);
                effect->y = position.y + (rand() & 511) - 655;
                state->phase -= 0x555;
            }
        }
        if (D_800E27EC >= 70)
            return 1;
        func_800CE688(state->particles);
        D_800E21F4 = state->particles;
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[1]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        func_800CE78C(state->particles);
        func_800CE870((char *)D_8009D254, 1, &D_800E21EC.x);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

/* Matrix loads are C; each GTE transfer uses its individual macro. */
int func_800D868C(int mode, RenderOrbitingEmitter *state)
{
    GteShortVector position;
    RenderOrbitingEffect *effect;
    int count;
    int phase;
    switch (mode) {
    case 0:
        state->phase = rand();
        count = func_800CE560(D_800F33E0->end, 16, 3,
                             (FieldAnimTaskCallback)func_800D7FBC);
        count += func_800CE5AC(&state->particles, count, 8, 18,
                                   func_800D7E78);
        return count;
    case 1:
        if (D_800E27EC == 1) {
            for (count = 0; count < 3; ++count) {
                effect = func_800CE610(D_800F33E0->end);
                if (effect) {
                    phase = state->phase;
                    effect->stage = 0;
                    effect->timer = 0;
                    effect->radius = 700;
                    effect->angle = phase;
                    func_800CE870((char *)D_8009D254, 1, &position.x);
                    effect->y = position.y - 400;
                    state->phase -= 0x555;
                }
            }
        }
        if (D_800E27EC >= 43)
            return 1;
        func_800CE688(state->particles);
        D_800E21F4 = state->particles;
        break;
    case 2:
        {
            GteMatrix **slot;
            slot = &D_800BCFA4.value;
            gte_ldrotmatrix((const GteMatrixWords *)*slot);
            gte_ldtransmatrix((const GteMatrixWords *)*slot);
        }
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[1]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        func_800CE78C(state->particles);
        func_800CE870((char *)D_8009D254, 1, &D_800E21EC.x);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

int func_800D8978(int mode, RenderVerticalEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale, palette, selector;
    switch (mode) {
    case 1:
        state->y = D_800E21F8.y + (-D_800E27EC * 900) / 24;
        state->x = D_800E21F8.x;
        state->z = D_800E21F8.z;
        state->angle += 128;
        state->timer++;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        if (state->stage == 0) {
            scale = 8192 - rsin(state->timer << 7);
            if (state->timer >= 8) {
                state->timer = 0;
                state->stage = 1;
            }
        } else {
            scale = 4096;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 1024;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        rotation.flags = 1;
        func_800CF3AC(D_800E1A14, &color, D_800E27EC);
        scale *= 2;
        selector = D_800F3368.palette;
        palette = D_800E1204[selector];
        func_800CEE20(&position, &rotation, scale, scale, 68,
                     GetClut(0, (selector == 4 && D_800F3428) ? palette + 7
                                                               : palette + 3),
                     1, 128, &color);
        break;
    }
    return 0;
}

int func_800D8B6C(int mode, void *state)
{
    GteShortVector position;
    RenderVerticalEffect *effect;
    switch (mode) {
    case 0:
        return func_800CE560(D_800F33E0->end, 12, 14,
                            (FieldAnimTaskCallback)func_800D8978);
    case 1:
        if (D_800E27EC < 32 && (D_800E27EC & 1)) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                effect->angle = rand();
                effect->stage = 0;
                effect->timer = 0;
                func_800CE870((char *)D_8009D254, 1, &position.x);
                effect->y = position.y - 512;
            }
        }
        if (D_800E27EC >= 50)
            return 1;
        /* Fall through: refresh the center and render parameters. */
    case 2:
        func_800CE870((char *)D_8009D254, 1, &D_800E21F8.x);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}

int func_800D8D14(int mode, GteShortVector *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale, palette;
    switch (mode) {
    case 1:
        state->y -= 3;
        if (D_800E27EC >= 12)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 6;
        rotation.flags = 0;
        func_800CF3AC(D_800E1AA0, &color, D_800E27EC);
        scale = 4096 - (D_800E27EC << 12) / 12;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale, 138,
                     GetClut(112, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800D8E74(int mode, RenderHelicalEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color = D_800C22E0;
    GteShortVector *particle;
    int scale, palette, interval;
    switch (mode) {
    case 1:
        state->x = D_800E2200.x + rcos(state->angle) * state->radius / 4096;
        state->z = D_800E2200.z + rsin(state->angle) * state->radius / 4096;
        if (state->direction)
            state->y = D_800E2200.y - D_800E27EC * 500 / 36;
        else
            state->y = (s16)(D_800E27EC * 500 / 36 - 1000) + D_800E2200.y;
        state->angle += 96;
        state->radius = rcos((D_800E27EC << 10) / 36) * 700 / 4096;
        if ((D_800E27EC & 1) == 0) {
            particle = func_800CE610(D_800E2208);
            if (particle) {
                particle->x = state->x + (rand() & 7) - 3;
                particle->y = state->y + (rand() & 7) - 3;
                particle->z = state->z + (rand() & 7) - 3;
            }
        }
        state->timer++;
        if (D_800E27EC >= 36)
            return 1;
        break;
    case 2:
        switch (state->stage) {
        case 0:
            rsin((state->timer << 10) / 12);
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 1;
            }
            break;
        case 1:
            scale = 4096;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            scale = rcos((state->timer << 10) / 12);
            state->radius += state->timer * 4;
            break;
        }
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        rotation.flags = 0;
        scale *= 2;
        interval = D_800F3368.palette;
        palette = D_800E1204[interval];
        func_800CEE20(&position, &rotation, scale, scale, 228,
                     GetClut(0, (interval == 4 && D_800F3428) ? palette + 6 : palette + 2),
                     1, 128, &color);
        break;
    }
    return 0;
}

/* Matrix loads are C; each GTE transfer uses its individual macro. */
int func_800D927C(int mode, RenderOrbitingEmitter *state)
{
    RenderHelicalEffect *effect;
    int count;
    int phase;
    switch (mode) {
    case 0:
        state->phase = rand();
        count = func_800CE560(D_800F33E0->end, 16, 4,
                             (FieldAnimTaskCallback)func_800D8E74);
        count += func_800CE5AC(&state->particles, count, 8, 32,
                                   func_800D8D14);
        return count;
    case 1:
        if (D_800E27EC < 2) {
            for (count = 0; count < 2; ++count) {
                effect = func_800CE610(D_800F33E0->end);
                if (effect) {
                    phase = state->phase;
                    effect->stage = 0;
                    effect->timer = 0;
                    effect->radius = 700;
                    effect->angle = phase + (count << 11);
                    effect->direction = D_800E27EC;
                }
            }
            state->phase += 1024;
        }
        if (D_800E27EC >= 70)
            return 1;
        func_800CE688(state->particles);
        D_800E2208 = state->particles;
        break;
    case 2:
        {
            GteMatrix **slot;
            slot = &D_800BCFA4.value;
            gte_ldrotmatrix((const GteMatrixWords *)*slot);
            gte_ldtransmatrix((const GteMatrixWords *)*slot);
        }
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        D_800F3368.tpage = D_800E2850[D_800E11E4[1]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        func_800CE78C(state->particles);
        func_800CE870((char *)D_8009D254, 1, &D_800E2200.x);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

int func_800D9554(int mode, RenderDampedSpark *state)
{
    GteShortVector position;
    u32 color;
    switch (mode) {
    case 1:
        state->x += state->vx + (rand() & 3) - 1;
        state->y += state->vy + (rand() & 3) - 1;
        state->z += state->vz;
        state->vx = state->vx * 31 / 32;
        state->vz = state->vz * 31 / 32;
        if (D_800E27EC < 19)
            state->vy--;
        else
            state->vy++;
        if (D_800E27EC >= 40)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        if (D_800E27EC < 19)
            func_800CF3AC(D_800E1AC8, &color, D_800E27EC);
        else if ((rand() & 3) == 0)
            color = 0xC8C8;
        else
            color = 0;
        func_800D1DEC(&position, &color, 128, 1);
        break;
    }
    return 0;
}

/* Matrix loads are C; each GTE transfer uses its individual macro. */
int func_800D96F4(int mode, RenderSparkEmitter *state)
{
    GteRotation rotation;
    RenderColor color = D_800C22E4;
    RenderDampedSpark *effect;
    int count, speed, scale, intensity, palette, selector;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 12, 24,
                            (FieldAnimTaskCallback)func_800D9554);
    case 1:
        if (D_800E27EC < 12) {
            for (count = 0; count < 2; ++count) {
                effect = func_800CE610(D_800F33E0->end);
                if (effect) {
                    effect->x = state->position.x;
                    effect->y = state->position.y;
                    effect->z = state->position.z;
                    speed = (rand() & 7) + 43;
                    effect->vx = rsin(state->phase) * speed / 4096;
                    effect->vz = rcos(state->phase) * speed / 4096;
                    effect->vy = (rand() & 31) - 10;
                    state->phase += 0x955;
                }
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
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 100;
        if (D_800E27EC < 33) {
            intensity = rcos(D_800E27EC << 5) / 32;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC << 4;
            rotation.flags = 0;
            scale = rsin(D_800E27EC << 5) + 4096;
            selector = D_800F3368.palette;
            palette = D_800E1204[selector];
            func_800CEE20(&state->position, &rotation, scale, scale, 66,
                         GetClut(0, (selector == 4 && D_800F3428) ? palette + 7 : palette + 3),
                         1, intensity, &color);
        }
        {
            GteMatrix **slot;
            slot = &D_800BCFA4.value;
            gte_ldrotmatrix((const GteMatrixWords *)*slot);
            gte_ldtransmatrix((const GteMatrixWords *)*slot);
        }
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}

#define SPINNING_MODEL_FUNC func_800D9A8C
#define SPINNING_MODEL_ANCHOR D_800E220C
#define SPINNING_MODEL_PALETTE 4
#define SPINNING_MODEL_COLOR(size) 0, (u8)((size) / 2), (u8)((size) / 8)
#include "FieldFx_SpinningModel.inc"

int func_800D9E5C(int mode, RenderArcingEffect *state)
{
    GteShortVector position;
    GteRotation rotation;
    int intensity, palette;
    switch (mode) {
    case 1:
        state->y += state->velocity_y;
        if (D_800E27EC < 19)
            state->velocity_y -= 2;
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = D_800E27EC << 7;
        rotation.z = 0;
        rotation.flags = 1;
        intensity = rsin((D_800E27EC << 11) / 24) / 32;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, 4096, 4096, 136,
                     GetClut(96, palette), 1, intensity, 0);
        break;
    }
    return 0;
}

int func_800D9FD4(int mode, GteShortVector *state)
{
    RenderArcingEffect *particle;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 1, &state->x);
        return func_800CE560(D_800F33E0->end, 8, 12,
                            (FieldAnimTaskCallback)func_800D9E5C);
    case 1:
        if (D_800E27EC < 32 && (D_800E27EC & 1)) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->x = state->x + rand() % 400 - 200;
                particle->y = state->y;
                particle->z = state->z + rand() % 400 - 200;
                particle->velocity_y = -(rand() & 7) - 16;
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
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}

/* The second spinning model flare and the emitter that spawns it on the
 * attached actor's target point. */
#define SPINNING_MODEL_FUNC func_800DA1FC
#define SPINNING_MODEL_ANCHOR D_800E2214
#define SPINNING_MODEL_PALETTE 3
#define SPINNING_MODEL_COLOR(size) (u8)((size) / 3), 0, (u8)((size) / 2)
#include "FieldFx_SpinningModel.inc"

int func_800DA5D4(int mode, FieldAnimEmitter *state) {
    FieldAnimEmittedPoint *point;
    int angle;

    switch (mode) {
    case 0:
        state->angle = rand();
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F32D8, 0, 0);
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
