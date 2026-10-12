/* MASPSX_FLAGS: --expand-div */

#include "pe1/render_object.h"
#include "pe1/gte.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/random.h"
#include "pe1/render_prim.h"
#include "pe1/field_actor.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_model_draw.h"
#include "common.h"
#include "pe1/field_flash_burst.h"
#include "pe1/field_spin_glow.h"
#include "pe1/field_glow_fountain.h"
#include "pe1/field_fade_particle.h"

int FieldFx_CosineEffect(int mode, RenderCosineEffect *effect) {
    u32 color;

    switch (mode) {
    case 1:
        effect->x = (effect->amplitude
            * rcos((D_800E27EC << 10) / effect->duration)) / 4096;
        effect->y += 16;
        if (D_800E27EC >= effect->duration)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1C04, &color, D_800E27EC);
        func_800D27FC(effect->x, effect->y, &color, 128, 1);
        break;
    }
    return 0;
}

int func_800DACA4(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor first = D_800C22E8;
    RenderColor second = D_800C22EC;
    RenderCosineEffect *particle;
    int scale, intensity;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 8, 24, (FieldAnimTaskCallback)FieldFx_CosineEffect);
    case 1:
        if (D_800E27EC < 51) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->amplitude = (rand() & 255) + 500;
                particle->duration = (rand() & 3) + 22;
                particle->y = state->phase;
                state->phase += 2218 + (rand() & 31);
            }
        }
        if (D_800E27EC >= 80) return 1;
        break;
    case 2:
        if (D_800E27EC <= 80) {
            D_800F3368.depth = 60;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = (D_800E27EC << 12) / 80;
            intensity = rsin((D_800E27EC << 11) / 80) / 32;
            func_800D004C(&position, 800, 800, 16, 0, scale, scale, &first, 0, intensity, 1);
            func_800D004C(&position, 250, 250, 8, 0, scale, scale, &second, 0, intensity, 1);
            func_800D0728(&position, 590, 670, 16, 0, scale, scale, 0, &first, intensity, 3);
        }
        D_800F3368.depth = 64;
        FieldFx_TransformTranslation(&state->position, 0);
        break;
    }
    return 0;
}

int func_800DAF8C(int mode, RenderSineEffect *state)
{
    RenderColor color;
    switch (mode) {
    case 1:
        state->position.y += state->velocity_y;
        state->position.z += 8;
        state->position.x = state->amplitude - state->amplitude * D_800E27EC / 64;
        state->angle = rsin(D_800E27EC << 5) / 64;
        state->scale = 1600 - D_800E27EC * 1000 / 64;
        if (D_800E27EC >= 64)
            return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1C2C, &color, D_800E27EC);
        func_800D0E88(&D_800E221C, (GteRotation *)&state->position, state->scale, state->angle,
                     &color, 0, 0, 128, 1);
        break;
    }
    return 0;
}

int func_800DB0D0(int mode, RenderSineEmitter *state)
{
    RenderSineEffect *effect;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 16, 16,
                             (FieldAnimTaskCallback)func_800DAF8C);
    case 1:
        if (D_800E27EC < 17) {
            effect = func_800CE610(D_800F33E0->end);
            if (effect) {
                effect->amplitude = (rand() & 1023) - 512;
                effect->position.y = state->phase;
                effect->position.z = rand();
                effect->velocity_y = (rand() & 127) - 64;
                state->phase += 0x8AA + (rand() & 31);
            }
        }
        if (D_800E27EC >= 73)
            return 1;
        break;
    case 2:
        D_800E221C.x = state->position.x;
        D_800E221C.y = state->position.y;
        D_800E221C.z = state->position.z;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}

void func_800DB25C(GteShortVector *position, int x, int y, int angle,
                   int radius, int depth, int intensity, RenderColor *color)
{
    GteShortVector center, p0, p1, p2, p3;
    RenderLinePacket *line;
    int r, g, b, dx, dy;
    line = (RenderLinePacket *)(D_800B0E38.packets[D_8009CDDC] + D_8009CDD8);
    D_8009CDD8 += 64;
    gte_lwc2_0_0(position);
    gte_lwc2_1_4(position);
    gte_rtps();
    r = color->r * intensity / 128;
    g = color->g * intensity / 128;
    b = color->b * intensity / 128;
    gte_stsxy2(&center);
    center.x += x;
    center.y += y;
    dx = rcos(angle) * radius / 4096;
    dy = rsin(angle) * radius / 4096;
    p0.x = center.x + dx;
    p0.y = center.y + dy;
    p2.x = center.x - dx;
    p2.y = center.y - dy;
    angle += 1024;
    dx = rcos(angle) * radius / 4096;
    dy = rsin(angle) * radius / 4096;
    p1.x = center.x + dx;
    p1.y = center.y + dy;
    p3.x = center.x - dx;
    p3.y = center.y - dy;
#define EDGE(a, z) \
    SetLineF2(line); \
    line->r = r; line->g = g; line->b = b; \
    line->x0 = a.x; line->y0 = a.y; \
    line->x1 = z.x; line->y1 = z.y; \
    func_800CF6F8(D_800B0E38.ordering[D_8009CDDC] + 4, line, depth)
    EDGE(p0, p1);
    line++;
    EDGE(p1, p2);
    line++;
    EDGE(p2, p3);
    line++;
    EDGE(p3, p0);
#undef EDGE
}

