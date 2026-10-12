#include "common.h"
#include "pe1/field_sprite_params.h"
#include "../room_lib/room_lib.h"
#include "pe1/room_script_record.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_texture_tables.h"
#include "pe1/psyq_gpu.h"
#include "pe1/room_m005.h"
#include "pe1/room_sound_slot.h"
#include "pe1/gte.h"
#include "pe1/room_m005_beacon.h"

typedef struct RoomM005_OrbRec {
    short h0;                     /* 0x00: x */
    short h2;                     /* 0x02: heading */
    short h4;                     /* 0x04: z */
    short h6;                     /* 0x06: size, ramps to 0x88 */
    short h8;                     /* 0x08: orbit phase */
    short hA;                     /* 0x0A: radius */
    short hC;                     /* 0x0C: state */
    short hE;                     /* 0x0E: fade counter */
} RoomM005_OrbRec;

typedef struct RoomM005_Dialog {
    short h0;
    short h2;
    unsigned short h4;            /* 0x04: anchor x */
    unsigned short h6;            /* 0x06: anchor y */
    unsigned short h8;            /* 0x08: anchor z */
    short hA;
    short hC[1];                  /* 0x0C: fade target block */
} RoomM005_Dialog;

/* Dialog anchor room_m005 keeps in its scene script record. */
#define RoomM005_ScriptDialog (*(RoomM005_Dialog *)&g_RoomScriptRecord)
#include "RoomM005_FxOrbiter_externs.h"
#include "pe1/gte_types.h"

int RoomM005_FxOrbiter(int mode, RoomM005_OrbRec *r) {
    unsigned short anchor[3];
    short q[4];
    char out[8];

    if (mode != 1) {
        if (mode == 2) goto burst;
        return 0;

    }
tick:
    if (r->hC != 0) {
        if (r->hC == mode) goto fade;
        return 0;

    }
init:
    anchor[0] = RoomM005_ScriptDialog.h4;
    anchor[1] = RoomM005_ScriptDialog.h6;
    anchor[2] = RoomM005_ScriptDialog.h8;
    r->h2 += rsin(D_800E27EC << 6) / 512;
    r->h0 = anchor[0] + rcos(r->h8) * r->hA / 4096;
    r->h4 = anchor[2] + rsin(r->h8) * r->hA / 4096;
    r->h8 += 0x18;
    if (r->h6 < 0x81) {
        r->h6 += 8;
    }
    if (ANIM_TASK_AS(RoomDlgState)->h12 == mode) {
        r->hC = 1;
        r->hE = 0;
    }
    return 0;

fade:
    {
        short n = r->hE + 1;
        r->hE = n;
        LoadAverageShort12(r, RoomM005_ScriptDialog.hC, 0x1000 - (n << 7), n << 7, r);
    }
    if (r->hE >= 0x20) {
        return 1;

    burst:
        q[0] = 0;
        q[1] = 0;
        q[2] = r->h8;
        q[3] = 0;
        func_800CF3AC(RoomM005_Rec2, out, (r->h2 + D_800E27EC) & 0x3F);
        {
            int a = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                a += 4;
            }
            func_800CEE20((GteShortVector *)r, (GteRotation *)q, 0x800, 0x800, (s16)D_800F3368.parameter02 + 0xD8,
                          (unsigned short)GetClut(0x20, a), 3, r->h6, (RenderColor *)out);
        }
        {
            int a = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428 != 0) {
                a += 4;
            }
            func_800CEE20((GteShortVector *)r, (GteRotation *)q, 0x1000, 0x1000, ((s16)D_800F3368.parameter02 << 1) + 0xD8,
                          (unsigned short)GetClut(0x20, a), 3, r->h6, (RenderColor *)out);
        }
    }
ret0:
    return 0;
}

#undef RoomM005_ScriptDialog

