#include "common.h"
#include "pe1/field_texture_tables.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/room_m256.h"
#include "pe1/room_m256_drop.h"
#include "pe1/room_m256_effects.h"
#include "pe1/room_m256_swing.h"
#include "pe1/room_m256_trail.h"
#include "pe1/room_model_pulse.h"
#include "pe1/room_orbiting_pair.h"

#include "m256_externs.h"

/* Cosine halves of the packed sine table words (pe1/gte_sine_table.h). */

void func_80192680(RoomM256Swing *state);

void func_80192654(RoomM256Swing *obj) {
    FieldActor *slot = obj->actor;

    if (slot->mode == 0xB) {
        obj->cycle.callback = (void (*)(void))func_80192680;
    }
}

void func_80192680(RoomM256Swing *state) {
    FieldActor *actor = state->actor;
    RoomM256SwingCycle *base = &state->cycle;
    int first;
    int phase;
    int value;
    int timer;
    int baseValue;

    if (state->cycle.started == 0) {
        state->cycle.started = 1;
        state->cycle.tick = 0;
        first = base->angles[state->cycle.segment];
        state->cycle.startAngle = first;
        state->cycle.sweep = base->angles[state->cycle.segment + 1] - first;
        state->cycle.length = base->durations[state->cycle.segment];
    }

    timer = (u16)state->cycle.tick + 1;
    phase = ((timer << 16) >> 5) / state->cycle.length;
    state->cycle.tick = timer;
    value = (-D_800966EE[(phase & 0xFFF) * 2] + 0x1000) *
            state->cycle.sweep;
    if (value < 0) {
        value += 0x1FFF;
    }

    baseValue = (u16)state->cycle.startAngle;
    actor->render_object.rotation_overrides[0].matrix_index = 3;
    actor->render_object.rotation_overrides[0].flags = 2;
    actor->render_object.rotation_overrides[0].y = baseValue + (value >> 13);

    if (state->cycle.tick >= state->cycle.length) {
        state->cycle.segment = (u16)state->cycle.segment + 1;
        if (state->cycle.segment >= 3) {
            func_80192808(state);
        } else {
            state->cycle.started = 0;
        }
    }

    if (actor->anim.parts.integer >= actor->action) {
        actor->anim.fixed = 0;
    }
}

int func_80192808(RoomM256Swing *obj) {
    int *state;

    obj->actor->render_object.rotation_overrides[0].matrix_index = 0;
    obj->actor->render_object.rotation_overrides[0].flags = 0;
    state = obj->cycle.signal;
    obj->status = 4;
    if (state != 0) {
        *state = 0;
    }

    return 0;
}

int func_8019283C(void) {
    return 0;
}

/* Flashes the scene when the drop touches the player. */
#define ROOM_M256_DROP_FLASH(radius)                                          \
    if (func_800C6B90(drop, radius) && drop->hazard && FX_TASK_AS(RoomM256DropEvent)->active) {  \
        RoomM256DropChannel *channel = ANIM_OWNER_AS(RoomM256DropChannel);                            \
        if ((channel->pool->node->flags & 0x3F000000) == 0x01000000) {        \
            ((RoomM256DropEntity *)D_8009D254)->actor->flags |= 0x4000;                               \
            channel->pool->node->flags =                                      \
                (channel->pool->node->flags & 0xC0FFFFFF) | 0x19000000;       \
            channel->pool->node->flags |= 0x80000000;                         \
        }                                                                     \
    }

/* Scatter drop: falls and jitters, sheds three kinds of splash children,
 * flashes the scene on contact with the player and expires once it leaves
 * the walkable area; the splash children slow down and bounce. */