int func_800DB5F4(int mode, RenderDiamondParticle *state)
{
    RenderColor color;
    int fade;
    switch (mode) {
    case 1:
        if (D_800E27EC < 8) break;
        return 1;
    case 2:
        fade = 128 - (D_800E27EC << 4);
        func_800CF3AC(D_800E1CC8, &color, state->color_time);
        func_800DB25C(&D_800E2224, state->x, state->y, state->angle, state->size, 1, fade, &color);
        break;
    }
    return 0;
}

int func_800DB6BC(int mode, RenderDiamondEmitter *state)
{
    RenderColor color;
    RenderDiamondParticle *particle;
    int phase;
    switch (mode) {
    case 0:
        state->position.x = D_800F32D0->actor->render_object.target_x;
        state->position.y = D_800F32D0->actor->render_object.target_y;
        state->position.z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 12, 10, (FieldAnimTaskCallback)func_800DB5F4);
    case 1:
        if (D_800E27EC < 25) {
            state->angle = -D_800E27EC * 64;
            state->radius = 150 - D_800E27EC * 150 / 24;
            state->x = rcos(state->angle * 2) * state->radius / 4096;
            state->y = rsin(state->angle * 2) * state->radius / 4096;
            state->size = rcos((D_800E27EC << 10) / 24) * 80 / 4096 + 32;
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->x = state->x;
                particle->y = state->y;
                particle->angle = state->angle;
                particle->size = state->size;
            }
        } else {
            state->y = 0;
            state->x = 0;
        }
        if (D_800E27EC >= 72) return 1;
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);

        D_800E2224.x = state->position.x;
        D_800E2224.y = state->position.y;
        D_800E2224.z = state->position.z;
        func_800CF3AC(D_800E1CC8, &color, D_800E27EC);
        func_800DB25C(&D_800E2224, state->x, state->y, state->angle, state->size, 1, 128, &color);
        phase = D_800E27EC - 24;
        if ((unsigned)phase < 16) {
            int angle = phase * 64;
            func_800DB25C(&D_800E2224, state->x, state->y, state->angle, state->size + rcos(angle)/16, 1, 64, &color);
            func_800DB25C(&D_800E2224, state->x, state->y, state->angle, state->size + rcos(angle)/32, 1, 96, &color);
        }
        break;
    }
    return 0;
}

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

/* Matching debt: 3 GTE transfer pins, 4 empty constraints, and the three
 * TU-local register reservations above. The unpinned matrix pointer lets
 * GCC reuse t1 for constant-division multiply-high results. */
int func_800DC058(int mode, FieldAnimTwinModel *state)
{
    GteShortVector angles;
    GteShortVector position;
    GteVector scale;
    RenderColor color;
    GteMatrix matrix;
    int height;
    int spin;
    int size;

    switch (mode) {
    case 0:
        state->angle = rand();
        state->stage = 0;
        state->timer = 0;
        func_800CE870((char *)D_8009D254, 1, &state->position.x);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F3418, 0, 0);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F342C, 0, 0);
        break;
    case 1:
        state->timer++;
        if (D_800E27EC >= 64)
            return 1;
        break;
    case 2:
        spin = 0;
        state->angle += 16;
        switch (state->stage) {
        case 0:
            height = (state->timer << 12) / 24;
            if (state->timer >= 24) {
                state->stage = 1;
                state->timer = 0;
            }
            break;
        case 1:
            height = 0x1000;
            spin = ((state->timer / 2 + 1) << 12) / 6;
            if (state->timer >= 12) {
                state->timer = 0;
                state->stage = 2;
            }
            break;
        default:
            height = 0x1000;
            break;
        }
        scale.z = 0x44C;
        scale.x = 0x44C;
        scale.y = height / 4;
        angles.x = 0;
        angles.y = state->angle;
        angles.z = 0;
        func_800CF3AC(D_800E1E24, &color, D_800E27EC);
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        D_800F3368.tpage = D_800E2850[D_800E11E4[9]];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        gte_ldrotmatrix((const GteMatrixWords *)D_800BCFA4.value);
        gte_ldtransmatrix((const GteMatrixWords *)D_800BCFA4.value);
        {
            int tpage = (u16)(D_800E2850[D_800E11E4[9]] | GetTPage(0, 3, 0, 0));
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            FieldFx_ModelSetTexture(tpage, GetClut(0, (kind == 4 && D_800F3428) ? palette + 10 : palette + 6));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(D_800F3418);
        FieldFx_ModelSetColors(D_800F3418, color.r, color.g, color.b);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_800F3418, &matrix);
        size = 0x4B0;
        scale.z = size;
        scale.x = size;
        angles.x = 0x800;
        angles.y = state->angle;
        angles.z = 0;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        position.y -= 1000;
        RotMatrixYXZ(&angles, &matrix);
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        FieldFx_ModelDraw((union FieldTexturedModel *)D_800F3418, &matrix);
        FieldFx_ModelRestoreColors(D_800F3418);
        if (spin) {
            scale.z = size;
            scale.x = size;
            angles.x = 0;
            angles.y = state->angle + spin;
            angles.z = 0;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            RotMatrixYXZ(&angles, &matrix);
            ScaleMatrix(&matrix, &scale);
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            {
                int tpage = (u16)(D_800E2850[D_800E11E4[9]] | GetTPage(0, 1, 0, 0));
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                FieldFx_ModelSetTexture(tpage, GetClut(0, (kind == 4 && D_800F3428) ? palette + 10 : palette + 6));
            }
            FieldFx_ModelSetSemiTrans(1);
            FieldFx_ModelSaveColors(D_800F342C);
            FieldFx_ModelSetColors(D_800F342C, 255, 255, 255);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_800F342C, &matrix);
            FieldFx_ModelRestoreColors(D_800F342C);
        }
        break;
    }
    return 0;
}

