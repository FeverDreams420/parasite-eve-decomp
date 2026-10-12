/* CC1_FLAGS: -fno-strength-reduce */
#include "common.h"
#include "pe1/field_textured_strip.h"
#include "pe1/gte_types.h"
#include "pe1/field_tri_test.h"
#include "pe1/gte.h"
#include "pe1/field_shaded_quad.h"
#include "pe1/field_actor.h"
#include "pe1/psyq_libc.h"


int func_800C5EB0(char *data, void *unused, int *hit) {
    GteShortVector local[5];
    FieldTexturedStrip *data_s3 = (FieldTexturedStrip *)data;
    register FieldTexturedChainNode *entry asm("$17");
    int *hit_s4;
    u32 i;
    u32 idx;
    u32 count;
    int countMinus;

    entry = data_s3->nodes;
    hit_s4 = hit;
    *hit_s4 = 0;

    idx = data_s3->collision_step;
    count = data_s3->count;
    if (idx < count) {
        entry[idx].visible = 2;
    }

    countMinus = data_s3->count;
    countMinus -= 1;
    i = 0;
    if (countMinus != 0) {
        FieldStripNodeTail *body;

        body = (FieldStripNodeTail *)&entry->brightness;
        do {
            int rnd = rand();

            body[-1].nextCellStep = rnd % 4;

            if (entry->visible == 2) {
                local[0] = body[0].edgeA;
                local[1] = body[0].edgeB;
                local[2] = body[1].edgeA;
                local[3] = body[1].edgeB;

                local[0].y = 0;
                local[1].y = 0;
                local[2].y = 0;
                local[3].y = 0;

                if (func_800C6B20(local) == 1) {
                    *hit_s4 = 1;
                }

                if (body->brightness >= 0xD) {
                    body->brightness -= 0xC;
                } else {
                    body->brightness = 0;
                    entry->visible = 0;
                }
            }
            i++;
            body++;
            entry++;
        } while (i < data_s3->count - 1);
    }

    i = (data_s3->collision_step++ == 0x64);

    return i;
}

void func_800C608C(int scale, u8 *src, u8 *dst) {
    scale = (short)scale;

    if (scale != 0x80) {
        int value;

        value = src[0];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[0] = value;

        value = src[1];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[1] = value;

        value = src[2];
        value *= scale;
        if (value > 0x7FFF) {
            value = 0x7FFF;
        }
        value >>= 7;
        dst[2] = value;
    } else {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
    }
}

int func_800C6148(FieldActor *obj, s16 *pos, unsigned short radius) {
    int delta[3];
    int x = Pe1Fixed_Integer(&obj->pos_x) - pos[0];
    int z;
    int x_sq = x * x;
    int z_sq;
    int radius_sq;
    delta[0] = x;
    z = Pe1Fixed_Integer(&obj->pos_z) - pos[2];
    z_sq = z * z;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}

int func_800C653C(void *arg0, GteShortVector *verts);


int func_800C61A8(void *arg0, GteMatrix *matrix) {
    GteShortVector verts[4];

    ApplyMatrixSV(matrix, &D_800F3310[0], &verts[0]);
    ApplyMatrixSV(matrix, &D_800F3310[1], &verts[1]);
    ApplyMatrixSV(matrix, &D_800F3310[2], &verts[2]);
    ApplyMatrixSV(matrix, &D_800F3310[3], &verts[3]);

    verts[0].x += matrix->t[0];
    verts[1].x += matrix->t[0];
    verts[2].x += matrix->t[0];
    verts[3].x += matrix->t[0];

    verts[0].y = 0;
    verts[1].y = 0;
    verts[2].y = 0;
    verts[3].y = 0;

    verts[0].z += matrix->t[2];
    verts[1].z += matrix->t[2];
    verts[2].z += matrix->t[2];
    verts[3].z += matrix->t[2];

    return func_800C653C(arg0, verts) != 0;
}

/* Floor-triangle containment in XZ: OP cross products of each edge with the
 * point relative to its corner; the point is inside when all Y components
 * agree in sign. */

int func_800C62DC(GteShortVector *point, GteShortVector *tri)
{
    FieldTriScratch *second;
    FieldTriScratch *third;
    FieldTriScratch *result;

    FIELD_TRI_SCRATCH->edge[0].y = 0;
    FIELD_TRI_SCRATCH->edge[1].y = 0;
    FIELD_TRI_SCRATCH->edge[2].y = 0;
    FIELD_TRI_SCRATCH->rel[0].y = 0;
    FIELD_TRI_SCRATCH->rel[1].y = 0;
    FIELD_TRI_SCRATCH->rel[2].y = 0;
    FIELD_TRI_SCRATCH->edge[0].x = tri[1].x - tri[0].x;
    FIELD_TRI_SCRATCH->edge[0].z = tri[1].z - tri[0].z;
    D_800E2844 = FIELD_TRI_SCRATCH;
    FIELD_TRI_SCRATCH->rel[0].x = point->x - tri[0].x;
    FIELD_TRI_SCRATCH->rel[0].z = point->z - tri[0].z;
    gte_ldopv1_psyq(&D_800E2844->edge[0]);
    gte_ldopv2(&FIELD_TRI_SCRATCH->rel[0]);
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point->x - tri[1].x;
    FIELD_TRI_SCRATCH->rel[1].z = point->z - tri[1].z;
    gte_stlvnl(&FIELD_TRI_SCRATCH->cross[0]);

    second = D_800E2844;
    gte_ldopv1_psyq(&second->edge[1]);
    gte_ldopv2(&second->rel[1]);
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point->x - tri[2].x;
    second->rel[2].z = point->z - tri[2].z;
    gte_stlvnl(&second->cross[1]);

    third = D_800E2844;
    gte_ldopv1_psyq(&third->edge[2]);
    gte_ldopv2(&third->rel[2]);
    gte_cop2_hazard_slot();
    gte_op0();
    gte_stlvnl(&third->cross[2]);

    result = D_800E2844;
    return ((result->cross[0].y ^ result->cross[1].y) |
            (result->cross[1].y ^ result->cross[2].y)) > 0;
}

int func_800C653C(void *arg0, GteShortVector *verts) {
    int first = func_800C62DC(arg0, verts);

    return first | func_800C62DC(arg0, verts + 1);
}

int func_800C6584(s16 *a, int radiusA, s16 *b, int radiusB) {
    int delta[3];
    int x = a[0] - b[0];
    int z;
    int x_sq = x * x;
    int z_sq;
    int radius;
    int radius_sq;
    delta[0] = x;
    z = a[2] - b[2];
    z_sq = z * z;
    radius = radiusA + radiusB;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}