int func_80192844(int mode, RoomM256Drop *drop) {
    GteRotation rotation = D_8018F1CC;
    RoomM256DropColor color = D_8018F1D4;
    RoomM256Drop *child;
    RenderMatrixSlot *slot;
    int width;
    int height;
    int fall;
    int bounce;

    switch (mode) {
    case 1:
        switch (drop->state) {
        case 0:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (rand() & 0x1F) - 0x10;
            drop->y += (rand() & 0x1F) - 0x10;
            drop->z += (rand() & 0x1F) - 0x10;
            if (drop->y >= g_RoomFloorY->y - 0x10 || drop->timer >= 0x18) {
                drop->state = 1;
                drop->y = g_RoomFloorY->y - 0x10;
                drop->vy = 0;
            }
            if (drop->timer & 1) {
                if ((rand() & 7) == 0) {
                    child = func_800CE610(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = drop->vx / 2 + (rand() & 0x1F) - 0x10;
                        child->vy = drop->vy / 2 + (rand() & 0x1F) - 0x10;
                        child->vz = drop->vz / 2 + (rand() & 0x1F) - 0x10;
                        child->state = 2;
                        child->timer = 0;
                    }
                }
                if ((rand() & 7) == 1) {
                    child = func_800CE610(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = (rand() & 0x1F) - 0x10;
                        child->vy = (rand() & 0x1F) - 0x10;
                        child->vz = (rand() & 0x1F) - 0x10;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
                if ((rand() & 0xF) == 2) {
                    child = func_800CE610(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = (rand() & 0x1F) - 0x10;
                        child->vy = (rand() & 0x1F) - 0x18;
                        child->vz = (rand() & 0x1F) - 0x10;
                        child->state = 4;
                        child->timer = 0;
                    }
                }
            }
            ROOM_M256_DROP_FLASH(0x19A);
            if (Geo_PointInPoly(drop->x << 16, drop->z << 16, D_8009D248, D_8009D1CC) == 0) return 1;
            if (drop->timer >= 0x20) return 1;
            break;
        case 1:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->x += (rand() & 0x1F) - 0x10;
            drop->z += (rand() & 0x1F) - 0x10;
            ROOM_M256_DROP_FLASH(0x26C);
            if (Geo_PointInPoly(drop->x << 16, drop->z << 16, D_8009D248, D_8009D1CC) == 0) return 1;
            if (drop->timer >= 0x20) return 1;
            break;
        case 2:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 127 / 128;
            drop->vz = drop->vz * 127 / 128;
            fall = (u16)drop->vy - 2;
            drop->vy = fall;
            if (drop->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        case 3:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 511 / 512;
            drop->vz = drop->vz * 511 / 512;
            fall = (u16)drop->vy - 3;
            drop->vy = fall;
            if (drop->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        case 4:
            drop->timer++;
            drop->x += drop->vx;
            drop->y += drop->vy;
            drop->z += drop->vz;
            drop->vx = drop->vx * 31 / 32;
            drop->vz = drop->vz * 31 / 32;
            fall = (u16)drop->vy - 3;
            drop->vy = fall;
            if (drop->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                drop->vy = bounce;
            }
            if (drop->timer >= 0x10) return 1;
            break;
        }
        break;
    case 2:
        slot = &D_800BCFA4;
        gte_ldrotmatrix(slot->value);
        gte_ldtransmatrix(slot->value);
        switch (drop->state) {
        case 0:
            width = (rsin(drop->timer << 5) + 0x1200) * 2;
            height = (rsin(drop->timer << 5) + 0xE00) * 2;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20((GteShortVector *)drop, 0, width, height,
                              (s16)D_800F3368.parameter02 * (drop->timer / 4),
                              GetClut(0, palette), 1, 0x72, 0);
            }
            if (rand() & 3) return 0;
            func_800D004C((GteShortVector *)drop, 0xFA, 0xFA, 5, 0, width, width, (struct RenderColor *)&color, 0,
                          rcos(drop->timer << 5) / 32, 3);
            return 0;
        case 1:
            width = (rsin(drop->timer << 5) + 0x1200) * 2;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20((GteShortVector *)drop, &rotation, width, width,
                              (s16)D_800F3368.parameter02 * (drop->timer / 4) + 0x20,
                              GetClut(0x10, palette), 1, 0x80, 0);
            }
            break;
        case 2:
            width = rsin(drop->timer << 6) / 2 + 0x1200;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20((GteShortVector *)drop, 0, width, width,
                              (s16)D_800F3368.parameter02 * (drop->timer / 2),
                              GetClut(0x10, palette), 1, 0x60, 0);
            }
            return 0;
        case 3:
            width = rsin(drop->timer << 6) / 2 + 0x1000;
            {
                int kind = D_800F3368.palette;
                int palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428) palette += 4;
                func_800CEE20((GteShortVector *)drop, 0, width, width,
                              (s16)D_800F3368.parameter02 * (drop->timer / 2) + 0x40,
                              GetClut(0x20, palette), 2, 0x50, 0);
            }
            break;
        case 4:
            width = rcos(drop->timer << 6) / 32 + 0x40;
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            func_800D1DEC(drop, &color, width, 1);
            break;
        }
        break;
    }
    return 0;
}

#undef ROOM_M256_DROP_FLASH

#undef ROOM_M256_DROP_FLASH

/* The stack templates the effects below copy: read-only data of this unit,
 * which GCC emits before the unit's jump tables, as retail has them. */
const RoomOrbitPairTemplate8 D_8018F208 = {{0, 0xFFE2}};
const RoomM256Template D_8018F210 = {{0, 0xFFBA}};
const RenderColor D_8018F218 = {0xC8, 0xBE, 0x18, 0};
const RenderColor D_8018F21C = {0x0A, 0x80, 0xF0, 0};
const GteRotation D_8018F220 = {0, 0, -10, 0};

int func_801934AC(int mode, u16 *counter) {
    RoomOrbitPairTemplate8 template = D_8018F208;
    s16 position[4];
    s16 target[4];
    s16 angles[4];
    char *pool;
    RoomOrbitSpark *child;
    int distance;
    int spread;
    switch (mode) {
    case 0:
    {
        RoomOrbitPairEventState *event = FX_TASK_AS(RoomOrbitPairEventState);
        *counter = 0;
        if (event->active) {
            RoomOrbitPairNode **slot = (RoomOrbitPairNode **)ANIM_OWNER_AS(RoomOrbitPairChannel)->pool;
            if (slot && *slot) {
                u8 *flag = (*slot)->state;
                if (*flag == 1) *flag = 2;
            }
        }
    }
    pool = ANIM_POOL_AS(RoomOrbitPairChannel)->pool;
    return func_800CE560(pool, 20, 50,
                         (FieldFxCallbackListCallback)func_80192844);
    case 1:
    pool = ANIM_OWNER_AS(RoomOrbitPairChannel)->pool;
    FieldFx_TransformMatrixPoint(pool, 4, &template, position);
    pool = ANIM_OWNER_AS(RoomOrbitPairChannel)->pool;
    func_800CE9D4((struct RoomFxTransformOwner *)pool, 4, (GteShortVector *)target);
    target[0] -= 0xDC;
    if (D_800E27EC < 110 && (D_800E27EC & 1)) {
        pool = ANIM_POOL_AS(RoomOrbitPairChannel)->pool;
        child = func_800CE610(pool);
        if (child) {
            distance = (rand() & 7) + 0x37;
            child->x = position[0];
            child->y = position[1];
            child->z = position[2];
            angles[0] = target[0];
            angles[1] = target[1];
            angles[2] = target[2];
            spread = rand() & 0x1F;
            angles[1] += spread - 0x10;
            func_800CFB7C((GteShortVector *)angles, distance, (GteShortVector *)&child->vx);
            child->parity = *counter & 1;
            child->state = 0;
            child->timer = 0;
        }
        (*counter)++;
    }
    if (D_800E27EC < 2) break;
    return 2;
    case 2:
    {
        int idx = D_800E11EA;
        int palette;
        D_800F3368.parameter00 = 32;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 32;
        D_800F3368.extent_y = 32;
        palette = D_800E2850[idx];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = palette;
    }
        break;
    }
    return 0;
}

