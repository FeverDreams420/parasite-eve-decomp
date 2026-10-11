#ifndef PE1_SCENE_E08_LAYERED_SPRITE_H
#define PE1_SCENE_E08_LAYERED_SPRITE_H

#include "pe1/room_fx.h"
#include "pe1/fixed_point.h"

typedef struct SceneLayerPosition {
    s16 x, y, z, pad;
} SceneLayerPosition;

/* Shared init/draw prefix. The updater also accesses a later history
 * beginning at 0x86; its surrounding layout is not yet represented here. */
typedef struct SceneLayeredSpriteState {
    Pe1Fixed16_16 center[3];     /* 0x00 */
    u32 reserved0C;
    s32 velocity[3];            /* 0x10 */
    u32 reserved1C;
    RoomFxSeed8 initialSeed;    /* 0x20 */
    u8 direction;              /* 0x28 */
    u8 reserved29;
    u16 drift;                 /* 0x2A */
    s32 phase;                 /* 0x2C */
    SceneLayerPosition layers[5]; /* 0x30 */
    RoomFxSeed8 layerSeeds[5];  /* 0x58 */
} SceneLayeredSpriteState;

PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneLayeredSpriteState, velocity) == 0x10, scene_layer_velocity_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneLayeredSpriteState, initialSeed) == 0x20, scene_layer_seed_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneLayeredSpriteState, layers) == 0x30, scene_layer_position_offset);
PE1_STATIC_ASSERT(PE1_OFFSETOF(SceneLayeredSpriteState, layerSeeds) == 0x58, scene_layer_history_offset);

#endif
