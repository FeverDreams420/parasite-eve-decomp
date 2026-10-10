#ifndef PE1_FIELD_EFFECT_POOL_H
#define PE1_FIELD_EFFECT_POOL_H

#include "common.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_anim_pool.h"
#include "pe1/render_object.h"
#include "pe1/field_actor.h"
#include "pe1/field_model_draw.h"
#include "pe1/room_floor.h"

/* Effect pool declarations for field engine emitters that read the room
 * render Y override as a one-field record, so its load stays below the
 * in-struct position stores like retail. field_anim.h declares the same word
 * as a plain s16 for its other users, so these units include this header
 * instead of field_anim.h. */
typedef FieldAnimTaskSlot FieldEffectPool;
typedef FieldAnimObjectPrefix FieldEffectOwner;

typedef int (*FieldEffectCallback)(int mode, void *state);

extern u8 *D_800F32D4;

#endif
