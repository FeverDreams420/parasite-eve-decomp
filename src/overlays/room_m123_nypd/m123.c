#include "common.h"
#include "pe1/akao_send.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_transform_point.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/fieldfx_calls.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/room_floor.h"
#include "pe1/room_m123.h"
#include "pe1/room_m123_beacon.h"
#include "pe1/room_m123_glow_burst.h"
#include "pe1/room_m123_joint_triangle.h"
#include "pe1/room_sound_slot.h"

#include "m123_externs.h"
#include "room_m123_effects.h"

typedef struct RoomPulseObject RoomPulseObject;

typedef struct RoomPulsePool {
    RoomPulseObject *object;
} RoomPulsePool;

struct RoomPulseObject {
    u32 flags;
    u8 pad[0x14];
    u8 *status;
};

typedef struct RoomPulseGlobal {
    u8 pad[0x4C];
    u32 flags;
} RoomPulseGlobal;

typedef struct RoomM123Burst {
    u16 frame;
    u16 phase;
    u16 x, y, z;
    u16 pad;
} RoomM123Burst;

int func_8019251C(int mode, RoomM123JointTriangle *triangle) {
    GteShortVector corners[4];
    GteShortVector offset = D_8018F1CC;
    GteShortVector tip = D_8018F1D4;
    RenderColor color = D_8018F1DC;
    int weight;
    int width;
    int position;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        switch (triangle->state) {
        case 0:
            triangle->frame++;
            if (triangle->frame >= 24) return 1;
            break;
        case 1:
            triangle->frame++;
            triangle->position = (s16)(triangle->position + 0x200) % 0x3000;
            if (triangle->frame >= 24) return 1;
            break;
        default:
            return 0;
        }
        break;
    case 2:
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x28, &offset, &corners[0]);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x22, &offset, &corners[1]);
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x1D, &tip, &corners[2]);
        switch (triangle->state) {
        case 0:
            FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x17, &tip, &corners[3]);
            weight = 0x5000 - rsin((triangle->frame << 10) / 24) * 4;
            LoadAverageShort12(&corners[3], &corners[0], 0x1000 - weight, weight, &corners[0]);
            LoadAverageShort12(&corners[3], &corners[1], 0x1000 - weight, weight, &corners[1]);
            LoadAverageShort12(&corners[3], &corners[2], 0x1000 - weight, weight, &corners[2]);
            width = (triangle->frame << 7) / 24;
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            func_800D2B58(&corners[0], &corners[1], &color, &color, width, width, 1);
            func_800D2B58(&corners[1], &corners[2], &color, &color, width, width, 1);
            func_800D2B58(&corners[2], &corners[0], &color, &color, width, width, 1);
            return 0;
        case 1:
            position = triangle->position;
            if (position < 0x1000) {
                weight = position;
                LoadAverageShort12(&corners[0], &corners[1], 0x1000 - weight, weight, &corners[0]);
            } else if (position < 0x2000) {
                weight = position - 0x1000;
                LoadAverageShort12(&corners[1], &corners[2], 0x1000 - weight, weight, &corners[0]);
            } else {
                weight = position - 0x2000;
                LoadAverageShort12(&corners[2], &corners[0], 0x1000 - weight, weight, &corners[0]);
            }
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            width = rsin(triangle->frame << 8) / 32;
            func_800D1DEC(&corners[0], &color, width, 1);
            break;
        default:
            return 0;
        }
        break;
    }
    return 0;
}

int func_801929FC(int mode) {
    int frame, index;
    RoomM123JointTriangleEmitterView *position;

    switch (mode) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM123Pool)->pool, 8, 16,
                             (FieldFxCallbackListCallback)func_8019251C);
    case 1:
        frame = D_800E27EC;
        if (frame == (frame / 6) * 6) {
            if (frame >= 40) return 2;
            for (index = 0; index < 3; index++) {
                position = func_800CE610(ANIM_POOL_AS(RoomM123Pool)->pool);
                if (position != 0) {
                    position->x = 1;
                    position->y = 0;
                    position->z = index << 12;
                }
            }
            if (D_800E27EC < 25) {
                position = func_800CE610(ANIM_POOL_AS(RoomM123Pool)->pool);
                if (position != 0) {
                    position->x = 0;
                    position->y = 0;
                    position->z = 0;
                }
            }
        }
        if (D_800E27EC >= 40) return 2;
        break;
    case 2: {
        int paletteIndex = D_800E11EA;
        int palette;
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        palette = D_800E2850[paletteIndex];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 12;
        D_800F3368.tpage = palette;
        break;
    }
    }
    return 0;
}

