/* MASPSX_FLAGS: --expand-div */
#include "common.h"
#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
#include "pe1/gte.h"
#include "pe1/random.h"
#include "pe1/field_anim.h"
#include "pe1/battle_runtime.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_flash_burst.h"
#include "pe1/field_spin_glow.h"
#include "pe1/field_glow_fountain.h"

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
        func_800C6D5C((union FieldTexturedModel *)D_800F3474, 0, 0);
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
                GsSetOrign(tpage, GetClut(0xD0, palette));
            }
            func_800C6ED8(1);
            func_800C6EF8(D_800F3474);
            func_800C7098(D_800F3474, color.r, color.g, color.b);
            func_800C71E4((union FieldTexturedModel *)D_800F3474, &matrix);
            scale.x = scale.y = 0x1800;
            scale.z = 0x1000;
            ScaleMatrix(&matrix, &scale);
            func_800C7098(D_800F3474, color.r, color.g >> 1, 0);
            {
                int tpage = (u16)(D_800E2850[D_800E11E6] | GetTPage(0, 1, 0, 0));
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428)
                    palette += 4;
                GsSetOrign(tpage, GetClut(0xD0, palette));
            }
            func_800C71E4((union FieldTexturedModel *)D_800F3474, &matrix);
            func_800C6F4C(D_800F3474);
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
                    FieldEng_RotateVector(
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
