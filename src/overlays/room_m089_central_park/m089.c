#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_m089_trail.h"
#include "room_m089.h"
#include "pe1/room_sound_slot.h"
#include "pe1/room_m089_spin_model.h"
#include "pe1/room_swirl_rise.h"
#include "pe1/field_model_draw.h"
#include "pe1/room_m089_model_pulse.h"

/* Spiral trail: the history shift is written out field by field (retail
 * loads all fifteen halfwords before the first store), and the flash angle
 * is `timer * 128`, which keeps the signed lh that `timer << 7` loses. */

int func_801924F8(int mode, RoomM089SpiralTrail *trail, GteShortVector *anchor) {
    GteRotation rotation;
    RenderColor head = D_8018F1CC;
    RenderColor tail = D_8018F1D0;
    RenderMatrixSlot *matrixSlot;
    int radius;
    int intensity;
    int kind;
    int palette;

    switch (mode) {
    case 1:
        switch (trail->state) {
        case 0:
            trail->timer++;
            trail->points[5].x = trail->points[4].x;
            trail->points[5].y = trail->points[4].y;
            trail->points[5].z = trail->points[4].z;
            trail->points[4].x = trail->points[3].x;
            trail->points[4].y = trail->points[3].y;
            trail->points[4].z = trail->points[3].z;
            trail->points[3].x = trail->points[2].x;
            trail->points[3].y = trail->points[2].y;
            trail->points[3].z = trail->points[2].z;
            trail->points[2].x = trail->points[1].x;
            trail->points[2].y = trail->points[1].y;
            trail->points[2].z = trail->points[1].z;
            trail->points[1].x = trail->points[0].x;
            trail->points[1].y = trail->points[0].y;
            trail->points[1].z = trail->points[0].z;
            trail->points[0].x = anchor->x;
            trail->points[0].y = anchor->y;
            trail->points[0].z = anchor->z;
            trail->points[0].y -= trail->rise - trail->rise * trail->timer / 16;
            radius = trail->points[0].pad - trail->points[0].pad * trail->timer / 16;
            trail->points[0].x += rcos(trail->angle) * radius / 4096;
            trail->points[0].z += rsin(trail->angle) * radius / 4096;
            trail->angle += trail->points[1].pad;
            if (trail->timer >= 16) {
                trail->state = 1;
                trail->timer = 0;
            }
            break;
        case 1:
            trail->timer++;
            trail->points[0].x = anchor->x;
            trail->points[0].y = anchor->y;
            trail->points[0].z = anchor->z;
            if (trail->timer >= 8) return 1;
            break;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (trail->state) {
        case 0:
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            if (trail->points[5].x | trail->points[5].y | trail->points[5].z)
                func_800D2B58(&trail->points[4], &trail->points[5], &head, &tail, 0x19, 0, 1);
            if (trail->points[4].x | trail->points[4].y | trail->points[4].z)
                func_800D2B58(&trail->points[3], &trail->points[4], &head, &tail, 0x32, 0x19, 1);
            if (trail->points[3].x | trail->points[3].y | trail->points[3].z)
                func_800D2B58(&trail->points[2], &trail->points[3], &head, &tail, 0x4B, 0x32, 1);
            if (trail->points[2].x | trail->points[2].y | trail->points[2].z)
                func_800D2B58(&trail->points[1], &trail->points[2], &head, &tail, 0x64, 0x4B, 1);
            if (trail->points[1].x | trail->points[1].y | trail->points[1].z)
                func_800D2B58(&trail->points[0], &trail->points[1], &head, &tail, 0x80, 0x64, 1);
            break;
        case 1:
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = trail->timer * 128;
            rotation.flags = 1;
            intensity = rcos(trail->timer << 7) / 32;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800CEE20(&trail->points[0], &rotation, 0x1000, 0x1000, 6,
                          (u16)GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, intensity, &head);
            break;
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

int func_80192A30(int mode, unsigned short *dst, unsigned short *src) {
    short *effect;
    RoomChanCtx *channel;
    int random;

    switch (mode) {
    case 0:
    channel = ANIM_POOL_AS(RoomChanCtx);
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
    return func_800CE560((char *)channel->w8, 0x38, 8,
                         (FieldAnimCallbackListCallback)func_801924F8);

    case 1:
    if (D_800E27EC < 9) {
        effect = func_800CE610((char *)ANIM_POOL_AS(RoomChanCtx)->w8);
        if (effect != 0) {
            effect[2] = 0;
            effect[1] = 0;
            effect[0] = 0;
            effect[6] = 0;
            effect[5] = 0;
            effect[4] = 0;
            effect[10] = 0;
            effect[9] = 0;
            effect[8] = 0;
            effect[14] = 0;
            effect[13] = 0;
            effect[12] = 0;
            effect[18] = 0;
            effect[17] = 0;
            effect[16] = 0;
            effect[22] = 0;
            effect[21] = 0;
            effect[20] = 0;
            effect[25] = 0;
            effect[24] = 0;
            effect[26] = func_80071A54();
            effect[3] = (func_80071A54() & 0x3FF) + 0x400;
            if (func_80071A54() & 1) {
                effect[7] = (func_80071A54() & 0x7F) + 0x1AC;
            } else {
                effect[7] = -0x1AC - (func_80071A54() & 0x7F);
            }
            effect[27] = (func_80071A54() & 0x3FF) + 0x5DC;
        }
    }

    if (D_800E27EC == 9) {
        RoomSoundSlot *sound = (RoomSoundSlot *)D_800B0E64;
        int volume;
        if (sound->channel != 0) {
            volume = 0x7F;
            random = func_800D3FD8();
            Akao_SendTableCommand(sound->channel, 0x5BB, random, 0x80, volume);
            if (sound->channel != 0) {
                Akao_SendTableCommand(sound->channel, 0x5BC, 0x80, 0x80, volume);
            }
        }
    }
    if (D_800E27EC < 0x24) {
        break;
    }
    return 1;

    case 2:
    {
        int value;
        int index;
        index = D_800E11FA;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        value = D_800E2850[index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        D_800F3368.tpage = value;
    }
        break;
    default:
        break;
    }
    return 0;
}

/* Spinning model: loads its model, swells a glow over the anchor for two
 * stages and draws the model twice, the inner copy at half scale. The tile
 * index and tpage table go through local pointers (retail keeps both in
 * saved registers, which spills the model intensity to the frame), and the
 * palette kind/palette pair is block-local per draw. */

int func_80192CD4(int mode, RoomM089SpinModel *spin, GteShortVector *anchor) {
    GteShortVector position;
    GteShortVector rotation;
    RenderColor color = D_8018F1D4;
    GteMatrix matrix;
    GteVector scale;
    GteMatrix innerMatrix;
    GteVector innerScale;
    int intensity;
    int size;
    int glow;
    int turn;
    int page;
    u16 *index;
    u16 *tpages;

    switch (mode) {
    case 0:
        spin->stage = 0;
        spin->timer = 0;
        D_80194168 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5864704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_80194168, 0, 0);
        return 0;
    case 1:
        spin->timer++;
        if (spin->stage < 2) break;
        return 1;
    case 2:
        size = 0;
        glow = 0;
        turn = 0;
        intensity = 0;
        switch (spin->stage) {
        case 0:
            turn = rcos(spin->timer << 6);
            intensity = rsin(spin->timer << 7) / 32;
            glow = 0x40;
            size = rsin(spin->timer << 6);
            if (spin->timer >= 16) {
                spin->stage = 1;
                spin->timer = 0;
            }
            break;
        case 1:
            glow = rcos(spin->timer << 6) / 32 * 3 / 2;
            size = 0x1000;
            if (spin->timer >= 16) {
                spin->stage = 2;
                spin->timer = 0;
            }
            break;
        }
        index = &D_800E11FA;
        tpages = D_800E2850;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[*index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 2;
        position.x = anchor->x;
        position.y = anchor->y;
        position.z = anchor->z;
        if (size != 0) {
            rotation.x = 0x400;
            rotation.y = 0;
            rotation.z = -D_800E27EC << 6;
            rotation.pad = 1;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                func_800CEE20(&position, (GteRotation *)&rotation, size, size, 0x26,
                              GetClut(0, (kind == 4 && D_800F3428) ? palette + 9 : palette + 5),
                              1, glow, &color);
            }
        }
        rotation.x = 0;
        rotation.z = 0;
        position.y -= (0x20 - D_800E27EC) << 4;
        turn = turn * 2 / 3;
        rotation.y = -(D_800E27EC << 6);
        D_800F3368.tpage = tpages[*index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && D_800F3428) ? palette + 7 : palette + 3));
        }
        FieldFx_ModelSetSemiTrans(1);
        RotMatrixYXZ(&rotation, &matrix);
        matrix.t[0] = position.x;
        matrix.t[1] = position.y;
        matrix.t[2] = position.z;
        scale.x = turn;
        scale.y = turn;
        scale.z = turn;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelSaveColors(D_80194168);
        FieldFx_ModelScaleColors(D_80194168, intensity);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80194168, &matrix);
        FieldFx_ModelRestoreColors(D_80194168);
        position.y += (0x20 - D_800E27EC) << 3;
        RotMatrixYXZ(&rotation, &innerMatrix);
        innerMatrix.t[0] = position.x;
        innerMatrix.t[1] = position.y;
        innerMatrix.t[2] = position.z;
        innerScale.x = turn / 2;
        innerScale.y = turn / 2;
        innerScale.z = turn / 2;
        ScaleMatrix(&innerMatrix, &innerScale);
        FieldFx_ModelSaveColors(D_80194168);
        FieldFx_ModelScaleColors(D_80194168, intensity);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80194168, &innerMatrix);
        FieldFx_ModelRestoreColors(D_80194168);
        break;
    }
    return 0;
}