int func_8018F330(int mode, RoomM005OrbiterState *state) {
    RoomM005Seed8 seed = D_8018EFF4;
    s16 target[4];
    char *pool;
    RoomM005OrbiterChild *child;
    RoomSoundSlot *soundSlot;
    int handle;
    int spread;
    int volume;

    switch (mode) {
    case 0:
        state->armed = 0;
        state->frame = 0;
        pool = ANIM_POOL_AS(RoomM005Channel)->pool;
        return func_800CE560(pool, 16, 24,
                             (FieldAnimCallbackListCallback)RoomM005_FxOrbiter);
    case 1:
        if (D_800E27EC == 7) {
            soundSlot = (RoomSoundSlot *)D_800B0E64;
            if (soundSlot->channel != 0) {
                volume = 0x7F;
                handle = func_800D3FD8();
                Akao_SendTableCommand(soundSlot->channel, 0x5AB, handle, 0x80, volume);
                if (soundSlot->channel != 0) {
                    Akao_SendTableCommand(soundSlot->channel, 0x5AC, 0x80, 0x80, volume);
                }
            }
        }
        if (D_800E27EC < 25) {
            pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
            func_800CE870(pool, 0, target);
            pool = ANIM_POOL_AS(RoomM005Channel)->pool;
            child = func_800CE610(pool);
            if (child) {
                child->seed = func_80071A54();
                spread = func_80071A54() & 0x1FF;
                child->y = target[1] + spread - 0x100;
                child->flag = 0;
                child->radius = (func_80071A54() & 0xFF) + 0x100;
                child->state = 0;
                child->timer = 0;
            }
        }
        if (state->armed == 1) {
            state->frame++;
            if ((s16)state->frame >= 32) return 1;
        } else if (ANIM_TASK_AS(RoomM005EventState)->triggered != 0) {
            state->armed = 1;
        }
        break;
    case 2:
        pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
        func_800CE870(pool, 0, (s16 *)D_80190B84);
        pool = ANIM_OWNER_AS(RoomM005Channel)->pool;
        FieldFx_TransformMatrixPoint(pool, 20, &seed, D_80190B8C);
        {
            int idx = D_800E11E8;
            int palette;
            D_800F3368.parameter00 = 16;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = 16;
            D_800F3368.extent_y = 16;
            palette = D_800E2850[idx];
            PE1_COMPILER_MEMORY_BARRIER();
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 8;
            D_800F3368.tpage = palette;
        }
        break;
    }
    return 0;
}

/* Cross flare: two small sprites either side of joint 9 while the frame
 * counter is below 35, then a flash track, two streaks and a large turning
 * sprite at joint 20. The first draw's intensity, the streak step and its
 * width are separate variables (all land in s0), while the streak fade and
 * the final size share one, and parameter02 is written second: each of
 * these decides retail's register choice. */

int func_8018F614(int mode) {
    GteShortVector position;
    GteRotation rotation;
    RoomM005Seed8 outer = D_8018EFFC;
    RoomM005Seed8 inner = D_8018F004;
    RenderColor color;
    RoomSoundSlot *soundSlot;
    int volume;
    int handle;
    int size;
    int step;
    int width;
    int level;

    switch (mode) {
    case 0:
        soundSlot = (RoomSoundSlot *)D_800B0E64;
        if (soundSlot->channel != 0) {
            volume = 0x7F;
            handle = func_800D3FD8();
            Akao_SendTableCommand(soundSlot->channel, 0x5F8, handle, 0x80, volume);
            if (soundSlot->channel != 0) {
                Akao_SendTableCommand(soundSlot->channel, 0x5F9, 0x80, 0x80, volume);
            }
        }
        break;
    case 1:
        if (D_800E27EC >= 35) return 1;
        break;
    case 2:
        D_800F3368.depth = 0x40;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM005Channel)->pool, 9, &inner, &position);
        if (D_800E27EC < 35) {
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            D_800F3368.tpage = D_800E2850[D_800E11EA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.parameter0A = 0;
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC << 6;
            rotation.flags = 0;
            level = rsin((D_800E27EC << 11) / 34);
            position.x -= 0x10;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&position, &rotation, 0x1000, 0x1000, 2,
                              (u16)GetClut(0x10, palette), 1, level, 0);
            }
            position.x += 0x20;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20(&position, &rotation, 0x1000, 0x1000, 2,
                              (u16)GetClut(0x10, palette), 1, level, 0);
            }
        }
        D_800F3368.depth = 0x18;
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM005Channel)->pool, 0x14, &outer, &position);
        if (D_800E27EC < 35) {
            step = D_800E27EC - 18;
            if ((unsigned int)step < 17) {
                size = rcos(step << 6);
                width = 600 - step * 100;
                func_800CF3AC(D_80190AF4, &color, step * 2);
                func_800D0728(&position, width, 700, 0x18, 0, size, size,
                              0, &color, 0x40, 1);
                func_800D0728(&position, 700, 800, 0x18, 0, size, size,
                              &color, 0, 0x40, 1);
            }
            func_800CF3AC(D_80190AF4, &color, D_800E27EC);
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            D_800F3368.tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            size = rsin((D_800E27EC << 10) / 34);
            rotation.x = 0;
            rotation.y = 0;
            rotation.z = D_800E27EC << 6;
            rotation.flags = 0;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                func_800CEE20(&position, &rotation, size, size, 0x40,
                              (u16)GetClut(0, (kind == 4 && D_800F3428) ? palette + 6 : palette + 2),
                              1, 0x60, &color);
            }
        }
        break;
    }
    return 0;
}

/* Sprite that drifts by a fixed velocity for six frames, then draws itself
 * at a room offset with a cosine-driven intensity. */
