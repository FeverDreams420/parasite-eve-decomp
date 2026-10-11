#ifndef PE1_ROOM_M256_TRAIL_H
#define PE1_ROOM_M256_TRAIL_H

#include "pe1/field_sprite_params.h"
#include "pe1/field_actor.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_anim.h"
#include "pe1/room_floor.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_engine_calls.h"

/* Shared 24-byte pool record: steering trail, damped sparks, model burst,
 * expanding sprite and floor impact. Modes 1 and 2 update and draw it. */
typedef struct RoomM256TrailEffect {
    s16 x, y, z, index;               /* 0x00 */
    s16 motionX, motionY, motionZ;     /* 0x08: angles or particle velocity */
    s16 ending;                       /* 0x0E */
    s16 speed, angle, kind, frame;     /* 0x10 */
} RoomM256TrailEffect;
PE1_STATIC_ASSERT(sizeof(RoomM256TrailEffect) == 0x18, room_m256_trail_effect_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM256TrailEffect, kind) == 0x14,
                  room_m256_trail_effect_kind);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomM256TrailEffect, frame) == 0x16,
                  room_m256_trail_effect_frame);

extern RenderColor D_8018F218, D_8018F21C;
extern GteRotation D_8018F220,D_8018F1CC;
extern char D_80195EFC[],D_80195F1C[],D_80195F1E[],D_80195F20[];
extern s32 D_80195EF0, D_80195EF8;
extern u16 D_800E11EA, D_800E1208;
typedef struct RoomM256TrailPalettes { u16 first, second; } RoomM256TrailPalettes;
extern RoomM256TrailPalettes D_800E11E8;
int func_80071A54(void);
#include "pe1/gte_types.h"
#include "pe1/player_entity.h"
void func_800C6EC0(int,int);
void func_800D3AFC(void *,int,void *,int);

int func_801940B0(int mode, void *effect);

#endif