/* Swirl/rise sprite for room_m089. Both draws pass their intensity through
 * one function-scope variable (two sets keep each computation where retail
 * has it), and the second draw takes the state as its rotation through a
 * local pointer, whose copy keeps the frame counter load below the palette
 * lookup. */

int RoomM089_SwirlRiseSprite(int mode, RoomSwirlRiseState *state,
                                        GteShortVector *anchor) {
    GteShortVector origin;
    GteRotation rotation;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int intensity;

    color = D_8018F1D8;
    switch (mode) {
    case 1:
        switch (state->phase) {
        case 0: {
            int radius;

            state->x = anchor->x;
            state->y = anchor->y;
            state->z = anchor->z;
            radius = state->radius * D_800E27EC / 64;
            state->x += rcos(state->angle) * radius / 4096;
            state->z += rsin(state->angle) * radius / 4096;
            state->y -= rsin(D_800E27EC << 5) * 120 / 4096;
            state->angle += 12;
            if (D_800E27EC >= 64) {
                return 1;
            }
            break;
        }
        case 1:
            state->x += 0x20;
            if (D_800E27EC >= 6) {
                return 1;
            }
            break;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (state->phase) {
        case 0: {
            int scale;
            int kind;
            int palette;
            int clut;

            rotation.x = 0;
            rotation.y = 0;
            rotation.z = state->angle + D_800E27EC * 12 + state->radius;
            rotation.flags = 0;
            intensity = 0x80 - D_800E27EC * 2;
            scale = rsin(D_800E27EC << 4) + 0x800;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = (u16)GetClut(0x20, palette);
            func_800CEE20((GteShortVector *)state, &rotation, scale, scale,
                          (s16)D_800F3368.parameter02 * (D_800E27EC / 16) + 0x24, clut, 1,
                          intensity, 0);
            return 0;
        }
        case 1: {
            int v;
            GteRotation *spin;
            int kind;
            int palette;
            u16 clut;

            origin.x = anchor->x;
            origin.y = anchor->y;
            origin.z = anchor->z;
            intensity = rcos((D_800E27EC << 10) / 6) / 32;
            *(int *)&color = 0x808080;
            spin = (GteRotation *)state;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            v = ((D_800E27EC & 1) << 4) + 0x80;
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x10, palette);
            func_800D2370(&origin, spin, 0x3C0, 0x8C, 0, v, 0xFF,
                          0xF, clut, &color, 0, (s16)intensity, 1);
            break;
        }
        default:
            return 0;
        }
        break;
    default:
        return 0;
    }
    return 0;
}

