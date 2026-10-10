#ifndef PE1_SCENE_E19_BLAST_H
#define PE1_SCENE_E19_BLAST_H

#include "pe1/render_object.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/gte.h"
#include "pe1/room_floor.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_owner_actor.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_anim_pool.h"
#include "pe1/field_texture_tables.h"
#include "pe1/scene_archive.h"

/* Scene e19 blast sequence: loads five shell models, then runs a five-state
 * charge/burst timeline at an actor position, spawning rising sparks while
 * the shells, glow rings and flares expand and fade. */
typedef struct SceneE19Blast {
    /* 0x00 */ GteShortVector position;
    /* 0x08 */ GteShortVector target;
    /* 0x10 */ GteShortVector origin;
    /* 0x18 */ s16 state;
    /* 0x1A */ s16 timer;
} SceneE19Blast;

typedef struct SceneE19BlastSpark {
    /* 0x00 */ u16 x, y, z;
    /* 0x06 */ s16 pad;
    /* 0x08 */ s16 state;
    /* 0x0A */ s16 timer;
} SceneE19BlastSpark;

typedef struct SceneE19BlastActor {
    u32 flags;
} SceneE19BlastActor;

typedef struct SceneE19BlastChannel {
    s32 reserved[2];
    SceneE19BlastActor **pool; /* 0x08 */
} SceneE19BlastChannel;

typedef struct SceneE19BlastPlayer {
    u8 reserved[0x4C];
    s32 flags; /* 0x4C */
} SceneE19BlastPlayer;

typedef struct SceneE19BlastScript {
    u8 reserved[0xD];
    u8 active; /* 0x0D */
} SceneE19BlastScript;

extern SceneE19BlastPlayer **D_8009D254;
extern u16 D_800E11EA[];
extern s32 D_8019B668;
extern void *D_8019B680, *D_8019B684, *D_8019B688, *D_8019B68C, *D_8019B690;

int func_80192F9C(int mode, SceneE19Blast *blast);
int func_80192E08(int mode, void *spark);
void *func_8006E498(void *channel, u32 key);
void func_8006DF50(void *channel, int sound, int time, int pan, int volume);
int func_80071A54(void);
int func_80077A64(int tp, int abr, int x, int y);
u16 func_80077AA4(int x, int y);
int rsin(int angle);
int rcos(int angle);
#include "pe1/gte_types.h"
int func_800C6B90(GteShortVector *position, int range);
void func_800C6EC0(int page, int clut);
void func_800D1D24(int mode, int count, int value);

#endif
