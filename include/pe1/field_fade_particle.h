#ifndef PE1_FIELD_FADE_PARTICLE_H
#define PE1_FIELD_FADE_PARTICLE_H

#include "common.h"
#include "pe1/render_object.h"
#include "pe1/field_effect_pool.h"
#include "pe1/player_entity.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"

/* Field engine fade particle (func_800DEFFC), spawned by the fading
 * emitter func_800DF6AC: a glow pinned to one actor matrix point that
 * sheds drifting and falling sparks into the same pool. */

int rand(void);

#endif /* PE1_FIELD_FADE_PARTICLE_H */