int func_80192BDC(int mode, RoomM123PulsingParticle *particle) {
    GteShortVector position;
    GteShortVector output;
    GteShortVector color;
    int scale;
    int palette;
    int kind;
    int blend;
    u16 clut;
    RenderMatrixSlot *matrixSlot;

    switch (mode) {
    case 1:
        particle->frame++;
        particle->offset += 16;
        if ((s16)particle->frame >= 48) return 1;
        break;
    case 2:
        scale = particle->scale *
                rcos(((s16)particle->frame << 10) / 48) / 4096;
        func_800CF844(&D_80195690, &position, scale / 2, &D_80195688,
                      scale, (s16)particle->offset);
        if ((s16)particle->frame < 16) {
            func_800CF3AC(D_801954BC, &color, 0x30 - (scale * 48) / 1024);
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            clut = GetClut(0x20, palette);
            func_800CEE20(&position, 0, 0x1000, 0x1000,
                          (s16)D_800F3368.parameter02 * 2 + 0xD8, clut, 3, g_RoomScriptRecord,
                          (RenderColor *)&color);
            matrixSlot = &D_800BCFA4;
            gte_ldrotmatrix(matrixSlot->value);
            gte_ldtransmatrix(matrixSlot->value);
            func_800D1DEC(&position, &color, g_RoomScriptRecord, 1);
            /* Retail re-tests the frame here; the blended tail only runs
             * from frame 16 on. */
            if ((s16)particle->frame < 16) break;
        }
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        func_800CF3AC(D_801954BC, &color, 0x30 - (scale * 48) / 1024);
        blend = rcos(((s16)particle->frame - 16) << 5);
        LoadAverageShort12(&position, &D_80195690, blend, 0x1000 - blend, &output);
        func_800D2B58(&output, &position, &color, 0, g_RoomScriptRecord, 0, 1);
        break;
    }
    return 0;
}

/* Glow burst: spawns sixteen pulsing particles at frame 1, then draws four
 * glow layers, two flash rings and a swept flare on joint 23. The parameter
 * block fields written twice share a base register; the tile indices are
 * read as D_800E11E4 elements so their loads stay below the base-register
 * stores, and the palette read-back follows the block's stores. */

