#ifndef PE1_FIELD_ENGINE_CALLS_H
#define PE1_FIELD_ENGINE_CALLS_H

#include "pe1/field_transform_point.h"
#include "pe1/field_collision.h"

/* Field engine helpers called from the room and scene effects
 * (FieldFx_CollisionQueries.c). The definitions take typed
 * records; each effect passes its own view, so these pointers are untyped. */

/* Sends asset message `message` at a halfword position while the object is
 * in status 3 or 4; returns the Asset_Find08w result, else 0. */
int func_800C6800(void *obj, int message, void *pos);
/* Nonzero when the player is within its hit radius plus radius of a
 * position on the XZ plane. */
int func_800C6B90(void *position, int radius);
/* Status check on the object (FieldFx_GetStatus() == 3). */
int func_800C6C18(void *obj);

#endif
