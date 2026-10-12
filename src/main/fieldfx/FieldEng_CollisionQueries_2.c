#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/asset_find.h"
#include "pe1/field_anim_pool.h"
#include "pe1/field_actor.h"
#include "pe1/field_engine_state.h"
#include "pe1/field_collision.h"

/* Floor-placement basis vectors and the asset lookups at a field
 * position; func_800C66C8 reads the normals func_800C65E4 writes. */

void func_800C65E4(s16 *from, s16 *to, GteMatrix *out) {
    GteVector side;
    GteVector up;
    GteVector forward;
    GteVector basis;

    basis = g_FieldPlacementUpAxis;
    forward.x = to[0] - from[0];
    forward.y = to[1] - from[1];
    forward.z = to[2] - from[2];
    OuterProduct12(&forward, &basis, &side);
    OuterProduct12(&forward, &side, &up);
    VectorNormalS(&side, (GteShortVector *)&out->t[0]);
    VectorNormalS(&up, (GteShortVector *)&out->t[1]);
    VectorNormalS(&forward, (GteShortVector *)&out->t[2]);
}

int func_800C66C8(FieldAnimObjectPrefix *owner, int message, GteMatrix *pos) {
    int result = 0;

    if (FieldEng_GetStatus(owner) == 3 || FieldEng_GetStatus(owner) == 4) {
        result = Asset_Find08w(message, owner->actor->state->progress,
                               (s16)pos->t[0], (s16)pos->t[1], (s16)pos->t[2]);
    }

    return result;
}

int func_800C6764(FieldAnimObjectPrefix *owner, int message, GteVector *pos) {
    int result = 0;

    if (FieldEng_GetStatus(owner) == 3 || FieldEng_GetStatus(owner) == 4) {
        result = Asset_Find08w(message, owner->actor->state->progress,
                               (s16)pos->x, (s16)pos->y, (s16)pos->z);
    }

    return result;
}

int func_800C6800(FieldAnimObjectPrefix *owner, int message, GteShortVector *pos) {
    int result = 0;

    if (FieldEng_GetStatus(owner) == 3 || FieldEng_GetStatus(owner) == 4) {
        result = Asset_Find08w(message, owner->actor->state->progress,
                               pos->x, pos->y, pos->z);
    }

    return result;
}
