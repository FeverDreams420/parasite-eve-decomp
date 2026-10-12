/*
 * Timed spawns: the sprite callback and the controller that spawns it every
 * sixth frame and flashes the scene on frame 7. The two functions follow
 * each other in eight hospital and Chrysler rooms; the unit's rodata is the
 * callback's seed initialiser.
 */
#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/fieldfx_callback_list.h"
#include "pe1/gte.h"
#include "pe1/room_fx.h"
#include "pe1/room_floor.h"
#include "pe1/render_matrix_slot.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_owner_actor.h"
#include "pe1/fieldfx_task_pool.h"
#include "pe1/scene_archive.h"
#include "pe1/gte_types.h"
#include "pe1/akao.h"
#include "pe1/player_entity.h"
#include "room_lib.h"

int RoomLib_UpdateTimedRender(int mode, RoomFxTimedRenderState *state) {
    RoomFxSeed8 work;
    RoomFxSeed8 seed = {{0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00}};
    RoomFxSeed8 *seedPtr;
    RoomSpriteMatrix **matrixSlot;
    int angle;

    seedPtr = &seed;
    switch (mode) {
    case 1:
        if (state->disabled != 0) {
            return 0;
        }
        state->frame++;
        if ((short)state->frame < 24) {
            break;
        }
        return 1;
    case 2:
        if (state->disabled != 0) {
            return 0;
        }
        matrixSlot = &D_800BCFA4.value;
        gte_ldrotmatrix(*matrixSlot);
        gte_ldtransmatrix(*matrixSlot);
        func_800CF3AC(&RoomLib_TimedRenderTable, &work,
                      (short)state->frame);
        angle = rsin((D_800E27EC << 10) / 24);
        func_800D0728((GteShortVector *)state, 0x578, 0x640, 0x14, (GteRotation *)seedPtr, angle, angle, 0,
                      (struct RenderColor *)&work, 0x80, 1);
        return 0;
    }
ret0:
    return 0;
ret1:
    return 1;
disabled:
    return 0;
}

typedef struct RoomTimedSpawnPosition {
    u16 x;
    u16 y;
    u16 z;
} RoomTimedSpawnPosition;

typedef struct RoomTimedSpawnState {
    s16 x;
    s16 y;
    RoomTimedSpawnPosition position;  /* 0x04 */
} RoomTimedSpawnState;

typedef struct RoomTimedSpawnEventState {
    u8 reserved[0xD];
    u8 active;                        /* 0x0D */
} RoomTimedSpawnEventState;

typedef struct RoomTimedSpawnNode {
    u32 flags;                        /* 0x00: 0x3F000000 owner bits */
    u8 reserved04[0x14];
    u8 *status;                       /* 0x18 */
} RoomTimedSpawnNode;

/* The animation owner: its link, whose first word is the render node. */
typedef struct RoomTimedSpawnOwner {
    u8 reserved[0x8];
    RoomTimedSpawnNode **link;        /* 0x08 */
} RoomTimedSpawnOwner;

typedef struct RoomTimedSpawnEngine {
    u8 reserved[0x4C];
    u32 flags;                        /* 0x4C */
} RoomTimedSpawnEngine;

int RoomLib_ControlTimedSpawn(int mode, RoomTimedSpawnState *state) {
    RoomFxTimedRenderState *actor;
    int tick;
    int value;
    u16 ground;

    switch (mode) {
    case 0:

        func_800CE870(ANIM_OWNER_AS(RoomTimedSpawnOwner)->link, 0, (s16 *)&state->position);
        state->x = 0;
        state->y = 0;

        if (FX_TASK_AS(RoomTimedSpawnEventState)->active != 0) {
            RoomTimedSpawnNode **p = ANIM_OWNER_AS(RoomTimedSpawnOwner)->link;
            if (p != 0 && *p != 0) {
                u8 *status = (*p)->status;
                if (*status == 1) {
                    *status = 2;
                }
            }
        }

        if (D_800B0E64[0] != 0) {
            value = func_800D3FD8();
            Akao_SendTableCommand(D_800B0E64[0], 0x588, value, 0x80, 0x7F);
        }
        return func_800CE560(((void **)ANIM_POOL_AS(void))[2], 0xC, 8,
                             (FieldFxCallbackListCallback)RoomLib_UpdateTimedRender);

        case 1:
    tick = D_800E27EC;
    if (tick < 0x20 && tick % 6 == 0) {
        actor = func_800CE610(((void **)ANIM_POOL_AS(void))[2]);
        if (actor != 0) {
            actor->x = state->position.x;
            actor->y = state->position.y;
            actor->z = state->position.z;
            ground = g_RoomFloorY->y;
            actor->disabled = 0;
            actor->frame = 0;
            actor->y = ground;
        }
    }

    if (D_800E27EC == 7 && FX_TASK_AS(RoomTimedSpawnEventState)->active != 0) {
        RoomTimedSpawnOwner *root;
        root = ANIM_OWNER_AS(RoomTimedSpawnOwner);
        if (((*root->link)->flags & 0x3F000000) ==
            0x01000000) {
            RoomTimedSpawnNode *object;
            RoomTimedSpawnEngine *engine = *(RoomTimedSpawnEngine **)g_PlayerEntity;
            engine->flags |= 0x4000;

            object = *root->link;
            object->flags = (object->flags & 0xC0FFFFFF) | 0x21000000;

            object = *root->link;
            object->flags |= 0x80000000;
        }
    }
        if (D_800E27EC >= 8) return 2;
    break;
    case 2:
 {
        int index = D_800E11EA;
        u16 tpage;
        D_800F3368.parameter00 = 0x20;
        D_800F3368.parameter02 = 2;
        D_800F3368.extent_x = 0x20;
        D_800F3368.extent_y = 0x20;
        tpage = D_800E2850[index];
        D_800F3368.palette = 3;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = 0;
        D_800F3368.tpage = tpage;
    }
    
    }
    return 0;
}
