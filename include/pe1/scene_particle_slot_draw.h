#ifndef PE1_SCENE_PARTICLE_SLOT_DRAW_H
#define PE1_SCENE_PARTICLE_SLOT_DRAW_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/render_object.h"
#include "pe1/psyq_gpu.h"
#include "pe1/psyq_libc.h"
#include "pe1/field_texture_tables.h"

/* Scene e08 particle slots draw: four textured sprites at the slot offsets
 * around the scene anchor, then their four shadows on the floor. */
extern int D_8019956C;
extern int D_8019957C;
extern GteShortVector D_8018F040;
extern RenderColor D_8018F048;

#endif
