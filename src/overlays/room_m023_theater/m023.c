#include "common.h"
#include "pe1/gte.h"
#include "pe1/room_drift_pulse.h"
#include "pe1/room_m023_effects.h"
#include "pe1/room_m023_beacon.h"

/* Drift/pulse sprite for room_m023. The glow strip's anchor is taken into
 * a local pointer first: its address load is what keeps the frame counter
 * load after the palette lookup like retail. */

int func_8018F004(int mode, RoomDriftPulseState *state) {
    GteShortVector origin;
    GteRotation rotation;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        switch (state->phase) {
        case 0:
            state->x += state->vx;
            state->y += state->vy;
            state->z += state->vz;
            if (D_800E27EC >= 4) {
                return 1;
            }
        case 1:
            state->x += state->vx;
            state->y += state->vy;
            state->z += state->vz;
            if (D_800E27EC < 8) {
                break;
            }
            return 1;
        default:
            return 0;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (state->phase) {
        case 0:
            {
                GteShortVector *anchor = &RoomM023_ScriptAnchor;
                int kind;
                int palette;
                int fade;
                u16 clut;

                *(int *)&color = 0x808080;
                origin.x = state->x;
                origin.y = state->y;
                origin.z = state->z;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                fade = (D_800E27EC - 1) << 4;
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = GetClut(0, palette);
                func_800D2370(anchor, (GteRotation *)state, 0x3C0, 0x64, 0, fade,
                              0xFF, 0xF, clut, 0, &color, 0xBE, 1);
            }
            rotation.x = state->x;
            rotation.y = state->y;
            rotation.z = state->z;
            rotation.flags = 1;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                u16 clut;

                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = GetClut(0x20, palette);
                func_800CEE20(&RoomM023_ScriptAnchor, &rotation, 0x1000, 0x1000,
                              ((s16)D_800F3368.parameter02 << 1) * (D_800E27EC - 1) + 0x60, clut,
                              3, 0x80, 0);
            }
            break;
        case 1:
            rotation.x = 0;
            rotation.y = 0;
            rotation.flags = 0;
            rotation.z = D_800E27EC << 6;
            {
                int scale = rcos((D_800E27EC - 1) << 7) / 32;
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                u16 clut;

                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = GetClut(0x10, palette);
                func_800CEE20((GteShortVector *)state, &rotation, 0x1000, 0x1000,
                              (s16)D_800F3368.parameter02 * D_800E27EC + 0x40, clut, 1, scale, 0);
            }
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

/* Every `interval` frames emits a particle with a random velocity, either
 * from the tracked model joint or from a random point of a wide band. */
int func_8018F3C8(int mode, RoomM023ScatterState *state) {
    RoomM023Template template = D_8018EFF4;
    s16 position[4];
    RoomM023Particle *particle;

    switch (mode) {
    case 0:
        state->unused08 = 0;
        state->unused0A = 0;
        state->timer = 0;
        switch (ANIM_TASK_AS(RoomM023EventState)->variant) {
        case 0:
            state->joint = 8;
            break;
        case 1:
            state->joint = 16;
            break;
        }
        return func_800CE560(ANIM_POOL_AS(RoomM023Channel)->pool, 16, 12,
                             (FieldAnimCallbackListCallback)func_8018F004);
    case 1:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM023Channel)->pool, state->joint, &template, position);
        state->timer++;
        if (ANIM_TASK_AS(RoomM023EventState)->interval == 0) return 1;
        if (ANIM_TASK_AS(RoomM023EventState)->interval < state->timer) {
            state->timer = 0;
            func_800CE870(ANIM_OWNER_AS(RoomM023Channel)->pool, 0, position);
            particle = func_800CE610(ANIM_POOL_AS(RoomM023Channel)->pool);
            if (particle == 0) return 0;
            if (func_80071A54() & 1) {
                particle->x = (func_80071A54() & 0x7FF) - 0x400;
                particle->y = func_80071A54();
                particle->z = 0;
                particle->attached = 0;
            } else {
                particle->x = position[0];
                particle->y = position[1];
                particle->z = position[2];
                particle->attached = 1;
            }
            particle->vx = func_80071A54() % 70 - 35;
            particle->vy = func_80071A54() % 70 - 35;
            particle->vz = func_80071A54() % 70 - 35;
        }
        break;
    case 2:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM023Channel)->pool, state->joint, &template, &RoomM023_ScriptAnchor);
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 12;
        break;
    }
    return 0;
}

