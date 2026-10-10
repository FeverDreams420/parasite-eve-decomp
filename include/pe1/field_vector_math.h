#ifndef PE1_FIELD_VECTOR_MATH_H
#define PE1_FIELD_VECTOR_MATH_H

#include "common.h"

/* World position in 16.16 fixed point; the planar helpers read x and z. */
typedef struct Vec3Fixed {
    s32 x;
    s32 y;
    s32 z;
} Vec3Fixed;

s32 func_800DFC80(Vec3Fixed *lhs, Vec3Fixed *rhs);
/* Planar (x/z) distance between two positions, in whole units. */
s32 func_800DFE20(Vec3Fixed *lhs, Vec3Fixed *rhs);
void func_800DFE94(Vec3Fixed *from, Vec3Fixed *to, s16 *out);
/* Heading from one position to another: ratan2(dx, dz) as a signed
 * halfword. */
int FieldEng_VecToAngle(Vec3Fixed *from, Vec3Fixed *to);
/* Turns a 12-bit angle toward target by at most step. */
int FieldEng_TurnToward(int current, int target, int step);

#endif /* PE1_FIELD_VECTOR_MATH_H */
