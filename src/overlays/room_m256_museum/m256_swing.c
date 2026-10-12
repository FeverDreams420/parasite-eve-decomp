/* MASPSX_FLAGS: --expand-div */

#include "common.h"
#include "pe1/room_m256_swing.h"
#include "pe1/room_m256_drop.h"

#include "m256_swing_externs.h"

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
    if (func_800C6B90(drop, radius) && drop->hazard && ANIM_TASK_AS(RoomM256DropEvent)->active) {  \
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
            drop->x += (func_80071A54() & 0x1F) - 0x10;
            drop->y += (func_80071A54() & 0x1F) - 0x10;
            drop->z += (func_80071A54() & 0x1F) - 0x10;
            if (drop->y >= g_RoomFloorY->y - 0x10 || drop->timer >= 0x18) {
                drop->state = 1;
                drop->y = g_RoomFloorY->y - 0x10;
                drop->vy = 0;
            }
            if (drop->timer & 1) {
                if ((func_80071A54() & 7) == 0) {
                    child = func_800CE610(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = drop->vx / 2 + (func_80071A54() & 0x1F) - 0x10;
                        child->vy = drop->vy / 2 + (func_80071A54() & 0x1F) - 0x10;
                        child->vz = drop->vz / 2 + (func_80071A54() & 0x1F) - 0x10;
                        child->state = 2;
                        child->timer = 0;
                    }
                }
                if ((func_80071A54() & 7) == 1) {
                    child = func_800CE610(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = (func_80071A54() & 0x1F) - 0x10;
                        child->vy = (func_80071A54() & 0x1F) - 0x10;
                        child->vz = (func_80071A54() & 0x1F) - 0x10;
                        child->state = 3;
                        child->timer = 0;
                    }
                }
                if ((func_80071A54() & 0xF) == 2) {
                    child = func_800CE610(ANIM_POOL_AS(RoomM256DropSpawnChannel)->pool);
                    if (child) {
                        child->x = drop->x;
                        child->y = drop->y;
                        child->z = drop->z;
                        child->vx = (func_80071A54() & 0x1F) - 0x10;
                        child->vy = (func_80071A54() & 0x1F) - 0x18;
                        child->vz = (func_80071A54() & 0x1F) - 0x10;
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
            drop->x += (func_80071A54() & 0x1F) - 0x10;
            drop->z += (func_80071A54() & 0x1F) - 0x10;
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
            if (func_80071A54() & 3) return 0;
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