/* Spark that jitters sideways (state 0) or swirls on a spinning circle
 * (state 1) while it falls with damped velocity and bounces off the
 * floor height; draws as a fixed or a pulsing sprite. */
int func_8019377C(int mode, RoomM256SwirlSpark *spark) {
    RenderMatrixSlot *matrixSlot;
    int fall;
    int bounce;
    int scale;

    switch (mode) {
    case 1:
        switch (spark->state) {
        case 0:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 31 / 32;
            spark->vz = spark->vz * 31 / 32;
            fall = (u16)spark->vy - 1;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->x += (rand() & 1) * 2 - 1;
            spark->z += (rand() & 1) * 2 - 1;
            if ((s16)spark->timer < 8) break;
            return 1;
        case 1:
            spark->timer++;
            spark->x += spark->vx;
            spark->y += spark->vy;
            spark->z += spark->vz;
            spark->vx = spark->vx * 511 / 512;
            spark->vz = spark->vz * 511 / 512;
            fall = (u16)spark->vy - 2;
            spark->vy = fall;
            if (spark->y >= g_RoomFloorY->y) {
                bounce = -(s16)fall;
                spark->vy = bounce;
            }
            spark->spin += 0x100;
            spark->x += rsin(spark->spin << 8) / 1024;
            spark->z += rcos(spark->spin << 8) / 1024;
            if ((s16)spark->timer < 16) break;
            return 1;
        }
        break;
    case 2:
        matrixSlot = &D_800BCFA4;
        gte_ldrotmatrix(matrixSlot->value);
        gte_ldtransmatrix(matrixSlot->value);
        switch (spark->state) {
        case 0: {
            u16 clut;
            int kind;
            int palette;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0, palette);
            func_800CEE20((GteShortVector *)spark, 0, 0x1800, 0x1800,
                          (s16)D_800F3368.parameter02 * (s16)spark->timer,
                          clut, 1, 100, 0);
            break;
        }
        case 1: {
            u16 clut;
            int kind;
            int palette;
            int page = 2;
            scale = rsin(((s16)spark->timer << 10) / 24) + 0x1000;
            scale /= 2;
            if (spark->small) page = 1;
            kind = D_800F3368.palette;
            palette = D_800E1204[kind];
            if (kind == 4 && D_800F3428 != 0) palette += 4;
            clut = GetClut(0x20, palette);
            func_800CEE20((GteShortVector *)spark, 0, scale, scale,
                          (s16)D_800F3368.parameter02 * ((s16)spark->timer / 2) + 0x40,
                          clut, page, 0x50, 0);
            break;
        }
        }
        break;
    }
    return 0;
}

/* Swirl spark controller: plays its two sounds on start, then while the
 * effect is young sheds two swirl sparks per frame from the tracked joint,
 * aimed through one of three (then one of two) headings picked by a
 * rotating counter; mode 2 sets the sprite parameters. */
int func_80193BFC(int mode, s16 *counter) {
    RoomM256Template template = D_8018F210;
    GteShortVector position;
    GteShortVector base;
    GteShortVector angles;
    RoomM256SwirlSpark *child;
    void **soundSlot;
    int distance = 0;
    int volume;
    int time;

    switch (mode) {
    case 0:
        *counter = 0;
        soundSlot = D_800B0E64;
        if (*soundSlot != 0) {
            volume = 0x7F;
            time = func_800D3FD8();
            Akao_SendTableCommand(*soundSlot, 0x5A7, time, 0x80, volume);
            /* Retail re-reads the sound owner for the test and again for
             * the argument. */
            if (*(void *volatile *)soundSlot != 0)
                Akao_SendTableCommand(*soundSlot, 0x5BF, 0x80, 0x80, volume);
        }
        return func_800CE560(ANIM_POOL_AS(RoomSparkChannel)->pool, 0x14, 0x20,
                             (FieldFxCallbackListCallback)func_8019377C);
    case 1:
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 4, &template, &position);
        func_800CE9D4((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomSparkChannel)->pool, 4, &base);
        if (D_800E27EC < 0x6F) {
            child = (RoomM256SwirlSpark *)func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                switch (*counter) {
                case 0:
                    distance = (rand() & 7) + 14;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (rand() & 0x7F) - 0x40;
                    break;
                case 1:
                    distance = (rand() & 0xF) + 10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (rand() & 0x7F) + 0x3C0;
                    break;
                case 2:
                    distance = (rand() & 0xF) + 10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y -= (rand() & 0x7F) + 0x3C0;
                    break;
                }
                func_800CFB7C(&angles, distance, (GteShortVector *)&child->vx);
                child->state = 0;
                child->timer = 0;
            }
            child = (RoomM256SwirlSpark *)func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
            if (child) {
                child->x = position.x;
                child->y = position.y;
                child->z = position.z;
                switch (*counter & 1) {
                case 0:
                    distance = (rand() & 0xF) + 0x10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y += (rand() & 0xFF) + 0x380;
                    break;
                case 1:
                    distance = (rand() & 0xF) + 0x10;
                    angles.x = base.x;
                    angles.y = base.y;
                    angles.z = base.z;
                    angles.y -= (rand() & 0xFF) + 0x380;
                    break;
                }
                func_800CFB7C(&angles, distance, (GteShortVector *)&child->vx);
                child->small = rand() & 1;
                child->state = 1;
                child->timer = 0;
            }
            *counter = (s16)(*counter + 1) % 3;
        }
        if (D_800E27EC < 2) {
            break;
        }
        return 2;
    case 2:
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        D_800F3368.tpage = D_800E2850[D_800E11EA];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        break;
    }
    return 0;
}