int func_80192F0C(int mode, RoomM123GlowBurst *burst) {
    GteShortVector template = D_8018F1E0;
    GteShortVector position;
    GteRotation rotation;
    RenderColor color;
    RoomM123BurstParticle *child;
    int i;
    int intensity;
    int step;
    int fade;

    switch (mode) {
    case 0:
        burst->timer = 0;
        return func_800CE560(ANIM_POOL_AS(RoomM123Pool)->pool, 8, 0x10,
                             (FieldFxCallbackListCallback)func_80192BDC);
    case 1:
        if (burst->timer < 0x80) burst->timer += 4;
        if (D_800E27EC == 1) {
            for (i = 0; i < 16; i++) {
                child = func_800CE610(ANIM_POOL_AS(RoomM123Pool)->pool);
                if (child) {
                    child->frame = 0;
                    child->scale = (rand() & 0x1FF) + 0x200;
                    child->offset = rand();
                }
            }
            func_800D3F64(0x580, func_800D3FD8());
            func_800D3F64(0x5BD, 0x80);
        }
        if (D_800E27EC >= 33) return 1;
        break;
    case 2:
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x32;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 0x17, &template, &position);
        rotation.x = 0;
        rotation.y = 0;
        rotation.z = -D_800E27EC << 6;
        rotation.flags = 0;
        intensity = burst->timer;
        if (D_800E27EC != 0) intensity = intensity * 2 / 3;
        rcos((D_800E27EC << 10) / 40);
        {
            int kind;
            int palette;
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            func_800D3BC8(&position, 0x1000, 0x1000, 0x50,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2),
                          1, intensity, 0, 0);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800D3BC8(&position, 0x1000, 0x1000, 0x4D, GetClut(0x50, palette),
                          1, intensity, 0, 0xA00);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800D3BC8(&position, 0x1000, 0x1000, 0x48, GetClut(0x30, palette),
                          1, intensity, 0, 0xC00);
        }
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            func_800D3BC8(&position, 0x1000, 0x1000, 0x49, GetClut(0x40, palette),
                          1, intensity, 0, 0xE00);
        }
        func_800CF3AC(D_801954BC, &color, D_800E27EC / 2);
        func_800D004C(&position, 700, 700, 0x10, 0, 0x1000, 0x1000, &color, 0, 0x40, 1);
        func_800CF3AC(D_801954E4, &color, D_800E27EC * 48 / 40);
        func_800D004C(&position, 0xA0, 0xA0, 8, 0, 0x1000, 0x1000, &color, 0, 0x80, 1);
        g_RoomScriptRecord = burst->timer;
        D_80195690.x = position.x;
        D_80195690.y = position.y;
        D_80195690.z = position.z;
        func_800CE9D4(ANIM_OWNER_AS(RoomM123Pool)->pool, 0, &D_80195688.vector);
        step = D_800E27EC - 16;
        if ((unsigned int)step < 17) {
            fade = rcos(step << 6);
            func_800CF3AC(D_801954E4, &color, step * 3);
            D_80195688.rotation.flags = 1;
            func_800D0728(&position, 700, 1000, 0x18, &D_80195688.rotation, fade, fade, 0, &color,
                          0x80, 1);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_800F3368.tpage = D_800E2850[D_800E11E8];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.depth = 0xC;
        break;
    }
    return 0;
}

/* Joint beacon: rides joint 0x17, swells in, pulses and fades out, tagging
 * the player once on contact; draws a glow sprite pair with two star fans,
 * a fading three-sprite ring on the ground, a turning model while the effect
 * starts and three scaled passes of a second model. */
