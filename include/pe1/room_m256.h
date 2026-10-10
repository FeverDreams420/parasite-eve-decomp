#ifndef PE1_ROOM_M256_H
#define PE1_ROOM_M256_H

#include "pe1/field_sprite_params.h"
#include "pe1/room_m256_trail.h"
#include "pe1/room_model_pulse_particle.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_owner_actor.h"
#include "pe1/scene_archive.h"

/* ABI declarations used by the room_m256 effect dispatcher. */
extern u16 D_800E11EA;
extern int D_80195EF8;
extern int func_801940B0(int mode, void *state);

extern int func_8006E498(int, int);

int func_80195320(int mode, u16 *out);

extern char D_801960A0[];
extern char D_80196098[];
extern char D_80195E64[];
extern s16 D_80196094;

int rcos(int);
int func_80077AA4(int, int);
void func_800CF844(void *, void *, int, void *, int, int);

#endif