int func_800DC5BC(int mode, GteShortVector *state)
{
    RenderColor color;
    GteRotation rotation;
    int scale, palette;
    int selector;
    switch (mode) {
    case 1:
        state->x += (rand() & 7) - 3;
        state->z += (rand() & 7) - 3;
        state->y -= (rand() & 3) + 2;
        if (D_800E27EC >= 14) return 1;
        break;
    case 2:
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        rotation.flags = 0;
        func_800CF3AC(D_800E1E64, &color, D_800E27EC);
        scale = rsin((D_800E27EC << 11) / 14);
        selector = D_800F3368.palette;
        palette = D_800E1204[selector];
        func_800CEE20(state, &rotation, scale, scale, 226,
                      GetClut(0, (selector == 4 && D_800F3428) ? palette + 6
                                                              : palette + 2),
                      1, 128, &color);
        break;
    }
    return 0;
}

int func_800DC750(int mode, GteShortVector *state)
{
    GteShortVector *particle;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 0, &state->x);
        return func_800CE560(D_800F33E0->end, 8, 20,
                            (FieldAnimTaskCallback)func_800DC5BC);
    case 1:
        if (D_800E27EC < 40) {
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
        D_800F3368.tpage = D_800E2850[D_800E11F6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 16;
        break;
    }
    return 0;
}

int FieldFx_DriftingEffect(int mode, GteShortVector *position) {
    RenderColor color;
    GteRotation rotation;
    int scale;
    int palette;
    switch (mode) {
    case 1:
        position->x += (rand() & 7) - 3;
        position->z += (rand() & 7) - 3;
        position->y -= (rand() & 3) + 2;
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = D_800E27EC * 24;
        func_800CF3AC(D_800E1EE8, &color, D_800E27EC);
        scale = (D_800E27EC << 7) + 8192;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(position, &rotation, scale, scale, 130,
                      GetClut(48, palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800DCA80(int mode, GteShortVector *state)
{
    GteShortVector *particle;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 1, &state->x);
        return func_800CE560(D_800F33E0->end, 8, 20,
                            (FieldAnimTaskCallback)FieldFx_DriftingEffect);
    case 1:
        if (D_800E27EC < 33 && (D_800E27EC & 1)) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->x = state->x + rand() % 800 - 400;
                particle->y = state->y + rand() % 400 - 700;
                particle->z = state->z + rand() % 800 - 400;
            }
        }
        if (D_800E27EC >= 60) return 1;
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

int func_800DCCCC(int mode, RenderTiltingSprite *state)
{
    RenderColor color;
    int u, v, palette;
    switch (mode) {
    case 1:
        state->rotation.y += state->velocity_y;
        state->rotation.z += 8;
        state->rotation.x = state->initial_tilt - state->initial_tilt * D_800E27EC / 128;
        state->height = 114;
        state->width = rcos(D_800E27EC * 16) / 4 + 200;
        if (D_800E27EC >= 64) return 1;
        break;
    case 2:
        func_800CF3AC(D_800E1F18, &color, D_800E27EC);
        u = ((D_800E27EC / 2) & 3) * 64;
        v = (((D_800E27EC / 2) & 7) / 4) * 16 + 160;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428) palette += 4;
        func_800D2370(&D_800E222C, &state->rotation, state->width, state->height,
                     u, v, 64, 16, GetClut(176, palette), 0, &color, 128, 1);
        break;
    }
    return 0;
}

int func_800DCE94(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    RenderColor color;
    RenderTiltingSprite *particle;
    int scale;
    int draw_mode;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 0, &state->position.x);
        return func_800CE560(D_800F33E0->end, 16, 16,
                            (FieldAnimTaskCallback)func_800DCCCC);
    case 1:
        if (D_800E27EC < 17) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->initial_tilt = (rand() & 511) - 256;
                particle->rotation.y = state->phase;
                particle->rotation.z = rand();
                particle->velocity_y = (rand() & 31) - 16;
                state->phase += 2218 + (rand() & 31);
            }
        }
        if (D_800E27EC >= 73) return 1;
        break;
    case 2:
        if (D_800E27EC < 65) {
            scale = (D_800E27EC << 6) + 2048;
            D_800F3368.depth = 60;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            func_800CF3AC(D_800E1F18, &color, D_800E27EC);
            draw_mode = 1;
            if (D_800E27EC & 1) draw_mode = 3;
            func_800D0728(&position, 200, 300, 16, 0, scale, scale, &color, 0, 128, draw_mode);
            scale = rsin(D_800E27EC * 16) / 2 + 2048;
            func_800D004C(&position, 700, 700, 16, 0, scale, scale, &color, 0, 128, 1);
            func_800D0728(&position, 600, 800, 24, 0, scale, scale, 0, &color, 128, 3);
        }
        D_800E222C.x = state->position.x;
        D_800E222C.y = state->position.y;
        D_800E222C.z = state->position.z;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.depth = 8;
        break;
    }
    return 0;
}

