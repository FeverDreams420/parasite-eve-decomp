#ifndef MENU_MEMCARD_LINK_H
#define MENU_MEMCARD_LINK_H

#include "pe1/gte.h"
#include "pe1/field_anim_callback_list.h"
#include "menu_memcard_pulse.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_anim_pool.h"

typedef struct MemcardLinkOffset { u8 bytes[8]; } MemcardLinkOffset;
typedef struct MemcardLinkColor { u8 bytes[4]; } MemcardLinkColor;
typedef struct MemcardLinkState { s16 phase, timer, opacity, reserved06; GteRotation rotation; } MemcardLinkState;
typedef struct MemcardLinkChannel { s32 reserved[2]; void *slots; } MemcardLinkChannel;
extern MemcardLinkOffset D_801ED818;
extern MemcardLinkColor D_801ED850, D_801ED854;
extern void *D_8009D254;
extern s16 D_801F1D86, D_801F1D88;
extern void func_800CE8F0(void *, s32, void *, void *);

s32 Memcard_LinkEffect(s32 mode, MemcardLinkState *state);

#endif
