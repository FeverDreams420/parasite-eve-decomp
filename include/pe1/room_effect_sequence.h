#ifndef PE1_ROOM_EFFECT_SEQUENCE_H
#define PE1_ROOM_EFFECT_SEQUENCE_H

#include "common.h"
#include "pe1/field_sprite_state.h"
#include "pe1/room_floor.h"
#include "pe1/field_script_context.h"
#include "pe1/scene_archive.h"
#include "pe1/psyq_libc.h"
#include "pe1/asset_find.h"

/* The effect sequence (src/overlays/room_lib/RoomFx_EffectSequence.c):
 * the room data it fills and reads (the same addresses in all five rooms
 * that link it) and the main code it calls. */

extern u8 D_80192C08;
extern u8 D_80192C09;
extern u8 D_80192C0A;
extern u8 D_80192C0C;
extern u8 D_80192C0D;
extern u8 D_80192C0E;
extern s16 D_80192C10;
extern u8 D_80192C18;
extern u8 D_80192C19;
extern u8 D_80192C1A;
extern u8 D_80192C1C;
extern u8 D_80192C1D;
extern u8 D_80192C1E;
extern s16 D_80192C20;
extern s16 D_80192C22;
extern u8 D_80192C28;
extern u8 D_80192C29;
extern u8 D_80192C2A;
extern u8 D_80192C2C;
extern u8 D_80192C2D;
extern u8 D_80192C2E;
extern s16 D_80192C30;
extern s16 D_80192C32;
extern void *D_80192C34;
/* The sparkle sprite preset consumed by the sequence draw passes. */
typedef struct RoomEffectSequenceSprite {
    u8 r, g, b, reserved03;
    u8 cell, clut, flip, reserved07;
    s16 offset;
    s16 depth;
    u8 reserved0C[4];
} RoomEffectSequenceSprite;
PE1_STATIC_ASSERT(sizeof(RoomEffectSequenceSprite) == 0x10, effect_sequence_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomEffectSequenceSprite, depth) == 0xA, effect_sequence_sprite_depth);
extern RoomEffectSequenceSprite D_80192C38;

extern int *func_800C2B28(int index);
extern int *func_800C2B10(int index);
/* Per-command handler table and state size table for func_800C2B90. */
extern int (*D_80192B5C[])(void *, void *, void *);
extern u16 D_80192BBC[];
extern volatile s16 D_80192C12;
extern char D_80192BFC;
extern char D_80192BF0;
extern short D_80192C30;

/* The module's class methods (slots 3 to 5). */
int RoomFx_EffectSequenceRegister(void *o);
int RoomFx_EffectSequenceStart(void *o);
s32 RoomFx_EffectSequenceNop5(void *o);

#endif