int func_8018F710(int mode, RoomM023JointGlow *glow) {
    GteShortVector position;
    GteRotation rotation;
    RoomM023Template template = D_8018EFF4;
    RenderColor ring = D_8018EFFC;
    RenderColor flare = D_8018F000;
    int size;
    int intensity;
    s16 frame;
    int kind;
    int palette;

    switch (mode) {
    case 0:
        switch (ANIM_TASK_AS(RoomM023EventState)->variant) {
        case 0:
            glow->joint = 8;
            func_800D3F64(0x5AE, func_800D3FD8());
            func_800D3F64(0x5AF, 0x80);
            break;
        case 1:
            glow->joint = 16;
            break;
        }
        glow->state = 0;
        glow->frame = 0;
        return 0;
    case 1:
        glow->frame++;
        if (glow->state >= 3) return 1;
        break;
    case 2:
        size = 0;
        intensity = 0;
        switch (glow->state) {
        case 0:
            frame = glow->frame;
            func_800D0728(&position, 0x258, 0x320, 0x20, 0, frame, frame, &ring, 0,
                          rsin(frame << 7) / 32, 1);
            size = glow->frame << 8;
            intensity = 0x50;
            if (glow->frame >= 8) {
                glow->state = 1;
                glow->frame = 0;
            }
            break;
        case 1:
            frame = glow->frame;
            size = frame * 64 + 0x800;
            intensity = frame * 48 / 32 + 0x50;
            if (frame >= 0x20) {
                glow->state = 2;
                glow->frame = 0;
            }
            break;
        case 2:
            intensity = 0xA0;
            size = rcos(glow->frame << 7) / 2 + 0x1000;
            if (glow->frame >= 8) {
                glow->state = 3;
                glow->frame = 0;
            }
            break;
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x18;
        if (D_800E27EC & 1) {
            size = size * 5 / 4;
        }
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = -D_800E27EC << 6;
        rotation.flags = 0;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM023Channel)->pool, glow->joint, &template, &position);
        {
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800CEE20(&position, &rotation, size * 3 / 2, size * 3 / 2, 0x80,
                          GetClut(0x30, palette), 1, intensity / ((D_800E27EC & 1) + 1), 0);
        }
        {
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800CEE20(&position, &rotation, size * 3 / 2, size * 3 / 2, 0x84,
                          GetClut(0x40, palette), 1, intensity, 0);
        }
        func_800D004C(&position, 0x258, 0x258, 0xC, 0, size, size, &flare, 0, 0x40, 1);
        break;
    }
    return 0;
}

/* Joint beacon: rides a model joint, swells in, pulses, flickers and
 * collapses, tagging the player once on contact; draws a glow sprite pair
 * with two star fans and a loaded model scaled by the beacon size. */
