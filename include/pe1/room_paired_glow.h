#ifndef PE1_ROOM_PAIRED_GLOW_H
#define PE1_ROOM_PAIRED_GLOW_H

#include "common.h"
#include "pe1/room_fx.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_model_draw.h"
#include "pe1/scene_archive.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_libc.h"
#include "pe1/akao_send.h"
#include "pe1/asset_find.h"

/* The paired glow (src/overlays/room_lib/RoomLib_PairedGlow.c). */

/* Per-room data: the glow sprite's record and packet, and the colour table
 * the emitters step through. */
extern RoomFxSpritePacket g_RoomPairedGlowSprite;
extern void *g_RoomPairedGlowPacket;
extern unsigned char g_RoomPairedGlowColorTable[];


int *FieldFx_RegPtr(int index);
void func_800C4E50(void *params);
void func_800C6EC0(int arg0, int arg1);

#endif
