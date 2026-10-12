#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/psyq_gpu.h"
#include "pe1/random.h"
#include "pe1/field_effect_pool.h"
#include "pe1/field_anim.h"
#include "pe1/field_actor.h"

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
        FieldModel_ScrollUv((union FieldTexturedModel *)D_800F32D4, 0, 0xC0);
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
            GsSetOrign(tpage, GetClut(0xC0, palette));
        }
        FieldModel_SetSemiTrans(1);
        FieldModel_SaveColors(D_800F32D4);
        FieldModel_ScaleColors(D_800F32D4, intensity);
        FieldModel_Draw((union FieldTexturedModel *)D_800F32D4, &matrix);
        scale.x /= 2;
        scale.y /= 2;
        scale.z /= 2;
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        FieldModel_Draw((union FieldTexturedModel *)D_800F32D4, &matrix);
        FieldModel_RestoreColors(D_800F32D4);
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
