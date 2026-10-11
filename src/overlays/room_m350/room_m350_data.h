#ifndef ROOM_M350_DATA_H
#define ROOM_M350_DATA_H

/* room_m350 data shared by several effect units, declared once. */

#include "common.h"
#include "pe1/gte_short_vector.h"
#include "pe1/gte_types.h"
#include "pe1/field_color.h"
#include "pe1/field_actor.h"
#include "pe1/room_m350_drop.h"

/* Colour seeds handed to the trail and glow drawers. */
extern RenderColor D_8019A3C8;
extern RenderColor D_8019A3D0;

/* Actor found by func_80199B0C (type and sub id lookup), and the pair of
 * anchor positions the paired effects attach to. */
extern FieldActor *D_8019A774;
extern GteShortVector D_8019A778[];

/* Actor found by func_80199CA4, and the joint position it returns. */
extern FieldActor *D_8019A7F8;
extern GteShortVector D_8019A7A0;

/* First animation event slot; the event controller clears the four slots
 * as one word and sets them byte by byte from here. */
extern unsigned char D_8019A854;

/* Rotation matrix the cloud controller builds and the swept flare uses. */
extern GteMatrix D_8019A870;

#endif /* ROOM_M350_DATA_H */