s32 func_801940B0(int mode, void *effect) {
    RoomM256TrailEffect *state = effect;
    RenderColor color = D_8018F218;
    RenderColor darkColor = D_8018F21C;
    GteRotation initialRotation = D_8018F220;
    GteRotation spriteRotation = D_8018F1CC;
    RoomM256TrailEffect *particle;
    GteShortVector delta;
    GteShortVector angles;
    GteMatrix transform;
    GteVector scale;
    /* Unused space; the retail frame is 0xF0 bytes. */
    s16 unusedVector[12];
    s16 impactFrame;
    s16 historyIndexX;
    s16 historyIndexY;
    s16 historyIndexZ;
    s32 nextModelFrame;
    s32 nextSpriteFrame;
    s16 renderKind;
    s16 ending;
    s16 turnAngle;
    s16 speedBeforeStep;
    s16 speedAfterStep;
    s16 nextEnding;
    s16 updateKind;
    FieldActorState *trailActorState;
    FieldActorState *trailActorFlags;
    FieldActorState *impactActorState;
    FieldActorState *impactActorFlags;
    s32 impactScale;
    s32 randomVelocityX;
    s32 randomVelocityY;
    s32 modelPage;
    s32 pulseCosine;
    s32 impactCosine;
    s32 randomVelocityZ;
    s32 turnBlend;
    s32 squaredSpeed;
    s32 alive;
    RenderEffectParameters *params;
    RoomM256TrailPalettes *palettes;
    u16 *paletteIndex;
    s16 *floorHeight;
    int tpage;
    int clut;
    s16 impactY;
    int historyCount;
    void *historyPosition;
    GteShortVector *historySample;
    s16 *spriteFrame;
    register int pulseExtent asm("$3");
    int spriteExtent;
    unsigned spritePalette;
    int spriteClutY;
    int translationZ;
    int renderPalette;
    s32 random;
    s32 renderScale;
    s32 fade;
    register int firstFade asm("$21");
    u16 initialPitch;
    u16 initialYaw;
    int trailClutY;
    int pulseClutY;
    int impactClutY;
    int ringClutY;
    int modelClutY;
    u32 tripledScale;
    u32 pulseSine;
    void *motionAngles;
    RoomM256TrailHistory *history;
    FieldActorState *trailPlayerState;
    FieldActorState *impactPlayerState;

    switch (mode) {
    case 1:
        updateKind = (s16) state->kind;
        switch (updateKind) {
        case 0:
            state->frame = 0;
            state->kind = 1U;
            state->ending = 0;
            FieldFx_TransformMatrixPoint(D_800F32D0->actor, 2, &initialRotation, state);
            func_800CE9D4((struct RoomFxTransformOwner *)D_800F32D0->actor, 0,
                          (GteShortVector *)&angles.x);
            historyCount = 16;
            historyPosition = state;
            initialPitch = angles.x + 0x200;
            angles.x = initialPitch;
            {
                int y = (u16)angles.y - 0x200;
                int shiftedIndex = state->index << 9;
                initialYaw = y + shiftedIndex;
            }
            angles.y = initialYaw;
            state->motionX = initialPitch;
            state->motionY = angles.y;
            state->motionZ = angles.z;
            state->motionY = (u16) (state->motionY + 0x800);
            FieldFx_UpdatePointHistory(&D_80195EFC[state->index], historyCount, historyPosition, 1);
            state->speed = (s16) ((rand() & 7) + 8);
            state->angle = 0;
            particle = func_800CE610(D_800F33E0->end);

            if (particle != 0) {
                particle->x = (u16) state->x;
                particle->y = (u16) state->y;
                {
                    int z = (u16)state->z;
                    particle->kind = 4;
                    particle->frame = 0;
                    particle->z = z;
                }

            }
            return 0;
        case 1:
            state->frame = (s16) ((u16) state->frame + 1);
            history = &D_80195EFC[state->index];
            func_800CE870((char *)g_PlayerEntity, 1, &delta.x);
            FieldFx_CalculateLookAngles(state, &delta.x, &angles.x);
            ending = state->ending;
            if (ending == 0) {
                if (state->speed >= 0x1B) {
                    turnAngle = state->angle;
                    if (turnAngle < 0x800) {
                        state->angle = (s16) (turnAngle + 0x50);
                    }
                    if (state->angle >= 0x801) {
                        state->angle = 0x800;
                    }
                }
                turnBlend = rsin(state->angle) * D_80195EF0;
                if (turnBlend < 0) {
                    turnBlend += 0xFFF;
                }
                motionAngles = &state->motionX;
                func_800CFD50(&angles.x, motionAngles, (turnBlend >> 0xC) & 0xFFFF);
                speedBeforeStep = state->speed;
                if (speedBeforeStep < 0x32) {
                    state->speed = (s16) (speedBeforeStep + 6);
                }
                speedAfterStep = state->speed;
                squaredSpeed = speedAfterStep * speedAfterStep;
                if (squaredSpeed < 0) {
                    squaredSpeed += 0xF;
                }
                func_800CFB7C(motionAngles, (s32) (squaredSpeed << 0xC) >> 0x10, (GteShortVector *)&delta.x);
                state->x = (s16) ((u16) state->x + delta.x);
                state->y = (s16) ((u16) state->y + delta.y);
                state->z = (s16) ((u16) state->z + delta.z);
            } else {
                nextEnding = ending + 1;
                state->ending = nextEnding;
                if (nextEnding >= 0x10) return 1;
            }
            {
                FieldFx_UpdatePointHistory(history, 0x10, state, 0);
                if ((func_800C6B90(state, 0x64) != 0) && (state->y >= (g_RoomFloorY->y - 0x202)) && (D_800E2368->flags != 0) && ((D_800F32D0->actor->state->core_flags & 0x3F000000) == 0x01000000)) {
                    trailPlayerState = g_PlayerEntity->state;
                    trailPlayerState->flags = (s32) (trailPlayerState->flags | 0x4000);
                    trailActorState = D_800F32D0->actor->state;
                    trailActorState->core_flags = (trailActorState->core_flags & 0xC0FFFFFF) | 0x19000000;
                    trailActorFlags = D_800F32D0->actor->state;
                    trailActorFlags->core_flags |= 0x80000000;
                }
                if (state->ending == 0) {
                    if (D_800E27EC == (((D_800E27EC / 3)) * 3)) {
                        particle = func_800CE610(D_800F33E0->end);
                        if (particle != 0) {
                            particle->x = (u16) state->x;
                            particle->y = (u16) state->y;
                            particle->z = (u16) state->z;
                            particle->motionX = (u16) state->motionX;
                            particle->motionY = (u16) state->motionY;
                            {
                                int x = (u16)particle->motionX;
                                int z = (u16)state->motionZ;
                                particle->frame = 0;
                                particle->motionX = x + 0x800;
                                particle->kind = 3;
                                particle->motionZ = z;
                            }
                        }
                    }
                    particle = func_800CE610(D_800F33E0->end);
                    if (particle != 0) {
                        random = rand();
                        historyIndexX = state->index;
                        particle->x = (u16) *(s16 *)(D_80195F1C + (((historyIndexX * 0x11) + (random & 3)) * 8));
                        random = rand();
                        historyIndexY = state->index;
                        particle->y = (u16) *(s16 *)(D_80195F1E + (((historyIndexY * 0x11) + (random & 3)) * 8));
                        random = rand();
                        historyIndexZ = state->index;
                        particle->z = (u16) *(s16 *)(D_80195F20 + (((historyIndexZ * 0x11) + (random & 3)) * 8));
                        particle->x = (u16) (particle->x - 0x40 + (rand() & 0x7F));
                        particle->y = (u16) (particle->y - 0x40 + (rand() & 0x7F));
                        particle->z = (u16) (particle->z - 0x40 + (rand() & 0x7F));
                        randomVelocityX = rand();
                        particle->motionX = (s16) (((randomVelocityX % 60)) - 0x1E);
                        randomVelocityY = rand();
                        particle->motionY = (s16) (((randomVelocityY % 60)) - 0x1E);
                        randomVelocityZ = rand();
                        particle->kind = 2;
                        particle->frame = 0;
                        particle->motionZ = (s16) (((randomVelocityZ % 60)) - 30);
                    }
                }
                floorHeight = &g_RoomFloorY->y;
                if ((state->y >= *floorHeight) || (((D_800E27EC < 0x51) == 0))) {

                    if (state->ending == 0) {
                        particle = func_800CE610(D_800F33E0->end);
                        if (particle != 0) {
                            particle->x = (u16) state->x;
                            particle->y = (u16) state->y;
                            particle->z = (u16) state->z;
                            impactY = *floorHeight;
                            particle->kind = 5;
                            particle->frame = 0;
                            particle->y = (u16) impactY;
                        }
                        state->ending = 1;
                        return 0;
                    }
                }
            }

            return 0;
        case 2:
            {
                register unsigned frame asm("$3") = *(volatile u16 *)&state->frame;
                register int vx asm("$6") = (s16)state->motionX;
                register unsigned coordinate asm("$2");
                register unsigned vxBits asm("$5");
                int dampX;
                unsigned z;
                unsigned vy;
                unsigned vz;
                int dampZ;
                coordinate = *(volatile u16 *)&state->x;
                vxBits = *(volatile u16 *)&state->motionX;
                asm volatile("" : :);
                dampX = vx * 63;
                state->x = coordinate + vxBits;
                coordinate = *(volatile u16 *)&state->y;
                vy = *(volatile u16 *)&state->motionY;
                state->frame = frame + 1;
                z = (u16)state->z;
                vz = (u16)state->motionZ;
                state->y = coordinate + vy;
                state->z = z + vz;
                state->motionX = dampX / 64;
                dampZ = (s16)state->motionZ * 63;
                if (dampZ < 0) dampZ += 63;
                vy = (u16)state->motionY;
                state->motionZ = dampZ >> 6;
                { int positionY;
                positionY = state->y;
                state->motionY = vy;

                if (positionY >= g_RoomFloorY->y) {
                    int signedY = (s16)vy;
                    state->motionY = -signedY;
                }
            }
                }
            alive = state->frame < 0x10;
            if (alive == 0) {
                return 1;
            }
            break;
        case 3:
            nextModelFrame = (u16) state->frame + 1;
            state->frame = nextModelFrame;
            asm("" : "=r"(nextModelFrame) : "0"(nextModelFrame));
            alive = (s16)nextModelFrame < 0xA;
            if (alive == 0) {
                return 1;
            }
            break;
        case 4:
            nextSpriteFrame = (u16) state->frame + 1;
            state->frame = nextSpriteFrame;
            asm("" : "=r"(nextSpriteFrame) : "0"(nextSpriteFrame));
            alive = (s16)nextSpriteFrame < 0x10;
            if (alive == 0) {
                return 1;
            }
            break;
        case 5:
            state->frame = (s16) ((u16) state->frame + 1);
            if ((func_800C6B90(state, 0xB4) != 0) && (state->frame < 0x10) && (D_800E2368->flags != 0) && ((D_800F32D0->actor->state->core_flags & 0x3F000000) == 0x01000000)) {
                impactPlayerState = g_PlayerEntity->state;
                impactPlayerState->flags = (s32) (impactPlayerState->flags | 0x4000);
                impactActorState = D_800F32D0->actor->state;
                impactActorState->core_flags = (impactActorState->core_flags & 0xC0FFFFFF) | 0x19000000;
                impactActorFlags = D_800F32D0->actor->state;
                impactActorFlags->core_flags |= 0x80000000;
            }
            alive = state->frame < 0x18;
checkLifetime:

            if (alive == 0) {
                return 1;
            }

            break;
        }
        break;
    case 2:
        renderKind = state->kind - 1;
        switch (renderKind) {
        case 0:
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            trailClutY = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                trailClutY += 4;
            }
            func_800D3114(D_80195EFC[state->index].point, 0x10, 0xF0, 0, 0x60, 0xFF, 0xF, (s32) D_800E2850[D_800E11EA], GetClut(0x30, trailClutY) & 0xFFFF, 0x80, (RenderColor *)&color.r, (RenderColor *)&darkColor.r, 1);
            fade = ((D_800E27EC & 1) << 5) + 0x60;
            func_800D004C((GteShortVector *)state, 0x190, 0x190, 8, 0, 0x1000, 0x1000, (RenderColor *)&color.r, 0, fade, 1);
            historySample = D_80195EFC[0].point + (rand() & 7);
            func_800D004C((void *)((unsigned)(state->index * 0x88) + (unsigned)historySample), 0x1F4, 0x1F4, 6, 0, 0x1000, 0x1000, (RenderColor *)&color.r, 0, (u32)fade >> 1, 1);
            return 0;
        case 1:
            firstFade = rcos(state->frame << 6) / 32;
            renderScale = rcos(state->frame << 6);
            asm volatile("" : : "m"(D_800F3368));
            params = &D_800F3368;
            spriteExtent = 0x10;
            spriteClutY = D_800E1208;
            asm("" : "=r"(params) : "0"(params), "r"(spriteClutY), "r"(spriteExtent));
            palettes = (RoomM256TrailPalettes *)&D_800E11E8;
            asm("" : "=r"(palettes) : "0"(palettes));
            params->parameter00 = spriteExtent;
            spritePalette = palettes->first;
            D_800F3368.parameter02 = 1;
            D_800F3368.extent_x = spriteExtent;
            D_800F3368.extent_y = spriteExtent;

            renderPalette = D_800E2850[spritePalette];
            D_800F336C = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = renderPalette;
            {
                register RoomM256TrailEffect *callState asm("$4");
                register void *callRotation asm("$5");
                register int callScaleX asm("$6");
                register int callScaleY asm("$7");
                int drawClut;
                int drawOffset;

                drawClut = GetClut(0x20, spriteClutY) & 0xFFFF;
                callState = state;
                callRotation = 0;
                callScaleX = renderScale;
                callScaleY = renderScale;
                drawOffset = (s16)((const RenderEffectParameters *)params)->parameter02 * 2 + 0xD8;
                /* Matching debt: consume CLUT and page in $2, then bound both
                 * outgoing stores before the texture-offset load. Empty ASM
                 * only; $4..$7 preserve the ready register arguments. */
                func_800CEE20((GteShortVector *)callState, callRotation, callScaleX, callScaleY,
                              drawOffset,
                    ({
                        register int clutWord asm("$2") = drawClut;
                        clutWord;
                    }),
                    ({
                        register int modeWord asm("$2") = 1;
                        modeWord;
                    }), firstFade,
                    (RenderColor *)(({
                        asm("" : : "r"(callState), "r"(callRotation),
                            "r"(callScaleX), "r"(callScaleY) : "$21", "$2");
                        &color.r;
                    })));
            }
            renderPalette = D_800E2850[palettes->second];

            D_800F336C = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = renderPalette;
            break;
        case 2:
            renderScale = rsin(((state->frame << 0xA) / 10)) / 4;
            fade = (rsin((state->frame << 0xB) / 10) / 48);
            gte_ldrotmatrix(D_800BCFA4.value);
            gte_ldtransmatrix(D_800BCFA4.value);
            {
                int pageDepth = 0;
                int pageBlend = 1;
                int pageX = 0;
            paletteIndex = &D_800E11EA;
            renderPalette = D_800E2850[*paletteIndex];

            D_800F336C = 3;
            D_800F3368.parameter06 = 0;
            D_800F3368.tpage = renderPalette;
            tpage = ((int (*)(int, int, int, int))GetTPage)(pageDepth, pageBlend, pageX, 0);
            }
            modelPage = (*(u16 *)((char *)D_800E2850 + ((unsigned)*paletteIndex << 1)) | tpage) & 0xFFFF;
            modelClutY = D_800E1204[D_800F336C];
            if (D_800F336C == 4) {
                if (D_800F3428 != 0) {
                    modelClutY += 4;
                }
            }
            FieldFx_ModelSetTexture(modelPage, GetClut(0x70, modelClutY) & 0xFFFF);
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&state->motionX, &transform);
            transform.t[0] = (s32) state->x;
            transform.t[1] = (s32) state->y;
            translationZ = state->z;
            scale.x = renderScale;
            scale.y = renderScale;
            scale.z = 0x2AA;
            transform.t[2] = translationZ;
            ScaleMatrix(&transform, &scale.x);
            FieldFx_ModelSaveColors((char *)D_80195EF8);
            FieldFx_ModelSetColors((char *)D_80195EF8, darkColor.r, darkColor.g, darkColor.b);
            FieldFx_ModelScaleColors((char *)D_80195EF8, fade & 0xFFFF);
            FieldFx_ModelDraw((union FieldTexturedModel *)D_80195EF8, &transform);
            FieldFx_ModelRestoreColors((char *)D_80195EF8);
            return 0;
        case 3:
            pulseSine = rsin(state->frame << 6);
            renderScale = ((s32) ((pulseSine >> 0x1F) + pulseSine) >> 1) + 0x1000;
            pulseCosine = rcos(state->frame << 6);
            fade = pulseCosine >> 5;
            if (pulseCosine < 0) {
                fade = (s32) (pulseCosine + 0x1F) >> 5;
            }
            pulseExtent = 0x40;
            D_800F3368.parameter00 = pulseExtent;
            D_800F3368.extent_x = pulseExtent;
            D_800F3368.extent_y = pulseExtent;
            D_800F3368.parameter02 = 4;
            D_800F3368.depth = 0x18;
            pulseClutY = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                pulseClutY += 4;
            }
            func_800CEE20((GteShortVector *)state, 0, renderScale, renderScale, 0x7C, GetClut(0x60, pulseClutY) & 0xFFFF, 1, fade, 0);
            D_800F3368.depth = 8;
            break;
        case 4:
            renderScale = rsin(state->frame << 6) + 0x1000;
            rsin((state->frame << 11) / 24);
            D_800F3368.parameter00 = 0x20;
            D_800F3368.parameter02 = 2;
            D_800F3368.extent_x = 0x20;
            D_800F3368.extent_y = 0x20;
            impactClutY = D_800E1204[D_800F336C];
            if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                impactClutY += 4;
            }
            clut = GetClut(0x10, impactClutY) & 0xFFFF;
            tripledScale = renderScale * 3;
            impactScale = (s32) (tripledScale + (tripledScale >> 0x1F)) >> 1;
            spriteFrame = (s16 *)&D_800F3368.parameter02;
            func_800CEE20((GteShortVector *)state, &spriteRotation, impactScale, impactScale, (*spriteFrame * (s16) (state->frame / 3)) + 0x20, clut, 1, 0x80, 0);
            impactFrame = state->frame;
            if (impactFrame < 0xD) {
                renderScale = rsin(((impactFrame << 10) / 12)) * 4;
                impactCosine = rcos(((state->frame << 10) / 12));
                fade = impactCosine >> 5;
                if (impactCosine < 0) {
                    fade = (s32) (impactCosine + 0x1F) >> 5;
                }
                D_800F3368.parameter00 = 0x20;
                *spriteFrame = 2;
                D_800F3368.extent_x = 0x20;
                D_800F3368.extent_y = 0x20;
                D_800F3368.parameter0A = 5;
                ringClutY = D_800E1204[D_800F336C];
                if ((D_800F336C == 4) && (D_800F3428 != 0)) {
                    ringClutY += 4;
                }
                func_800CEE20((GteShortVector *)state, &spriteRotation, renderScale, renderScale, 0x92, GetClut(0x80, ringClutY) & 0xFFFF, 1, fade, (RenderColor *)&darkColor.r);
                D_800F3368.parameter0A = 0;
            }
            return 0;
        }
        break;
    default:
        break;
    }
    return 0;
}

