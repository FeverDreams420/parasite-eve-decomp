#ifndef PE1_ROOM_PAIRED_GLOW_H
#define PE1_ROOM_PAIRED_GLOW_H

#include "common.h"
#include "pe1/room_fx.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_model_draw.h"
#include "pe1/scene_archive.h"

/* The paired glow (src/overlays/room_lib/RoomLib_PairedGlow.c). */

/* Per-room data: the glow sprite's record and packet, and the colour table
 * the emitters step through. */
extern RoomFxSpritePacket g_RoomPairedGlowSprite;
extern void *g_RoomPairedGlowPacket;
extern unsigned char g_RoomPairedGlowColorTable[];


int *func_800C2B10(int index);
void *func_8006E498(void *owner, int id);
void func_8006DF50(void *owner, int soundId, int arg2, int volume, int pan);
void func_800C4E50(void *params);
void func_80071A44(RoomFxVec4 *vec, int value, int shift);
int func_80077A64(int arg0, int arg1, int arg2, int arg3);
int func_80077AA4(int arg0, int arg1);
void func_800C6EC0(int arg0, int arg1);

#endif