int func_800DD19C(int mode, RenderSparkEmitter *state)
{
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    int scale;
    switch (mode) {
    case 0:
        state->phase = rand();
        func_800CE870((char *)D_8009D254, 1, &state->position.x);
        state->position.y -= 510;
        return 0;
    case 1:
        if (D_800E27EC >= 64) return 1;
        break;
    case 2:
        D_800F3368.depth = 60;
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        scale = 4096 + (D_800E27EC << 5);
        func_800CF3AC(D_800E1FA4, &color, D_800E27EC);
        func_800D004C(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
        func_800D0728(&position, 410, 500, 20, 0, 4096, 4096, 0, &color, 128, 3);
        func_800CF3AC(D_800E1FCC, &color, D_800E27EC);
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = D_800E27EC * 16;
        func_800D004C(&position, 400, 100, 24, &rotation, scale, scale, &color, 0, 128, 1);
        break;
    }
    return 0;
}

int func_800DD380(int mode, RenderConvergingSprite *state)
{
    RenderColor color __attribute__((aligned(4)));
    GteRotation rotation;
    GteShortVector position;
    int scale, weight, palette;
    switch (mode) {
    case 1:
        state->timer++;
        if (state->stage >= 2) return 1;
        break;
    case 2:
        scale = 0;
        switch (state->stage) {
        case 0:
            weight = (state->timer * 4096) / 52;
            LoadAverageShort12(&state->position, &D_800E2234, 4096-weight, weight, &position);
            position.x += rsin(state->phase + D_800E27EC*160) / 16;
            state->position.y -= 2;
            func_800CF3AC(D_800E1FEC, &color, (D_800E27EC << 6) / 52);
            scale = rsin(D_800E27EC*128) / 2 + 4096;
            if (state->timer >= 52) {
                state->stage = 1;
                state->timer = 0;
                state->bursts = 0;
            }
            break;
        case 1:
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            scale = rsin((D_800E27EC << 11) / 12);
            weight = scale / 32;
            /* The target writes all four packed color bytes in one word. */
            *(unsigned *)&color = (unsigned)weight | ((unsigned)weight << 8) | ((rand() & 15) << 16);
            if (state->timer >= 12) {
                state->bursts++;
                if (state->bursts < 4) {
                    state->position.x = D_800E2234.x;
                    state->position.y = D_800E2234.y;
                    state->position.z = D_800E2234.z;
                    state->position.x += rand() % 400 - 200;
                    state->position.y += rand() % 400 - 200;
                    state->position.z += rand() % 400 - 200;
                    state->timer = 0;
                } else state->stage = 2;
            }
            break;
        }
        rotation.x = 0;
        rotation.y = 0;
        rotation.flags = 0;
        rotation.z = D_800E27EC * 12;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428) palette += 4;
        func_800CEE20(&position, &rotation, scale, scale, 128, GetClut(32,palette), 1, 128, &color);
        break;
    }
    return 0;
}

