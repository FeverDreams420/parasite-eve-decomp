#ifndef PE1_PROJECTION_SLOT_H
#define PE1_PROJECTION_SLOT_H

#include "common.h"

/* Points at the projection distance (GTE H) of the current view. The
 * renderer, the 3D sound panner and the billboards read it. A one-field
 * record, so the billboard's load stays below its scratch stores.
 * fx_common points it at its own scratch while it projects; its two units
 * take cc1 -G8, so cc1 sees this small word as a cheap address and loads
 * and stores it absolutely, as retail does, instead of keeping its address
 * in a register. */
typedef struct FieldProjectionSlot {
    s32 *distance;
} FieldProjectionSlot;

extern FieldProjectionSlot D_800BCFA8;

#endif /* PE1_PROJECTION_SLOT_H */
