#ifndef PE1_FIELD_TRANSFORM_POINT_H
#define PE1_FIELD_TRANSFORM_POINT_H

#include "common.h"
#include "pe1/gte_short_vector.h"
#include "pe1/gte_types.h"

/* Field engine helpers that place a point through one of an owner's
 * transform records (RoomFxTransformOwner in pe1/room_fx.h). */
struct RoomFxTransformOwner;

void func_800CE9D4(struct RoomFxTransformOwner *owner, int index,
                   GteShortVector *out);
void FieldEng_TransformMatrixPoint(struct RoomFxTransformOwner *owner, int index,
                                  const GteShortVector *input,
                                  GteShortVector *output);

void FieldEng_RotateVector(const GteMatrixWords *matrix,
                           const GteShortVector *input, GteShortVector *output);
void FieldEng_CalculateLookAngles(GteShortVector *from, GteShortVector *to,
                                 GteShortVector *out);

#endif /* PE1_FIELD_TRANSFORM_POINT_H */