int func_800DD76C(int mode, RenderConvergingEmitter *state)
{
    RenderConvergingSprite *particle;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 1, &state->position.x);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.y -= 550;
        return func_800CE560(D_800F33E0->end, 16, 24, (FieldAnimTaskCallback)func_800DD380);
    case 1:
        if (D_800E27EC < 49 && (D_800E27EC & 1)) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->position.x = state->position.x + rand() % 700 - 350;
                particle->position.y = state->position.y;
                particle->position.z = state->position.z + rand() % 700 - 350;
                particle->stage = 0;
                particle->timer = 0;
                particle->phase = rand();
            }
        }
        if (D_800E27EC >= 140) return 1;
        break;
    case 2:
        D_800E2234.x = state->target.x;
        D_800E2234.y = state->target.y;
        D_800E2234.z = state->target.z;
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

int func_800DD9E4(int mode, RenderSettlingSprite *state)
{
    RenderColor color;
    GteShortVector position;
    int scale;
    int blend;
    int palette;

    switch (mode) {
    case 1:
        state->timer++;
        if (state->stage >= 2)
            return 1;
        break;
    case 2:
        scale = 0x1000;
        switch (state->stage) {
        case 0:
            blend = 0x1000 - rcos((state->timer << 10) / 28);
            LoadAverageShort12(state, &D_800E223C, 0x1000 - blend, blend,
                               state);
            state->position.y += rsin(state->phase) / 128;
            state->phase += 160;
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            func_800CF3AC(D_800E207C, &color, (D_800E27EC << 6) / 28);
            scale = 0x2000 - (state->timer << 12) / 28;
            if (state->timer >= 28) {
                state->stage = 1;
                state->timer = 0;
                state->position.x += (rand() & 255) - 128;
                state->position.y += (rand() & 255) - 128;
                state->position.z += (rand() & 255) - 128;
            }
            break;
        case 1:
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            blend = rcos((state->timer << 10) / 12) / 32;
            *(u32 *)&color = blend | (blend << 8);
            state->position.x += (rand() & 7) - 3;
            state->position.y += (rand() & 7) - 3;
            state->position.z += (rand() & 7) - 3;
            if (state->timer >= 8)
                state->stage = 2;
            break;
        }
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        func_800D1DEC(&position, &color, 128, 1);
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, 0, scale, scale, 128, GetClut(32, palette), 3,
                      128, &color);
        break;
    }
    return 0;
}

int func_800DDD70(int mode, GteShortVector *state)
{
    GteShortVector position;
    RenderColor color;
    RenderSettlingSprite *particle;
    int scale;
    int draw_mode;
    switch (mode) {
    case 0:
        func_800CE870((char *)D_8009D254, 1, &state->x);
        return func_800CE560(D_800F33E0->end, 16, 32,
                            (FieldAnimTaskCallback)func_800DD9E4);
    case 1:
        if (D_800E27EC < 67) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->position.x = state->x + (rand() & 511) - 256;
                particle->position.y = state->y - rand() % 600;
                particle->position.z = state->z + (rand() & 511) - 256;
                particle->stage = 0;
                particle->timer = 0;
                particle->phase = rand();
            }
        }
        if (D_800E27EC >= 86) return 1;
        break;
    case 2:
        position.x = D_8009D254->renderObject.matrices[19].translation[0];
        position.y = D_8009D254->renderObject.matrices[19].translation[1];
        position.z = D_8009D254->renderObject.matrices[19].translation[2];
        D_800E223C.x = position.x;
        D_800E223C.y = position.y;
        D_800E223C.z = position.z;
        if (D_800E27EC < 87) {
            scale = (D_800E27EC << 12) / 86 + 2048;
            D_800F3368.depth = 16;
            func_800CF3AC(D_800E20AC, &color, D_800E27EC);
            draw_mode = 1;
            if (D_800E27EC & 1) draw_mode = 3;
            func_800D004C(&position, 500, 500, 16, 0, scale, scale, &color, 0, 128, 1);
            color.b = 0;
            func_800D0728(&position, 400, 300, 20, 0, scale, scale, &color, 0, 64, draw_mode);
        }
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

/* Mode 0 anchors at actor point 19; mode 1 runs 25 frames; mode 2 flashes
 * the screen, swells the model for 16 frames and draws the fan, band and
 * quad while the burst lasts. */
