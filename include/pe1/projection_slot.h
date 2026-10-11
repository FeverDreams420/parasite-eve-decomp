#ifndef PE1_PROJECTION_SLOT_H
#define PE1_PROJECTION_SLOT_H

#include "common.h"

/* Points at the projection distance (GTE H) of the current view. The
 * renderer, the 3D sound panner and the billboards read it. A one-field
 * record, so the billboard's load stays below its scratch stores.
 * fx_common, which points it at its own scratch while it projects, keeps
 * a scalar view (fx_common.h): as a record its saved value moves to
 * another register. */
typedef struct FieldProjectionSlot {
    s32 *distance;
} FieldProjectionSlot;

extern FieldProjectionSlot D_800BCFA8;

#endif /* PE1_PROJECTION_SLOT_H */
