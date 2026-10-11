#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/asset_find.h"
#include "pe1/field_anim_pool.h"
#include "pe1/field_actor.h"

/* Floor-placement basis vectors and the asset lookups at a field
 * position; func_800C66C8 reads the normals func_800C65E4 writes. */

int FieldEng_GetStatus(void *obj);

#include "FieldEng_CollisionQueries_2_externs.h"

void func_800C65E4(s16 *from, s16 *to, GteMatrix *out) {
    GteVector side;
    GteVector up;
    GteVector forward;
    GteVector basis;

    basis = D_800C213C;
    forward.x = to[0] - from[0];
    forward.y = to[1] - from[1];
    forward.z = to[2] - from[2];
    OuterProduct12(&forward, &basis, &side);
    OuterProduct12(&forward, &side, &up);
    VectorNormalS(&side, (GteShortVector *)&out->t[0]);
    VectorNormalS(&up, (GteShortVector *)&out->t[1]);
    VectorNormalS(&forward, (GteShortVector *)&out->t[2]);
}

void *func_800C66C8(char *obj, void *asset, GteMatrix *pos) {
    void *result = 0;

    if (FieldEng_GetStatus(obj) == 3 || FieldEng_GetStatus(obj) == 4) {
        result = (void *)Asset_Find08w((int)asset, ((FieldAnimObjectPrefix *)obj)->actor->state->progress,
                               (s16)pos->t[0], (s16)pos->t[1], (s16)pos->t[2]);
    }

    return result;
}

void *func_800C6764(char *obj, void *asset, GteVector *pos) {
    void *result = 0;

    if (FieldEng_GetStatus(obj) == 3 || FieldEng_GetStatus(obj) == 4) {
        result = (void *)Asset_Find08w((int)asset, ((FieldAnimObjectPrefix *)obj)->actor->state->progress,
                               (s16)pos->x, (s16)pos->y, (s16)pos->z);
    }

    return result;
}

void *func_800C6800(char *obj, void *asset, GteShortVector *pos) {
    void *result = 0;

    if (FieldEng_GetStatus(obj) == 3 || FieldEng_GetStatus(obj) == 4) {
        result = (void *)Asset_Find08w((int)asset, ((FieldAnimObjectPrefix *)obj)->actor->state->progress,
                               pos->x, pos->y, pos->z);
    }

    return result;
}