int func_800DE0A8(int mode, FieldFlashBurst *state)
{
    GteVector scale;
    RenderColor color;
    GteMatrix matrix;
    u16 *slots;
    u16 *tpages;
    int intensity;
    int size;

    switch (mode) {
    case 0:
        func_800CE870((char *)g_PlayerEntity, 1, state->point);
        state->anchor.x = g_PlayerEntity->render_object.matrices[19].translation[0];
        state->anchor.y = g_PlayerEntity->render_object.matrices[19].translation[1];
        state->anchor.z = g_PlayerEntity->render_object.matrices[19].translation[2];
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F3474, 0, 0);
        break;
    case 1:
        if (D_800E27EC >= 25)
            return 1;
        break;
    case 2:
        if (D_800E27EC == 0) {
            *(u32 *)&color = 0xA0A0A0;
            func_800D1AE0(&color.r, 0x50, 2, 8);
        }
        if (D_800E27EC >= 1 && D_800E27EC < 6) {
            *(u32 *)&color = 0xA0FFFF;
            func_800D1AE0(&color.r, 0x80 - (D_800E27EC - 1) * 32, 1, 8);
        }
        if (D_800E27EC < 17) {
            matrix = *(GteMatrix *)&g_PlayerEntity->render_object.matrices[0];
            scale.x = scale.y = rcos(D_800E27EC << 6) / 3;
            scale.z = rsin(D_800E27EC << 6);
            ScaleMatrix(&matrix, &scale);
            matrix.t[0] = g_PlayerEntity->render_object.matrices[19].translation[0];
            matrix.t[1] = g_PlayerEntity->render_object.matrices[19].translation[1];
            matrix.t[2] = g_PlayerEntity->render_object.matrices[19].translation[2];
            func_800CF3AC(D_800E20CC, &color, D_800E27EC);
            slots = &D_800E11E6;
            D_800F3368.tpage = D_800E2850[slots[0]];
            D_800F3368.palette = 1;
            func_800CEDA8(1);
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
                FieldFx_ModelSetTexture(tpage, GetClut(0xD0, palette));
            }
            FieldFx_ModelSetSemiTrans(1);
            FieldFx_ModelSaveColors(D_800F3474);
            FieldFx_ModelSetColors(D_800F3474, color.r, color.g, color.b);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_800F3474, &matrix);
            scale.x = scale.y = 0x1800;
            scale.z = 0x1000;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSetColors(D_800F3474, color.r, color.g >> 1, 0);
            {
                int tpage = (u16)(D_800E2850[D_800E11E6] | GetTPage(0, 1, 0, 0));
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428)
                    palette += 4;
                FieldFx_ModelSetTexture(tpage, GetClut(0xD0, palette));
            }
            FieldFx_ModelDraw((union FieldTexturedModel *)D_800F3474, &matrix);
            FieldFx_ModelRestoreColors(D_800F3474);
        }
        if (D_800E27EC < 25) {
            D_800F3368.depth = 16;
            func_800CF3AC(D_800E20CC, &color, (D_800E27EC << 4) / 24);
            func_800D004C((GteShortVector *)state, 600, 600, 12, 0, 0x1000, 0x1000,
                          &color, 0, 0x80, 1);
            if (D_800E27EC < 17) {
                color.b = 0;
                size = rsin(D_800E27EC << 6) + 0x800;
                func_800CF3AC(D_800E20CC, &color, D_800E27EC);
                func_800D0728((GteShortVector *)state, 500, 600, 20, 0, size, size,
                              &color, 0, 0x40, 1);
            }
            intensity = rcos((D_800E27EC << 10) / 24) / 32;
            *(u32 *)&color = 0xC8B4B4;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11F6];
            D_800F3368.palette = 1;
            func_800CEDA8(1);
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                func_800CEE20((GteShortVector *)state, 0, 0x2000, 0x2000, 0x42,
                              GetClut(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3),
                              1, intensity, &color);
            }
        }
        break;
    }
    return 0;
}

/* Mode 1 moves the glow with damped horizontal speed and gravity for 32
 * ticks; mode 2 draws a pulsing spinning sprite with a fainter halo. */
int func_800DE7A8(int mode, FieldSpinGlow *state)
{
    GteShortVector position;
    GteRotation rotation;
    int scale;
    int intensity;
    int palette;

    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        state->vy = state->vy + 1;
        state->vx = state->vx * 31 / 32;
        state->vz = state->vz * 31 / 32;
        if (D_800E27EC >= 32)
            return 1;
        break;
    case 2:
        intensity = state->intensity - state->intensity * D_800E27EC / 32;
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = state->angle + D_800E27EC * 8;
        scale = rsin(D_800E27EC * 32) + 0x2800;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale,
                      (s16)D_800F3368.parameter02 * (D_800E27EC / 4) + 0xC0, GetClut(64, palette), 1,
                      intensity, 0);
        scale = 0x1800;
        palette = D_800E1204[D_800F3368.palette];
        if (D_800F3368.palette == 4 && D_800F3428)
            palette += 4;
        func_800CEE20(&position, &rotation, scale, scale,
                      (s16)D_800F3368.parameter02 * (D_800E27EC / 8) + 0x60, GetClut(0, palette), 3,
                      intensity, 0);
        break;
    }
    return 0;
}

/* Mode 0 anchors at the owner actor and opens a pool of 32 spin glows;
 * mode 1 throws two glows per frame for 16 frames and ends at frame 48;
 * mode 2 draws the rising shape quad, the opening ring and the band. */
