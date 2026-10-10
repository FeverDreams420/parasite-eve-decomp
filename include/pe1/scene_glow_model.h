#ifndef PE1_SCENE_GLOW_MODEL_H
#define PE1_SCENE_GLOW_MODEL_H

#include "pe1/room_fx.h"
#include "pe1/field_sprite_state.h"
#include "pe1/room_floor.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_script_context.h"

/* Scene e08 glowing model: the effect model drawn at the scene anchor,
 * followed by a glow sprite that grows once the timer passes 10. */
typedef struct SceneGlowModelOwner {
    /* 0x00 */ u8 pad00[0x24];
    /* 0x24 */ int mode;
} SceneGlowModelOwner;

typedef struct SceneGlowModelTimer {
    /* 0x00 */ u8 pad00[2];
    /* 0x02 */ s16 ticks;
} SceneGlowModelTimer;

typedef struct SceneGlowModelState {
    /* 0x00 */ u8 pad00[2];
    /* 0x02 */ s16 height;
    /* 0x04 */ u8 pad04[4];
    /* 0x08 */ RoomFxSeed8 seed;
    /* 0x10 */ s16 scaleX;
    /* 0x12 */ s16 scaleY;
    /* 0x14 */ s16 scaleZ;
    /* 0x16 */ u8 pad16[2];
    /* 0x18 */ u16 depth;
} SceneGlowModelState;

typedef struct SceneGlowModelColor {
    u8 bytes[8];
} SceneGlowModelColor;

extern u8 *D_80199528;
extern int D_8019956C;
extern int D_8019957C;
extern u8 D_80198860[];
extern u8 D_801996B0[];

void func_80071A44(RoomFxVec4 *vec, int value, int size);
int func_80077A64(int arg0, int arg1, int x, int y);
int func_80077AA4(int x, int y);
void func_800C6EC0(int tpage, int clut);

#endif
