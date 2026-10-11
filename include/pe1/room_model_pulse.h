#ifndef PE1_ROOM_MODEL_PULSE_H
#define PE1_ROOM_MODEL_PULSE_H

#include "pe1/room_spark.h"
#include "pe1/room_model_pulse_particle.h"
#include "pe1/field_texture_tables.h"

/* room_m256 pulsing model effect: one controller (func_80195728) that seeds
 * sixteen ring particles on frame 1, draws a ring sprite at the slot-2
 * position and, from frame 4 on, a model scaled by cos and brightened by sin.
 * The ring particles (func_8019552C) read the shared state below. */

typedef struct RoomModelPulseMatrix {
    s16 m[3][3];
    s16 reserved12;
    s32 t[3];                     /* 0x14 */
} RoomModelPulseMatrix;

typedef struct RoomModelPulseScale {
    s32 x, y, z, reserved0C;
} RoomModelPulseScale;

extern GteRotation D_8018F258;          /* ring placement rotation seed */
extern u8 D_80195E8C[];                 /* colour ramp table */
extern s16 D_80196094;                  /* ring brightness */
extern void *D_801960A8;                /* model asset */

int func_80195728(int mode, s16 *state);

#endif /* PE1_ROOM_MODEL_PULSE_H */