int func_80193618(int mode, RoomM123Beacon *state) {
    GteVector scale;
    GteShortVector position;
    GteShortVector rotation;
    RoomM123BeaconOffset offset = D_8018F1E8;
    GteMatrix matrix;
    GteMatrix spin;
    RoomM123BeaconColor color;
    int size;
    int page;

    switch (mode) {
    case 0:
        state->fade = 200;
        state->phase = 0;
        state->timer = 0;
        state->glow = 0;
        state->width = 0;
        state->depth = 0x1000;
        if (FX_TASK_AS(RoomM123BeaconEvent)->active) {
            RoomM123BeaconPool *pool = ANIM_OWNER_AS(RoomM123BeaconChannel)->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        state->armed = 1;
        D_80195698 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5483704);
        D_8019569C = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5883704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_80195698, 0, 0);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_8019569C, 0, 0xF0);
        return 0;
    case 1:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123BeaconChannel)->pool, 0x17, &offset, state);
        func_800CE9D4((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomM123BeaconChannel)->pool, 0,
                      &state->rotation);
        state->alpha = 0x80;
        switch (state->phase) {
        case 0:
            state->timer++;
            state->height = rcos(state->timer << 7) / 2 + 0x1000;
            if (state->width < 0x800) state->width += 0x100;
            state->depth -= 0x80;
            state->glow = rcos(state->timer << 7) + 0x1000;
            state->shade = state->timer;
            if (state->timer >= 8) {
                state->phase = 1;
                state->timer = 0;
            }
            break;
        case 1:
            state->timer++;
            state->height = 0x1000;
            state->glow = ((D_800E27EC & 1) << 9) + 0x1000;
            if (state->shade < 0x20) state->shade++;
            if (state->timer >= 0x20) {
                state->phase = 2;
                state->timer = 0;
            }
            break;
        case 2:
            state->height = 0x1000;
            state->width = 0x800;
            state->glow = 0x1000;
            state->timer++;
            state->shade++;
            state->alpha = 0x80 - state->timer * 8;
            if (state->timer >= 0x10) return 1;
            break;
        }
        func_800CFB7C(&state->rotation, 0x7D0, &position);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.x += position.x;
        state->target.y += position.y;
        state->target.z += position.z;
        if (func_800CEB8C(state, &state->target, 0x140) == 0) return 0;
        if (state->armed == 0) return 0;
        if (FX_TASK_AS(RoomM123BeaconEvent)->active) {
            RoomM123BeaconChannel *channel = ANIM_OWNER_AS(RoomM123BeaconChannel);
            if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {
                ((RoomM123BeaconEntity *)D_8009D254)->actor->flags |= 0x4000;
                channel->pool->node->flags =
                    (channel->pool->node->flags & 0xC0FFFFFF) | 0x11000000;
                channel->pool->node->flags |= 0x80000000;
            }
        }
        state->armed = 0;
        break;
    case 2:
        D_800F3368.depth = 0x32;
        D_800F3368.parameter00 = 0x40;
        D_800F3368.parameter02 = 4;
        D_800F3368.extent_x = 0x40;
        D_800F3368.extent_y = 0x40;
        D_800F3368.tpage = D_800E2850[D_800E11FA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        if (state->glow != 0) {
            rotation.x = 0;
            rotation.y = 0;
            rotation.pad = 0;
            rotation.z = D_800E27EC << 6;
            size = state->glow;
            func_800CF3AC(D_8019550C, &color, state->shade);
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                func_800CEE20(&state->position, (GteRotation *)&rotation, size * 3 / 2, size * 3 / 2, 0x50,
                              GetClut(0, (kind == 4 && D_800F3428) ? palette + 6
                                                                  : palette + 2),
                              1, 0xA0, (struct RenderColor *)&color);
            }
            func_800D004C(&state->position, 0x2BC, 0x2BC, 0xC, 0, size, size, (struct RenderColor *)&color, 0, 0x50, 1);
            func_800D004C(&state->position, 0xF0, 0xF0, 8, 0, size, size, (struct RenderColor *)&color, 0, 0x80, 1);
            if (state->fade > 0) {
                state->fade -= 8;
                if (state->fade > 0) {
                    position.x = state->position.x;
                    position.y = state->position.y;
                    position.z = state->position.z;
                    D_800F3368.parameter00 = 0x20;
                    D_800F3368.parameter02 = 2;
                    D_800F3368.extent_x = 0x20;
                    D_800F3368.extent_y = 0x20;
                    D_800F3368.tpage = D_800E2850[D_800E11E4[3]];
                    D_800F3368.palette = 3;
                    D_800F3368.parameter06 = 0;
                    {
                        int kind = D_800F3368.palette;
                        int palette = D_800E1204[kind];
                        if (kind == 4 && D_800F3428) palette += 4;
                        func_800D3BC8(&position, 0x1000, 0x1000, 0x4D, GetClut(0x50, palette), 1,
                                      state->fade, 0, 0xA00);
                    }
                    D_800F3368.parameter00 = 0x10;
                    D_800F3368.parameter02 = 1;
                    D_800F3368.extent_x = 0x10;
                    D_800F3368.extent_y = 0x10;
                    {
                        int kind = D_800F3368.palette;
                        int palette = D_800E1204[kind];
                        if (kind == 4 && D_800F3428) palette += 4;
                        func_800D3BC8(&position, 0x1000, 0x1000, 0x48, GetClut(0x30, palette), 1,
                                      state->fade, 0, 0xC00);
                    }
                    D_800F3368.parameter00 = 0x40;
                    D_800F3368.parameter02 = 4;
                    D_800F3368.extent_x = 0x40;
                    D_800F3368.extent_y = 0x40;
                    {
                        int kind = D_800F3368.palette;
                        int palette = D_800E1204[kind];
                        if (kind == 4 && D_800F3428) palette += 4;
                        func_800D3BC8(&position, 0x1000, 0x1000, 0x49, GetClut(0x40, palette), 1,
                                      state->fade, 0, 0xE00);
                    }
                }
            }
        }
        {
            u16 *index = &D_800E11EA;
            u16 *tpages = D_800E2850;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            if (D_800E27EC < 0x11) {
                RenderMatrixSlot *slot;
                rotation.x = 0x800;
                rotation.y = 0;
                rotation.z = D_800E27EC << 4;
                RotMatrixYXZ(&rotation, &spin);
                matrix = *ANIM_OWNER_AS(RoomM123BeaconChannel)->pool->matrix;
                MulMatrix0(&matrix, &spin, &matrix);
                scale.x = scale.y = rsin(D_800E27EC << 6) / 4 + 0x400;
                scale.z = 0x1000;
                ScaleMatrix(&matrix, &scale);
                matrix.t[0] = state->position.x;
                matrix.t[1] = state->position.y;
                matrix.t[2] = state->position.z;
                slot = &D_800BCFA4;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
                D_800F3368.tpage = tpages[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
                {
                    int kind = D_800F3368.palette;
                    int palette = D_800E1204[kind];
                    if (kind == 4 && D_800F3428) palette += 4;
                    FieldFx_ModelSetTexture(page, GetClut(0x60, palette));
                }
                FieldFx_ModelSetSemiTrans(1);
                FieldFx_ModelSaveColors(D_8019569C);
                FieldFx_ModelSetColors(D_8019569C, color.r, color.g, color.b);
                size = rcos(D_800E27EC << 6) / 64;
                FieldFx_ModelScaleColors(D_8019569C, size);
                FieldFx_ModelDraw((union FieldTexturedModel *)D_8019569C, &matrix);
                FieldFx_ModelRestoreColors(D_8019569C);
            }
        }
        rotation.x = 0x800;
        rotation.y = 0;
        rotation.z = D_800E27EC << 7;
        RotMatrixYXZ(&rotation, &spin);
        matrix = *ANIM_OWNER_AS(RoomM123BeaconChannel)->pool->matrix;
        MulMatrix0(&matrix, &spin, &matrix);
        scale.x = scale.y = state->height * 2 / 5;
        scale.z = 0x1800;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        {
            RenderMatrixSlot *slot = &D_800BCFA4;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
        }
        {
            u16 *index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 3, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x60, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(D_80195698);
        FieldFx_ModelSetColors(D_80195698, color.r, color.g, color.b);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80195698, &matrix);
        FieldFx_ModelRestoreColors(D_80195698);
        FieldFx_ModelSaveColors(D_80195698);
        scale.z = 0x1000;
        scale.x = scale.y = state->depth;
        ScaleMatrix(&matrix, &scale);
        {
            u16 *index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 3, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x60, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSetColors(D_80195698, color.r, color.g, color.b);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80195698, &matrix);
        FieldFx_ModelRestoreColors(D_80195698);
        FieldFx_ModelSaveColors(D_80195698);
        scale.z = 0x1000;
        scale.x = scale.y = state->width;
        ScaleMatrix(&matrix, &scale);
        {
            u16 *index = &D_800E11EA;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            page = (u16)(D_800E2850[*index] | GetTPage(0, 1, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x60, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSetColors(D_80195698, color.r, color.g, color.b);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80195698, &matrix);
        FieldFx_ModelRestoreColors(D_80195698);
        break;
    }
    return 0;
}

int func_801945EC(int mode, RoomM123Particle *particle) {
    GteShortVector position;
    int textureFrame, scale, kind, palette;
    u16 clut;

    switch (mode) {
    case 1: {
        int frame = particle->frame;
        int speed = D_801956A4;
        int shift = speed * 24;

        frame++;
        particle->frame = frame;
        particle->offset += shift / 4096;
        if ((s16)particle->frame >= 24) return 1;
    } break;
    case 2: {
        int sampleFrame;

        scale = D_801956A4 * particle->scale;
        sampleFrame = (s16)particle->frame;
        func_800CF844(&D_801956B0, &position, sampleFrame << 6, D_801956A8,
                      scale / 4096, (s16)particle->offset);
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(0x20, palette);
        textureFrame = (s16)D_800F3368.parameter02 * (particle->frame & 7) + 0x40;
        func_800CEE20(&position, 0, 0x555, 0x555, textureFrame, clut, 1,
                      D_801956A0, 0);
    } break;
    }
    return 0;
}

int func_80194768(int mode)
{
    GteShortVector seed = D_8018F1E0;
    GteShortVector output;
    RoomM123Particle *particle;
    int i;
    int time;
    int scale;
    u16 outX, outY, outZ;
    RoomM123Pool *context;
    void *texture;
    u16 palette;

    switch (mode) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM123Pool)->pool, 8, 24,
                             (FieldFxCallbackListCallback)func_801945EC);
    case 1:
        if (D_800E27EC == 1) {
            for (i = 0; i < 24; i++) {
                particle = func_800CE610(ANIM_POOL_AS(RoomM123Pool)->pool);
                if (particle != 0) {
                    particle->frame = (i << 6) / 24;
                    particle->scale = (rand() & 63) + 64;
                    particle->offset = rand();
                }
            }
        } else {
            particle = func_800CE610(ANIM_POOL_AS(RoomM123Pool)->pool);
            if (particle != 0) {
                particle->frame = 0;
                particle->scale = (rand() & 63) + 64;
                particle->offset = rand();
            }
        }
        time = D_800E27EC;
        D_801956A4 = 0x1000;
        if (time >= 50)
            D_801956A4 = 0x1000 - ((time - 50) << 12) / 30;
        if (time < 80) break;
        return 1;
    case 2:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, 23, &seed, &output);
        scale = rsin((D_800E27EC << 11) / 80);
        if (scale < 0)
            scale += 31;
        outX = output.x;
        outY = output.y;
        outZ = output.z;
        context = ANIM_OWNER_AS(RoomM123Pool);
        /* Keep the context load ahead of the texture address. */
        asm volatile("" : : "r"(context));
        texture = D_801956A8;
        D_801956B0 = outX;
        D_801956B2 = outY;
        D_801956B4 = outZ;
        /* Finish the position writes before reducing the signed scale. */
        asm volatile("" : : : "memory");
        D_801956A0 = scale >> 5;
        func_800CE9D4(context->pool, 0, texture);
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        palette = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 12;
        D_800F3368.tpage = palette;
        break;
    }