int func_8018FC14(int mode, RoomM023Beacon *state) {
    GteVector scale;
    GteShortVector position;
    GteShortVector rotation;
    RoomM023Template offset = D_8018EFF4;
    GteMatrix matrix;
    GteMatrix spin;
    RoomM023BeaconColor color;
    int size;
    int page;

    switch (mode) {
    case 0:
        switch (ANIM_TASK_AS(RoomM023BeaconEvent)->variant) {
        case 0:
            state->joint = 8;
            break;
        case 1:
            state->joint = 0x10;
            break;
        }
        state->phase = 0;
        state->timer = 0;
        state->height = 0;
        state->glow = 0;
        if (ANIM_TASK_AS(RoomM023BeaconEvent)->active) {
            RoomM023BeaconPool *pool = ANIM_OWNER_AS(RoomM023BeaconChannel)->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        state->armed = 1;
        D_80190760 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5463704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_80190760, 0, 0);
        return 0;
    case 1:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM023BeaconChannel)->pool, state->joint, &offset, state);
        func_800CE9D4((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomM023BeaconChannel)->pool, 0,
                      &state->rotation);
        switch (state->phase) {
        case 0:
            state->timer++;
            state->width = 0x400;
            state->height += 0x19A;
            state->glow = 0x1000;
            state->shade = state->timer * 8 / 5;
            if (state->timer >= 5) {
                state->phase = 1;
                state->timer = 0;
            }
            break;
        case 1:
            state->timer++;
            state->width = rcos(state->timer << 7) + 0x1000;
            state->glow = rcos(state->timer << 7) + 0x1000;
            state->shade = state->timer + 8;
            if (state->timer >= 8) {
                state->phase = 2;
                state->timer = 0;
            }
            break;
        case 2:
            state->timer++;
            state->glow = 0x1000;
            state->shade = 0x10;
            state->width = ((D_800E27EC & 1) << 8) + 0x1000;
            if (state->timer >= 0x20) {
                state->phase = 3;
                state->timer = 0;
            }
            break;
        case 3:
            state->glow = 0x1000;
            state->timer++;
            state->width = 0x1000 - (state->timer << 8);
            state->shade = state->timer + 0x10;
            if (state->timer >= 0x10) return 1;
            break;
        }
        func_800CFB7C(&state->rotation, state->height, &position);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.x += position.x;
        state->target.y += position.y;
        state->target.z += position.z;
        if (func_800CEB8C(state, &state->target, 0x50) == 0) return 0;
        if (state->armed == 0) return 0;
        if (ANIM_TASK_AS(RoomM023BeaconEvent)->active) {
            RoomM023BeaconChannel *channel = ANIM_OWNER_AS(RoomM023BeaconChannel);
            if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {
                ((RoomM023BeaconEntity *)D_8009D254)->actor->flags |= 0x4000;
                channel->pool->node->flags =
                    (channel->pool->node->flags & 0xC0FFFFFF) | 0x11000000;
                channel->pool->node->flags |= 0x80000000;
            }
        }
        state->armed = 0;
        break;
    case 2:
        D_800F3368.depth = 0x18;
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        if (state->glow != 0) {
            rotation.x = 0;
            rotation.y = 0;
            rotation.pad = 0;
            rotation.z = D_800E27EC * 0x60;
            size = state->glow;
            size = size * 2 / 3;
            if (D_800E27EC & 1) size = size * 9 / 8;
            func_800CF3AC(D_801906F0, &color, state->shade);
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&state->position, (GteRotation *)&rotation, size, size, 0x80,
                              GetClut(0x30, palette), 1, 0x80, (struct RenderColor *)&color);
            }
            func_800D004C(&state->position, 0x190, 0x190, 0xC, 0, size, size, (struct RenderColor *)&color, 0, 0x40, 1);
            func_800D004C(&state->position, 0xBE, 0xBE, 8, 0, size, size, (struct RenderColor *)&color, 0, 0x80, 1);
            position.x = state->position.x;
            position.y = state->position.y;
            position.z = state->position.z;
            position.y = g_RoomFloorY->y;
            rotation.x = 0x400;
            rotation.pad = 1;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&position, (GteRotation *)&rotation, size, size, 0x80,
                              GetClut(0x30, palette), 1, 0x40, (struct RenderColor *)&color);
            }
        }
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        RotMatrix(&rotation, &spin);
        matrix = *ANIM_OWNER_AS(RoomM023BeaconChannel)->pool->matrix;
        MulMatrix0(&matrix, &spin, &matrix);
        scale.x = scale.y = state->width / 16;
        scale.z = (state->height << 12) / 8000;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        {
            RenderMatrixSlot *slot;
            u16 *index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            slot = &D_800BCFA4;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
            page = (u16)(D_800E2850[*index] | GetTPage(0, 1, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x50, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(D_80190760);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80190760, &matrix);
        FieldFx_ModelRestoreColors(D_80190760);
        FieldFx_ModelSaveColors(D_80190760);
        scale.z = 0x1000;
        scale.y = 0x1800;
        scale.x = 0x1800;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelShiftColors(D_80190760, -0xFF, -0x8C, -0xFF);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80190760, &matrix);
        FieldFx_ModelRestoreColors(D_80190760);
        FieldFx_ModelSaveColors(D_80190760);
        scale.z = 0x1000;
        scale.y = 0x800;
        scale.x = 0x800;
        matrix.t[1] = g_RoomFloorY->y;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelScaleColors(D_80190760, 0x19);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80190760, &matrix);
        FieldFx_ModelRestoreColors(D_80190760);
        break;
    }
    return 0;
}
