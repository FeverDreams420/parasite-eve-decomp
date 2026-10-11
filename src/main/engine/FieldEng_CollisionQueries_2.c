/* CC1_FLAGS: -fno-schedule-insns */
#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/asset_find.h"
#include "pe1/field_anim_pool.h"
#include "pe1/field_actor.h"

/* Floor-placement basis vectors and the asset lookups at a field
 * position; func_800C66C8 reads the normals func_800C65E4 writes. */

void OuterProduct12(int *a, int *b, int *out);
void VectorNormalS(int *in, void *out);
int FieldEng_GetStatus(void *obj);

#include "FieldEng_CollisionQueries_2_externs.h"

void func_800C65E4(s16 *from, s16 *to, GteMatrix *out) {
    int crossA[4];
    int crossB[4];
    int delta[4];
    GteVector basis;
    s16 *from_t0;
    s16 *to_a3;
    GteMatrix *out_s2;
    register int *basis_src asm("$5");
    int *delta_s1;
    int *crossB_s0;
    register int *call_delta_a0 asm("$4");
    int *call_basis_a1;
    register int *call_crossA_a2 asm("$6");
    int b0;
    register int b1 asm("$3");
    register int b2 asm("$4");

    from_t0 = from;
    to_a3 = to;
    out_s2 = out;
    asm volatile("" ::: "$16", "$17");
    basis_src = D_800C213C;
    asm volatile("" : "=r"(basis_src) : "0"(basis_src));
    b0 = basis_src[0];
    b1 = basis_src[1];
    b2 = basis_src[2];
    basis.x = b0;
    basis.y = b1;
    basis.z = b2;
    b0 = basis_src[3];
    basis.pad = b0;
    asm volatile("" ::: "memory");

    delta_s1 = delta;
    call_delta_a0 = delta_s1;
    delta_s1[0] = to_a3[0] - from_t0[0];
    call_basis_a1 = (int *)&basis;
    delta_s1[1] = to_a3[1] - from_t0[1];
    call_crossA_a2 = crossA;
    delta_s1[2] = to_a3[2] - from_t0[2];

    OuterProduct12(call_delta_a0, call_basis_a1, call_crossA_a2);
    call_delta_a0 = delta_s1;
    call_basis_a1 = crossA;
    asm volatile("" : "=r"(call_basis_a1) : "0"(call_basis_a1));
    crossB_s0 = crossB;
    OuterProduct12(call_delta_a0, call_basis_a1, crossB_s0);

    VectorNormalS(crossA, &out_s2->t[0]);
    VectorNormalS(crossB_s0, &out_s2->t[1]);
    VectorNormalS(delta_s1, &out_s2->t[2]);
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