/* Pulsing model burst: spawns a ring of swirl sprites on the first frame and
 * rising sprites every other frame, tags the player while it is close, and
 * draws three turning layers of one model, a fourth model and a glow strip
 * that fade out over the first 0x31 frames. */
int func_80193618(int mode, RoomM089ModelPulse *state, GteShortVector *anchor) {
    GteShortVector rotation;
    RenderColor color;
    GteMatrix matrix;
    GteVector scale;
    GteMatrix wideMatrix;
    GteVector wideScale;
    GteMatrix lowMatrix;
    GteVector lowScale;
    GteMatrix coreMatrix;
    RoomSwirlRiseState *child;
    RoomM089PulseChannel *channel;
    int fade;
    int outer;
    int tall;
    int glow;
    int wide;
    int low;
    int page;
    int i;

    switch (mode) {
    case 0:
        state->phase = 0;
        state->timer = 0;
        state->position.x = anchor->x;
        state->position.y = anchor->y;
        state->position.z = anchor->z;
        if (ANIM_TASK_AS(RoomM089PulseEvent)->active) {
            RoomM089PulsePool *pool = ANIM_OWNER_AS(RoomM089PulseChannel)->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        D_8019416C = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5464704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_8019416C, 0, 0);
        D_80194168 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5864704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_80194168, 0, 0);
        return func_800CE560(ANIM_POOL_AS(RoomM089SpawnChannel)->pool, 0x10, 0x18,
                             (FieldAnimCallbackListCallback)func_80193200);
    case 1:
        if (D_800E27EC == 1) {
            for (i = 0; i < 16; i++) {
                child = func_800CE610(ANIM_POOL_AS(RoomM089SpawnChannel)->pool);
                if (child) {
                    child->phase = 0;
                    child->angle = (func_80071A54() & 0x7F) + (i << 8);
                    child->radius = (func_80071A54() & 0xFF) + 0x1F4;
                }
            }
        }
        if (D_800E27EC < 0x38 && (D_800E27EC & 1)) {
            child = func_800CE610(ANIM_POOL_AS(RoomM089SpawnChannel)->pool);
            if (child) {
                child->phase = 1;
                child->x = (func_80071A54() & 0x7F) + 0x200;
                child->y = func_80071A54();
                child->z = 0;
            }
        }
        if (D_800E27EC >= 0x40) return 1;
        if (func_800C6B90(state, 0xF0) == 0) break;
        if (D_800E27EC >= 0x28) break;
        if (ANIM_TASK_AS(RoomM089PulseEvent)->active) {
            channel = ANIM_OWNER_AS(RoomM089PulseChannel);
            if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {
                ((RoomM089PulseEntity *)D_8009D254)->actor->flags |= 0x4000;
                channel->pool->node->flags =
                    (channel->pool->node->flags & 0xC0FFFFFF) | 0x29000000;
                channel->pool->node->flags |= 0x80000000;
            }
        }
        break;
    case 2:
        fade = rcos(D_800E27EC << 5) / 32;
        outer = rcos(D_800E27EC << 5) + 0x400;
        tall = rsin(D_800E27EC << 5) + 0x400;
        glow = rcos(D_800E27EC << 5) / 32;
        wide = rsin(D_800E27EC << 5) + 0x800;
        low = 0x800 - rsin(D_800E27EC << 5) / 4;
        D_800F3368.depth = 0x10;
        outer /= 4;
        wide /= 3;
        if (D_800E27EC < 0x21) {
            u16 *index;
            rotation.x = 0;
            rotation.y = -(D_800E27EC << 5);
            rotation.z = 0;
            index = &D_800E11FA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 1, 0, 0));
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && D_800F3428) ? palette + 6
                                                                     : palette + 2));
            }
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &matrix);
            matrix.t[0] = state->position.x;
            matrix.t[1] = state->position.y;
            matrix.t[2] = state->position.z;
            scale.x = outer;
            scale.y = tall;
            scale.z = outer;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(D_8019416C);
            FieldFx_ModelScaleColors(D_8019416C, fade);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_8019416C, &matrix);
            FieldFx_ModelRestoreColors(D_8019416C);
            rotation.x = 0;
            rotation.z = 0;
            rotation.y = D_800E27EC << 5;
            RotMatrixYXZ(&rotation, &wideMatrix);
            wideMatrix.t[0] = state->position.x;
            wideMatrix.t[1] = state->position.y;
            wideMatrix.t[2] = state->position.z;
            wideScale.x = wide;
            wideScale.y = low;
            wideScale.z = wide;
            ScaleMatrix(&wideMatrix, &wideScale);
            FieldFx_ModelSaveColors(D_8019416C);
            FieldFx_ModelScaleColors(D_8019416C, glow);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_8019416C, &wideMatrix);
            FieldFx_ModelRestoreColors(D_8019416C);
            rotation.x = 0;
            rotation.z = 0;
            rotation.y = D_800E27EC << 4;
            RotMatrixYXZ(&rotation, &lowMatrix);
            lowMatrix.t[0] = state->position.x;
            lowMatrix.t[1] = state->position.y;
            lowMatrix.t[2] = state->position.z;
            lowScale.x = outer;
            lowScale.y = low * 3 / 2;
            lowScale.z = outer;
            ScaleMatrix(&lowMatrix, &lowScale);
            FieldFx_ModelSaveColors(D_8019416C);
            FieldFx_ModelScaleColors(D_8019416C, glow);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_8019416C, &lowMatrix);
            FieldFx_ModelRestoreColors(D_8019416C);
        }
        if (D_800E27EC < 0x19) {
            u16 *index;
            rotation.x = 0;
            rotation.y = D_800E27EC << 6;
            rotation.z = 0;
            fade = rcos((D_800E27EC << 10) / 24) / 32;
            outer = rsin((D_800E27EC << 10) / 24) * 3 / 2;
            index = &D_800E11FA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 3, 0, 0));
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && D_800F3428) ? palette + 7
                                                                     : palette + 3));
            }
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&rotation, &coreMatrix);
            coreMatrix.t[0] = state->position.x;
            coreMatrix.t[1] = state->position.y;
            coreMatrix.t[2] = state->position.z;
            lowScale.x = outer;
            lowScale.y = outer;
            lowScale.z = outer;
            ScaleMatrix(&coreMatrix, &lowScale);
            FieldFx_ModelSaveColors(D_80194168);
            FieldFx_ModelScaleColors(D_80194168, fade);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_80194168, &coreMatrix);
            FieldFx_ModelRestoreColors(D_80194168);
        }
        if (D_800E27EC < 0x31) {
            func_800CF3AC(D_801940F8, &color, D_800E27EC);
            func_800D004C(&state->position, 0x1F4, 0x1F4, 0xC, 0, 0x1000, 0x1000, &color, 0, 0x80, 1);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        break;
    }
    return 0;
}

short *func_80193FF0(int arg0, short arg1, short arg2, short arg3) {
    if (arg0 == 1) {
        D_80194160 = arg1;
        D_80194162 = arg2;
        D_80194164 = arg3;
    }

    return &D_80194160;
}