int func_80195320(int mode, u16 *out) {
    FieldActor *actor;
    FieldActorState *state;
    u8 *action;
    /* Keep the return register available for the retail mode dispatch. */
    register int ret asm("$2");
    RoomM256TrailEffect *task;
    u16 value;

    switch (mode) {
    case 0:

        *out = (u16)D_800E2368->variables[0];
        D_80195EF8 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5887704);
        FieldFx_ModelScrollUv((union FieldTexturedModel *)D_80195EF8, 0, 0);
        if (D_800E2368->flags) {
            actor = (FieldActor *)D_800F32D0->actor;
            if (actor) {
                state = actor->state;
                if (state) {
                    action = state->action_state;
                    if (*action == 1) *action = 2;
                }
            }
        }
        return func_800CE560(D_800F33E0->end, 0x18, 0x20,
                               func_801940B0);

        case 1:
    if (D_800E27EC == mode) {
        task = func_800CE610(D_800F33E0->end);
        if (task) {
            value = *out;
            task->kind = 0;
            task->frame = 0;
            task->index = value;
        }
        func_800D3F64(0x5A6, func_800D3FD8());
        func_800D3F64(0x5BE, 0x80);
    }
    ret = D_800E27EC < 8;
    if (ret) return 0;
    return 2;

    case 2:
    {
        unsigned palette_index = D_800E11EA;
        u16 palette;
        palette = D_800E2850[palette_index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 8;
        D_800F3368.tpage = palette;
    }
    }
    return 0;
}