int func_800DEA30(int mode, FieldGlowFountain *state)
{
    GteRotation rotation;
    GteShortVector vector;
    RenderColor black = D_800C22BC;
    FieldSpinGlow *glow;
    int intensity;
    int scale;
    int kind;
    int palette;
    int i;

    switch (mode) {
    case 0:
        state->seed = rand();
        state->x = D_800F32D0->actor->render_object.target_x;
        state->y = D_800F32D0->actor->render_object.target_y;
        state->z = D_800F32D0->actor->render_object.target_z;
        return func_800CE560(D_800F33E0->end, 16, 32,
                             (FieldEffectCallback)func_800DE7A8);
    case 1:
        if (D_800E27EC < 16) {
            glow = func_800CE610(D_800F33E0->end);
            if (glow) {
                for (i = 0; i < 2; i++) {
                    vector.x = rand() % 48 - 24;
                    vector.z = -(rand() % 80);
                    vector.y = rand() % 48 - 32;
                    FieldFx_RotateVector(
                        (GteMatrixWords *)g_PlayerEntity->render_object.matrices,
                        &vector, &vector);
                    glow->vx = vector.x;
                    glow->vy = vector.y;
                    glow->vz = vector.z;
                    glow->x = state->x + (rand() & 0xFF) - 0x80;
                    glow->y = state->y + (rand() & 0xFF) - 0x80;
                    glow->z = state->z + (rand() & 0xFF) - 0x80;
                    glow->intensity = (rand() & 0x3F) + 0x50;
                    glow->angle = rand();
                    state->seed += 0x955;
                }
            }
        }
        if (D_800E27EC >= 48)
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
        D_800F3368.parameter0A = 5;
        D_800F3368.parameter06 = 1;
        D_800F3368.depth = 0x40;
        if (D_800E27EC < 25) {
            state->y -= 6;
            intensity = rcos((D_800E27EC << 10) / 24) * 240 / 4096;
            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            rotation.z = D_800E27EC << 4;
            scale = 0x3000 - rcos((D_800E27EC << 10) / 12);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20((GteShortVector *)state, &rotation, scale, scale, 0x40,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, intensity, 0);
            if (D_800E27EC < 9) {
                intensity = rcos(D_800E27EC << 7) / 32;
                func_800D004C((GteShortVector *)state, 100, 700, 16, &rotation,
                              scale, scale, &black, 0, intensity, 1);
            }
        }
        D_800F3368.depth = 0;
        if (D_800E27EC < 17) {
            intensity = 0x80 - (D_800E27EC - 4) * 8;
            vector.x = state->x;
            vector.y = state->y;
            vector.z = state->z;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC << 7;
            scale = rsin((D_800E27EC - 4) << 6) * 2;
            func_800D0728(&vector, 600, 700, 0x18, &rotation, scale, scale, 0,
                          &black, intensity, 1);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11E6];
        D_800F3368.palette = 1;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x18;
        break;
    }
    return 0;
}

/* Phase 0 follows actor matrix point `kind` and every fourth frame sheds a
 * falling spark (phase 2, one in three) or a drifting one (phase 1);
 * phases 1 and 2 drift for 16 and 24 frames. Mode 2 draws each phase as a
 * shape quad capped at the emitter's intensity D_800E2244. */
