#ifndef PE1_FIELD_ANIM_POOL_H
#define PE1_FIELD_ANIM_POOL_H

#include "common.h"

/* Field effect task pool and the active effect owner, shared by field_anim.h
 * and field_effect_pool.h so both declare D_800F33E0 and D_800F32D0 with one
 * type. */
typedef struct FieldAnimTaskSlot {
    u16 id;              /* 0xFFFF marks a free slot. */
    u16 age;
    char *start;
    char *end;
} FieldAnimTaskSlot;

/* Observed prefix of the active effect owner; actor is at +0x08. */
typedef struct FieldAnimObjectPrefix {
    u8 reserved00;
    u8 asset_type;
    u8 reserved02[6];
    struct FieldActor *actor;
} FieldAnimObjectPrefix;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimObjectPrefix, actor) == 8,
                  field_anim_object_actor_offset);

extern FieldAnimTaskSlot *D_800F33E0;
extern FieldAnimObjectPrefix *D_800F32D0;

#endif
