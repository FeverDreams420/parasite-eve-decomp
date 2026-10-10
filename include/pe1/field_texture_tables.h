#ifndef PE1_FIELD_TEXTURE_TABLES_H
#define PE1_FIELD_TEXTURE_TABLES_H

#include "common.h"

/* Texture page slot table; D_800E11E6 and D_800E11F6 are entries 1 and 9. */
extern u16 D_800E11E4[];
extern u16 D_800E11E6;
extern u16 D_800E11F6;
/* Palette (CLUT) table indexed by effect kind. */
extern u16 D_800E1204[];
/* Slot table entries 4 and 11 and palette table entry 3. */
extern u16 D_800E11EC, D_800E11FA;
extern u16 D_800E120A;

/* View of one of those entries: a few effects read it as a one-field
 * record, which GCC orders after their particle-record stores. */
typedef struct FieldTextureSlot {
    u16 value;
} FieldTextureSlot;
#define TEXTURE_SLOT_RECORD(slot) (((FieldTextureSlot *)&(slot))->value)

/* Texture page table indexed by a slot from D_800E11E4. */
extern u16 D_800E2850[];
/* Selects the alternate palette for effect kind 4 when set. */
extern int D_800F3428;

#endif /* PE1_FIELD_TEXTURE_TABLES_H */
