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
    u8 owner_kind;   /* 1 for a task-list owner, 0 after a script reset */
    u8 live_slots;   /* script slots alive at the last update pass */
    u8 reserved04[4];
    struct FieldActor *actor;
} FieldAnimObjectPrefix;
PE1_STATIC_ASSERT(PE1_OFFSETOF(FieldAnimObjectPrefix, actor) == 8,
                  field_anim_object_actor_offset);

/* The running task context (FieldAnimTaskContext in field_anim.h). */
extern struct FieldAnimTaskContext *D_800E2368;
extern FieldAnimTaskSlot *D_800F33E0;
extern FieldAnimObjectPrefix *D_800F32D0;

/* Each effect reads the pool and the active owner through its own view of
 * the record; the cast is free (a pointer copy). */
#define ANIM_POOL_AS(type) ((type *)D_800F33E0)
#define ANIM_OWNER_AS(type) ((type *)D_800F32D0)
/* The same for the running task context, which effects read as their own
 * event state. */
#define ANIM_TASK_AS(type) ((type *)D_800E2368)

#endif
