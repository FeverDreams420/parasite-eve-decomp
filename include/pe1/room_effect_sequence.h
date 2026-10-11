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

/* The 0xC used bytes of each 0x10-byte sprite preset. The third
 * preset's trailing word is a separate model resource (D_80192C34). */
typedef struct RoomEffectSequenceSprite {
    u8 r, g, b, reserved03;
    u8 cell, clut, flip, reserved07;
    s16 offset;
    s16 depth;
} RoomEffectSequenceSprite;
PE1_STATIC_ASSERT(sizeof(RoomEffectSequenceSprite) == 0xC, effect_sequence_sprite_size);
PE1_STATIC_ASSERT(PE1_OFFSETOF(RoomEffectSequenceSprite, depth) == 0xA, effect_sequence_sprite_depth);
extern RoomEffectSequenceSprite D_80192C08, D_80192C18, D_80192C28, D_80192C38;
extern void *D_80192C34;

extern int *func_800C2B28(int index);
extern int *func_800C2B10(int index);
/* Per-command handler table and state size table for func_800C2B90. */
extern int (*D_80192B5C[])(void *, void *, void *);
extern u16 D_80192BBC[];
extern char D_80192BFC;
extern char D_80192BF0;

/* The module's class methods (slots 3 to 5). */
int RoomFx_EffectSequenceRegister(void *o);
int RoomFx_EffectSequenceStart(void *o);
s32 RoomFx_EffectSequenceNop5(void *o);

#endif