ret0:
    return 0;
}

int func_80194A70(int mode, RoomPulseParticle *particle, int *reference)
{
    int color[2];
    GteShortVector position = D_8018F1F0;
    GteShortVector *positionPtr;
    GteMatrix **matrixSlot;
    int wave;

    positionPtr = &position;
    switch (mode) {
    case 1:
        if (particle->state != 0)
            break;
        particle->frame++;
        if (particle->frame >= 24)
            return 1;
        break;
    case 2:
        if (particle->state != 0)
            break;
        matrixSlot = &D_800BCFA4.value;
        gte_ldrotmatrix(*matrixSlot);
        gte_ldtransmatrix(*matrixSlot);
        func_800CF3AC((void *)*reference, color, particle->frame);
        wave = rsin((D_800E27EC << 10) / 24);
        func_800D0728((GteShortVector *)particle, 1000, 1500, 24, (GteRotation *)positionPtr, wave, wave, 0, (struct RenderColor *)color, 128, 1);
        break;
    }
    return 0;
}

int func_80194C04(int mode, RoomPulseParticle *particle)
{
    RoomPulseParticle *next;
    RoomPulsePool *pool;
    RoomPulseObject *object;
    RoomM123Pool *context;
    RoomSoundSlot *soundSlot;
    int time;
    int volume;
    u16 palette;

    switch (mode) {
    case 0:
        func_800CE870(ANIM_OWNER_AS(RoomM123Pool)->pool, 1, &particle->z);
        particle->x = 0;
        particle->y = 0;
        if (FX_TASK_AS(u8)[13]) {
            pool = ANIM_OWNER_AS(RoomM123Pool)->pool;
            if (pool != 0 && pool->object != 0) {
                object = pool->object;
                if (*object->status == 1)
                    *object->status = 2;
            }
        }
        soundSlot = (RoomSoundSlot *)D_800B0E64;
        if (soundSlot->channel != 0) {
            volume = 0x7F;
            time = func_800D3FD8();
            Akao_SendTableCommand(soundSlot->channel, 0x582, time, 0x80, volume);
            if (soundSlot->channel != 0)
                Akao_SendTableCommand(soundSlot->channel, 0x5B1, 0x80, 0x80, volume);
        }
        return func_800CE560(ANIM_POOL_AS(RoomM123Pool)->pool, 12, 8,
                             (FieldFxCallbackListCallback)func_80194A70);
    case 1:
        if (D_800E27EC < 32 && D_800E27EC % 6 == 0) {
            next = func_800CE610(ANIM_POOL_AS(RoomM123Pool)->pool);
            if (next != 0) {
                next->x = particle->z;
                next->y = particle->pad;
                next->z = particle->state;
                next->y = g_RoomFloorY->raw;
                next->state = 0;
                next->frame = 0;
            }
        }
        if (D_800E27EC == 32 && FX_TASK_AS(u8)[13]) {
            context = ANIM_OWNER_AS(RoomM123Pool);
            if ((((RoomPulsePool *)context->pool)->object->flags & 0x3F000000) == 0x01000000) {
                (*((RoomPulseGlobal **)D_8009D254))->flags |= 0x4000;
                ((RoomPulsePool *)context->pool)->object->flags =
                    (((RoomPulsePool *)context->pool)->object->flags & 0xC0FFFFFF) | 0x21000000;
                ((RoomPulsePool *)context->pool)->object->flags |= 0x80000000;
            }
        }
        if (D_800E27EC < 8) break;
        return 2;
    case 2:
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        palette = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
        break;
    }
ret0:
    return 0;
}