int func_8019552C(int mode, RoomModelPulseParticle *particle) {
    GteShortVector position;
    GteShortVector color;
    int scale;
    int palette;
    int kind;
    u16 clut;

    switch (mode) {
    case 1:
        particle->frame++;
        particle->offset += 8;
        if ((s16)particle->frame >= 16) return 1;
        break;
    case 2:
        scale = particle->scale * rcos((s16)particle->frame << 6) / 4096;
        func_800CF844(&D_801960A0, &position, scale, &D_80196098,
                      scale, (s16)particle->offset);
        func_800CF3AC(D_80195E64, &color, 0x30 - (scale * 48) / 1024);
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428 != 0) palette += 4;
        clut = GetClut(0x20, palette);
        func_800CEE20(&position, 0, 0x1000, 0x1000,
                       (s16)D_800F3368.parameter02 * 2 + 0xD8, clut, 3,
                       D_80196094, (RenderColor *)&color);
        gte_ldrotmatrix(D_800BCFA4.value);
        gte_ldtransmatrix(D_800BCFA4.value);
        func_800D1DEC(&position, &color, D_80196094, 1);
        break;
    }
    return 0;
}

/* Controller: loads the model asset, seeds sixteen ring particles on frame
 * 1, then draws the slot-2 ring sprite and, from frame 4, the model scaled by
 * cos and brightened by sin of the elapsed time.
 *
 * Mode 2 keeps the ModelBurstController preamble (spin record, brightness
 * scaled by 2/3, a cos sample) whose results this effect never reads: the
 * spin stores and the cos call survive in retail, and the brightness test is
 * the branch that splits the scheduling block before the /40 division (its
 * empty jump is only removed after scheduling). */