int func_8018FB84(int mode, RoomM005DriftingSpriteState *state) {
    GteShortVector position;
    RenderColor color;
    RenderMatrixSlot *matrixSlot;
    int scale;
    int kind;
    int palette;
    int u;
    int v;
    u16 clut;

    color = D_8018F00C;
    switch (mode) {
    case 1:
        state->x += state->vx;
        state->y += state->vy;
        state->z += state->vz;
        if (D_800E27EC >= 6) return 1;
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        position.x = state->x;
        position.y = state->y;
        position.z = state->z;
        position.x += D_80190BA0.x;
        position.y += D_80190BA0.y;
        position.z += D_80190BA0.z;
        u = (D_800E27EC & 1) << 7;
        v = ((D_800E27EC << 3) & 0x10) + 0x20;
        scale = rcos((D_800E27EC << 10) / 6) / 24;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = GetClut(0, palette);
        func_800D2370(&D_80190BA8, (GteRotation *)&position, 0x44C, 0x6E, u, v,
                      0x7F, 0xF, clut, &color, &color, (short)scale, 1);
        break;
    }
    return 0;
}

/* Joint beacon: rides joint 0x14, sheds drifting sprites for the first
 * frames, swells and shrinks, tags the player on contact; draws four passes
 * of a model, three glow sprites and a turning second model. */