int func_800DEFFC(int mode, RenderFadeParticle *particle)
{
    GteShortVector offset = D_800C22F0;
    RenderColor color;
    RenderFadeParticle *child;
    int scale;
    int intensity;
    int kind;
    int palette;

    switch (mode) {
    case 1:
        switch (particle->phase) {
        case 0:
            particle->timer++;
            FieldFx_TransformMatrixPoint((struct RoomFxTransformOwner *)g_PlayerEntity,
                                          particle->kind, &offset,
                                          (GteShortVector *)particle);
            if (D_800E27EC & 3)
                return 0;
            if (rand() % 3 == 0) {
                child = func_800CE610(D_800F33E0->end);
                if (child == 0)
                    break;
                child->position[0] = particle->position[0];
                child->position[1] = particle->position[1];
                child->position[2] = particle->position[2];
                child->vx = (rand() & 7) - 3;
                child->vz = (rand() & 7) - 3;
                child->vy = (rand() & 0xF) - 12;
                child->phase = 2;
                child->timer = 0;
            } else {
                child = func_800CE610(D_800F33E0->end);
                if (child == 0)
                    return 0;
                child->position[0] = particle->position[0];
                child->position[1] = particle->position[1];
                child->position[2] = particle->position[2];
                child->position[0] += (rand() & 0x7F) - 0x40;
                child->position[1] += (rand() & 0x7F) - 0x40;
                child->position[2] += (rand() & 0x7F) - 0x40;
                child->vx = (rand() & 0xF) - 7;
                child->vz = (rand() & 0xF) - 7;
                child->vy = (rand() & 7) - 11;
                child->phase = 1;
                child->timer = 0;
            }
            break;
        case 1:
            particle->timer++;
            particle->position[0] += particle->vx;
            particle->position[1] += particle->vy;
            particle->position[2] += particle->vz;
            particle->position[0] += (rand() & 7) - 3;
            particle->position[1] += (rand() & 7) - 3;
            particle->position[2] += (rand() & 7) - 3;
            if (particle->timer < 16)
                break;
            return 1;
        case 2:
            particle->timer++;
            particle->position[0] += particle->vx;
            particle->position[1] += particle->vy;
            particle->position[2] += particle->vz;
            if (particle->timer < 24)
                break;
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        switch (particle->phase) {
        case 0:
            scale = 0x1800;
            if (particle->timer < 33)
                scale = rsin(particle->timer << 5) + 0x800;
            if (particle->timer < 25) {
                intensity = rsin((particle->timer << 10) / 24) / 32;
            } else if (particle->timer & 1) {
                intensity = 0x80;
            } else {
                intensity = 0x64;
            }
            if (D_800E2244 < intensity)
                intensity = D_800E2244;
            func_800CEE20((GteShortVector *)particle, 0, scale / 2, scale / 2,
                          (s16)D_800F3368.parameter02 * (particle->timer & 3) + 0x68,
                          GetClut(16, (D_800F3368.palette == 4 && D_800F3428)
                                      ? D_800E1204[D_800F3368.palette] + 4
                                      : D_800E1204[D_800F3368.palette]),
                          1, intensity, 0);
            break;
        case 1:
            intensity = particle->timer << 6;
            scale = rcos(intensity) / 2 + 0x800;
            intensity = rcos(intensity) / 32;
            if (D_800E2244 < intensity)
                intensity = D_800E2244;
            func_800CEE20((GteShortVector *)particle, 0, scale, scale,
                          (s16)D_800F3368.parameter02 * (particle->timer & 3) + 0x68,
                          GetClut(16, (D_800F3368.palette == 4 && D_800F3428)
                                      ? D_800E1204[D_800F3368.palette] + 4
                                      : D_800E1204[D_800F3368.palette]),
                          1, intensity / 2, 0);
            break;
        case 2:
            scale = rsin((particle->timer << 10) / 24) + 0x1000;
            func_800CF3AC(D_800E213C, &color, particle->timer);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428)
                palette += 4;
            func_800CEE20((GteShortVector *)particle, 0, scale / 2, scale,
                          (s16)D_800F3368.parameter02 * (s16)(particle->timer / 6) + 0x60,
                          GetClut(0, palette), 1, 0x80, &color);
            break;
        }
        break;
    }
    return 0;
}

#include "pe1/render_object.h"

int func_800DF6AC(int mode, RenderFadeEmitter *state)
{
    RenderFadeParticle *particle;
    u16 intensity, page;
    switch (mode) {
    case 0:
        state->count = 0;
        state->intensity = 160;
        return func_800CE560(D_800F33E0->end, 20, 32,
                             (FieldAnimCallbackListCallback)func_800DEFFC);
    case 1:
        if ((D_800E27EC & 3) == 0 && state->count < 8) {
            particle = func_800CE610(D_800F33E0->end);
            if (particle) {
                particle->kind = D_800E2164[state->count];
                particle->phase = 0;
                particle->timer = 0;
            }
            state->count++;
        }
        if (D_800E27EC >= 32)
            state->intensity -= 2;
        if (state->intensity <= 0) {
            state->intensity = 0;
            return 1;
        }
        break;
    case 2:
        intensity = state->intensity;
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        page = D_800E2850[D_800E11E6];
        D_800E2244 = intensity;
        D_800F3368.palette = 1;
        D_800F3368.tpage = page;
        func_800CEDA8(1);
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 24;
        break;
    }
    return 0;
}

int FieldFx_RotatingEffect(int mode) {
    GteShortVector position;
    GteRotation rotation;
    u32 color;
    int scale;

    switch (mode) {
    case 0:
        break;
    case 1:
        if (D_800E27EC >= 24)
            return 1;
        break;
    case 2:
        D_800F3368.depth = 0;
        func_800CE870((char *)D_8009D254, 1, &position.x);
        position.y -= 300;
        func_800CF3AC(D_800E1494, &color, D_800E27EC);
        rotation.x = 1024;
        rotation.y = 0;
        rotation.z = D_800E27EC << 8;
        rotation.flags = 1;
        scale = rsin((D_800E27EC << 10) / 24);
        func_800D0728(&position, 300, 400, 10, &rotation,
                      scale, scale, 0, (RenderColor *)&color, 128, 1);
        break;
    }
    return 0;
}

int FieldFx_PointEmitter(int mode, FieldAnimEmitter *state) {
    FieldAnimEmittedPoint *point;
    int angle;

    switch (mode) {
    case 0:
        state->angle = rand();
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_800F32D8, 0, 0);
        return func_800CE560(D_800F33E0->end, 16, 24,
                             (FieldAnimCallbackListCallback)func_800D9A8C);
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
        func_800CE870((char *)D_8009D254, 1, D_800E220C);
        break;
    }
    return 0;
}