int func_80195728(int mode, s16 *state) {
    GteRotation rotation = D_8018F258;
    GteShortVector position;
    GteRotation spin;
    RenderColor color;
    RoomModelPulseMatrix matrix;
    RoomModelPulseScale scale;
    RoomModelPulseParticle *child;
    void *slot;
    u16 *tpages;
    int size;
    int brightness;
    int i;

    switch (mode) {
    case 0:
        *state = 0;
        D_801960A8 = Asset_FindTable08ByU32Key(D_800B0E64[0], 0xC5487704);
        FieldFx_ModelScrollUv(D_801960A8, 0, 0);
        return func_800CE560(ANIM_POOL_AS(RoomSparkChannel)->pool, 8, 0x10,
                             (FieldFxCallbackListCallback)func_8019552C);
    case 1:
        if (*state < 0x80) *state += 4;
        if (D_800E27EC == 1) {
            for (i = 0; i < 16; i++) {
                slot = func_800CE610(ANIM_POOL_AS(RoomSparkChannel)->pool);
                child = slot;
                if (child) {
                    child->frame = 0;
                    child->scale = (rand() & 0x1FF) + 0x200;
                    child->offset = rand();
                }
            }
        }
        if (D_800E27EC < 0x10) break;
        return 1;
    case 2:
        {
            D_800F3368.parameter00 = 0x40;
            D_800F3368.parameter02 = 4;
            D_800F3368.extent_x = 0x40;
            D_800F3368.extent_y = 0x40;
            D_800F3368.tpage = D_800E2850[D_800E11FA];
            D_800F3368.palette = 3;
            D_800F3368.parameter06 = 1;
            D_800F3368.parameter0A = 0;
            D_800F3368.depth = 0x18;
        }
        FieldFx_TransformMatrixPoint(ANIM_OWNER_AS(RoomSparkChannel)->pool, 2, &rotation, &position);
        spin.x = 0;
        spin.y = 0;
        spin.flags = 0;
        spin.z = -D_800E27EC << 6;
        /* Copied from the burst template; the value is never read here, but
         * the emptied branch still splits retail's schedule. */
        brightness = *state;
        if (D_800E27EC != 0) brightness = brightness * 2 / 3;
        rcos((D_800E27EC << 10) / 40);
        func_800CF3AC(D_80195E8C, &color, D_800E27EC * 3);
        func_800D004C(&position, 0xA0, 0xA0, 8, 0, 0x1000, 0x1000, &color, 0,
                      0x80, 1);
        D_80196094 = *state;
        D_801960A0.x = position.x;
        D_801960A0.y = position.y;
        D_801960A0.z = position.z;
        func_800CE9D4((struct RoomFxTransformOwner *)ANIM_OWNER_AS(RoomSparkChannel)->pool, 2,
                      &D_80196098);
        D_80196098.x += 0x400;
        if (D_800E27EC >= 4) {
            int angle;

            i = D_800E27EC - 4;
            angle = (i << 10) / 12;
            size = rcos(angle);
            brightness = rsin(angle) / 32;
            {
                RenderMatrixSlot *matrixSlot = &D_800BCFA4;
                gte_ldrotmatrix(matrixSlot->value);
                gte_ldtransmatrix(matrixSlot->value);
            }
            {
                u16 clut;
                int kind;
                int palette;
                int page;
                int tpage;
                tpages = D_800E2850;
                tpage = tpages[D_800E11EA];
                D_800F3368.palette = 3;
                D_800F3368.parameter06 = 0;
                D_800F3368.tpage = tpage;
                page = (tpages[D_800E11EA] | ((int (*)(int, int, int, int))GetTPage)(0, 1, 0, 0)) & 0xFFFF;
                kind = D_800F3368.palette;
                palette = D_800E1204[kind];
                if (kind == 4 && D_800F3428 != 0) palette += 4;
                clut = GetClut(0x50, palette);
                FieldFx_ModelSetTexture(page, clut);
            }
            FieldFx_ModelSetSemiTrans(1);
            RotMatrixYXZ(&D_80196098, &matrix);
            matrix.t[0] = position.x;
            matrix.t[1] = position.y;
            matrix.t[2] = position.z;
            scale.x = size;
            scale.y = size;
            scale.z = size;
            ScaleMatrix(&matrix, &scale);
            FieldFx_ModelSaveColors(D_801960A8);
            FieldFx_ModelScaleColors(D_801960A8, (u16)brightness);
            FieldFx_ModelDraw(D_801960A8, (GteMatrix *)&matrix);
            FieldFx_ModelRestoreColors(D_801960A8);
        }
        D_800F3368.parameter00 = 0x10;
        D_800F3368.parameter02 = 1;
        D_800F3368.extent_x = 0x10;
        D_800F3368.extent_y = 0x10;
        D_80196098.x += 0x600;
        {
            int tpage = D_800E2850[D_800E11E8];
            D_800F3368.palette = 2;
            D_800F3368.parameter06 = 0;
            D_800F3368.depth = 0xC;
            D_800F3368.tpage = tpage;
        }
        break;
    }
    return 0;
}

void *func_80195CC8(void) {
    return D_80195EEC;
}

void *func_80195CD8(void *arg0, int value) {
    if (value < 0x200) {
        value = 0x200;
    }

    D_80195EF0 = value;
    return D_80195EF4;
}