int func_80194F68(int mode, RoomM123Wave *wave) {
    int color[2];
    GteShortVector position = D_8018F1F8;
    int result, scale, kind, palette;
    u16 clut;
    switch (mode) {
    case 1:
        if (wave->state != 0) break;
        wave->frame++;
        wave->y -= 3;
        if ((s16)wave->frame >= 24) return 1;
        break;
    case 2:
        if (wave->state != 0) break;
        func_800CF3AC(D_80195624, color, (s16)wave->frame);
        result = rcos((D_800E27EC << 10) / 24);
        position.z = D_800E27EC << 5;
        scale = result / 2 + 2048;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(48, palette);
        func_800CEE20((GteShortVector *)wave, (GteRotation *)&position, scale, scale,
                      (s16)D_800F3368.parameter02 + 220, clut, 1, 128, (struct RenderColor *)color);
        break;
    }
    return 0;
}

int func_80195114(int mode, RoomM123Burst *burst, int *choice) {
    GteShortVector position = D_8018F1CC;
    RoomM123Wave *child;
    int value;

    switch (mode) {
    case 0:
        value = *choice ? 34 : 40;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM123Pool)->pool, value, &position, &burst->x);
        burst->frame = 0;
        burst->phase = 0;
        func_800D3F64(0x581, func_800D3FD8());
        return func_800CE560(ANIM_POOL_AS(RoomM123Pool)->pool, 12, 24,
                             (FieldFxCallbackListCallback)func_80194F68);
    case 1:
        if (D_800E27EC < 32) {
            child = func_800CE610(ANIM_POOL_AS(RoomM123Pool)->pool);
            if (child != 0) {
                child->x = burst->x;
                child->y = burst->y;
                child->z = burst->z;
                {
                    int sample = rand();
                    int previous = child->x;
                    child->x = (previous - 128) + (sample & 255);
                }
                {
                    int sample = rand();
                    int previous = child->y;
                    child->y = (previous - 128) + (sample & 255);
                }
                {
                    int sample = rand();
                    int previous = child->z;
                    child->z = (previous - 128) + (sample & 255);
                }
                child->state = 0;
                child->frame = 0;
            }
        }
        if (D_800E27EC >= 8) return 2;
        break;
    case 2: {
        int paletteIndex = D_800E11E8;
        int palette;
        D_800F3368.parameter00 = 16;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 16;
        D_800F3368.extent_y = 16;
        palette = D_800E2850[paletteIndex];
        D_800F3368.palette = 2;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = palette;
        break;
    }
    }
    return 0;
}
