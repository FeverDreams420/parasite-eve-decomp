#ifndef PE1_FIELD_TEXTURE_TABLES_H
#define PE1_FIELD_TEXTURE_TABLES_H

#include "common.h"

/* Texture page slot table; D_800E11E6 and D_800E11F6 are entries 1 and 9. */
extern u16 D_800E11E4[];
extern u16 D_800E11E6;
extern u16 D_800E11F6;
/* Palette (CLUT) table indexed by effect kind. */
extern u16 D_800E1204[];
/* Texture page table indexed by a slot from D_800E11E4. */
extern u16 D_800E2850[];
/* Selects the alternate palette for effect kind 4 when set. */
extern int D_800F3428;

#endif /* PE1_FIELD_TEXTURE_TABLES_H */