int func_8018FDC4(int mode, RoomM005Beacon *state) {
    GteVector scale;
    GteShortVector position;
    GteShortVector rotation;
    RoomM005BeaconOffset offset = D_8018F010;
    GteShortVector spinAngles;
    GteMatrix matrix;
    GteMatrix spin;
    GteVector spinScale;
    RoomM005BeaconSprite *sprite;
    int size;
    int alpha;
    int page;

    switch (mode) {
    case 0:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomM005BeaconChannel)->pool, 0x14, &offset, state);
        func_800CE9D4((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomM005BeaconChannel)->pool, 0,
                      &state->rotation);
        state->rotation.pad = 1;
        state->phase = 0;
        state->timer = 0;
        state->height = 0;
        if (ANIM_TASK_AS(RoomM005BeaconEvent)->active) {
            RoomM005BeaconPool *pool = ANIM_OWNER_AS(RoomM005BeaconChannel)->pool;
            if (pool && pool->node) {
                u8 *flag = pool->node->state;
                if (*flag == 1) *flag = 2;
            }
        }
        D_80190B94 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5462704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_80190B94, 0, 0);
        D_80190B98 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5862704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_80190B98, 0, 0);
        return func_800CE560(ANIM_POOL_AS(RoomM005BeaconSpawnChannel)->pool, 0x10, 8,
                             (FieldAnimCallbackListCallback)func_8018FB84);
    case 1:
        if (D_800E27EC < 10) {
            sprite = func_800CE610(ANIM_POOL_AS(RoomM005BeaconSpawnChannel)->pool);
            if (sprite) {
                sprite->x = (func_80071A54() & 0x3FF) - 0x200;
                sprite->y = (func_80071A54() & 0x3FF) - 0x200;
                sprite->z = 0;
                sprite->vx = (func_80071A54() & 7) - 3;
                sprite->vy = (func_80071A54() & 7) - 3;
                sprite->vz = 0;
            }
        }
        switch (state->phase) {
        case 0:
            state->timer++;
            state->width = 0x1000;
            state->height += 0x200;
            if (state->timer >= 5) {
                state->phase = 1;
                state->timer = 0;
            }
            break;
        case 1:
            state->timer++;
            state->width = 0x1000 - (state->timer << 9);
            if (state->timer >= 8) return 1;
            break;
        }
        func_800CFB7C(&state->rotation, state->height, &position);
        state->target.x = state->position.x;
        state->target.y = state->position.y;
        state->target.z = state->position.z;
        state->target.x += position.x;
        state->target.y += position.y;
        state->target.z += position.z;
        if (func_800CEB8C(state, &state->target, 100) == 0) return 0;
        if (ANIM_TASK_AS(RoomM005BeaconEvent)->active) {
            RoomM005BeaconChannel *channel = ANIM_OWNER_AS(RoomM005BeaconChannel);
            if ((channel->pool->node->flags & 0x3F000000) != 0x01000000) return 0;
            ((RoomM005BeaconEntity *)D_8009D254)->actor->flags |= 0x4000;
            channel->pool->node->flags =
                (channel->pool->node->flags & 0xC0FFFFFF) | 0x11000000;
            channel->pool->node->flags |= 0x80000000;
        }
        break;
    case 2:
        D_800F3368.depth = 8;
        matrix = *ANIM_OWNER_AS(RoomM005BeaconChannel)->pool->matrix;
        scale.x = scale.y = state->width / 40;
        scale.z = (state->height << 12) / 8680;
        ScaleMatrix(&matrix, &scale);
        matrix.t[0] = state->position.x;
        matrix.t[1] = state->position.y;
        matrix.t[2] = state->position.z;
        {
            RenderMatrixSlot *slot;
            u16 *index = &D_800E11EA;
            u16 *tpages = D_800E2850;
            D_800F3368.tpage = D_800E2850[*index];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 0;
            slot = &D_800BCFA4;
            gte_ldrotmatrix(slot->value);
            gte_ldtransmatrix(slot->value);
            page = (u16)(tpages[*index] | GetTPage(0, 1, 0, 0));
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428) palette += 4;
            FieldFx_ModelSetTexture(page, GetClut(0x30, palette));
        }
        FieldFx_ModelSetSemiTrans(1);
        FieldFx_ModelSaveColors(D_80190B94);
        FieldFx_ModelScaleColors(D_80190B94, 0x32);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80190B94, &matrix);
        FieldFx_ModelRestoreColors(D_80190B94);
        FieldFx_ModelSaveColors(D_80190B94);
        scale.z = 0x1000;
        scale.x = scale.y = 0x2000;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelShiftColors(D_80190B94, -0xFF, -0x82, 0x30);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80190B94, &matrix);
        FieldFx_ModelRestoreColors(D_80190B94);
        FieldFx_ModelSaveColors(D_80190B94);
        scale.x = scale.y = 0x400;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelScaleColors(D_80190B94, 0xC8);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80190B94, &matrix);
        FieldFx_ModelRestoreColors(D_80190B94);
        FieldFx_ModelSaveColors(D_80190B94);
        matrix.t[1] = g_RoomFloorY->y;
        scale.x = scale.y = 0x2000;
        ScaleMatrix(&matrix, &scale);
        FieldFx_ModelScaleColors(D_80190B94, 0x18);
        FieldFx_ModelDraw((union FieldTexturedModel *)D_80190B94, &matrix);
        FieldFx_ModelRestoreColors(D_80190B94);
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 1;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0x20;
        alpha = 0xA0;
        size = state->width;
        if (D_800E27EC & 1) {
            alpha = 0x80;
            size = size * 3 / 2;
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&state->position, 0, size, size, 2,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, alpha, 0);
        }
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&state->target, 0, state->width / 2, state->width / 2, 2,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          1, alpha, 0);
        }
        position.x = state->position.x;
        position.y = state->position.y;
        position.z = state->position.z;
        position.y = g_RoomFloorY->y;
        rotation.y = 0;
        rotation.z = 0;
        rotation.x = 0x400;
        rotation.pad = 1;
        {
            int kind = D_800F3368.palette;
            int palette = D_800E1204[kind];
            func_800CEE20(&position, (GteRotation *)&rotation, size, size, 2,
                          GetClut(0, (kind == 4 && D_800F3428) ? palette + 8 : palette + 4),
                          3, alpha, 0);
        }
        if (D_800E27EC < 0xD) {
            alpha = rcos((D_800E27EC << 10) / 12) / 32;
            size = rsin((D_800E27EC << 10) / 12);
            {
                RenderMatrixSlot *slot = &D_800BCFA4;
                gte_ldrotmatrix(slot->value);
                gte_ldtransmatrix(slot->value);
            }
            {
                u16 *index = &D_800E11FA;
                D_800F3368.tpage = D_800E2850[*index];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 1;
                page = (u16)(D_800E2850[*index] | GetTPage(0, 1, 0, 0));
            }
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                FieldFx_ModelSetTexture(page, GetClut(0, (kind == 4 && D_800F3428) ? palette + 7
                                                                     : palette + 3));
            }
            FieldFx_ModelSetSemiTrans(1);
            spinAngles.x = state->rotation.x;
            spinAngles.y = state->rotation.y;
            spinAngles.z = state->rotation.z;
            spinAngles.x += 0x800;
            spinAngles.z = D_800E27EC << 7;
            RotMatrixYXZ(&spinAngles, &spin);
            spin.t[0] = state->position.x;
            spin.t[1] = state->position.y;
            spin.t[2] = state->position.z;
            spinScale.z = spinScale.y = spinScale.x = size / 2;
            ScaleMatrix(&spin, &spinScale);
            FieldFx_ModelSaveColors(D_80190B98);
            FieldFx_ModelScaleColors(D_80190B98, alpha / 2);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_80190B98, &spin);
            FieldFx_ModelRestoreColors(D_80190B98);
        }
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 4;
        D_80190BA8.x = state->position.x;
        D_80190BA8.y = state->position.y;
        D_80190BA8.z = state->position.z;
        D_80190BA0.x = state->rotation.x;
        D_80190BA0.y = state->rotation.y;
        D_80190BA0.z = 0;
        D_80190BA0.x = 0;
        break;
    }
    return 0;
}
