#ifndef PE1_ROOM_MODEL_PULSE_PARTICLE_H
#define PE1_ROOM_MODEL_PULSE_PARTICLE_H

#include "common.h"
#include "pe1/gte_short_vector.h"

/* room_m256 pulsing model: one ring particle. func_8019552C advances and
 * draws it; the model pulse controller (func_80195728) seeds sixteen. */
typedef struct RoomModelPulseParticle {
    u16 frame;                    /* 0x00 */
    u16 offset;                   /* 0x02 */
    s16 scale;                    /* 0x04 */
} RoomModelPulseParticle;

typedef struct RoomModelPulseAnchor {
    s16 x, y, z;
} RoomModelPulseAnchor;

/* Shared with the controller: the ring spin it writes (slot-2 output) and
 * the ring centre the particles orbit. */
extern GteShortVector D_80196098;
extern RoomModelPulseAnchor D_801960A0;

int func_8019552C(int mode, RoomModelPulseParticle *particle);

#endif /* PE1_ROOM_MODEL_PULSE_PARTICLE_H */
