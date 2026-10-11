/* CC1_FLAGS: -fno-strength-reduce */
#include "common.h"
#include "pe1/field_textured_strip.h"
#include "pe1/gte_types.h"
#include "pe1/field_tri_test.h"
#include "pe1/gte.h"
#include "pe1/field_shaded_quad.h"
#include "pe1/field_actor.h"

int rand(void);
int func_800C6B20(void *arg0);

int func_800C5EB0(char *data, void *unused, int *hit) {
    GteShortVector local[5];
    FieldTexturedStrip *data_s3 = (FieldTexturedStrip *)data;
    char *entries;
    int *hit_s4;
    u32 i;
    int idx;
    int count;
    int countMinus;
    u16 timer;

    entries = (char *)data_s3->nodes;
    hit_s4 = hit;
    *hit_s4 = 0;

    idx = data_s3->collision_step;
    count = data_s3->count;
    if ((u32)idx < count) {
        entries[idx * 0x44] = 2;
    }

    countMinus = data_s3->count;
    countMinus -= 1;
    i = 0;
    if (countMinus != 0) {
        register FieldTexturedChainNode *entry asm("$17");
        char *body;
        entry = (FieldTexturedChainNode *)entries;
        body = entries + 4;
        do {
            int rnd = rand();

            body[-1] = rnd % 4;

            if (entry->visible == 2) {
                local[0] = *(GteShortVector *)(body + 16);
                local[1] = *(GteShortVector *)(body + 24);
                local[2] = *(GteShortVector *)(body + 84);
                local[3] = *(GteShortVector *)(body + 92);

                local[0].y = 0;
                local[1].y = 0;
                local[2].y = 0;
                local[3].y = 0;

                if (func_800C6B20(local) == 1) {
                    *hit_s4 = 1;
                }

                if (*(u16 *)body >= 0xD) {
                    *(u16 *)body -= 0xC;
                } else {
                    *(u16 *)body = 0;
                    entry->visible = 0;
                }
            }
            i++;
            body += 0x44;
            entry++;
        } while (i < data_s3->count - 1);
    }

    timer = *(volatile u16 *)&data_s3->collision_step;
    i = ((s16)timer == 0x64);
    data_s3->collision_step = timer + 1;

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
    volatile int delta[3];
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

/* Matching debt: 2 register pins and 4 empty constraints.
 * Diagonal loads use gte_ldopv1_psyq; the other vector loads stay C. */
int func_800C62DC(GteShortVector *point, GteShortVector *tri)
{
    int pointZ;
    int vertexZ;
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
    asm volatile("" : "=m"(D_800E2844) : "m"(D_800E2844));
    FIELD_TRI_SCRATCH->rel[0].z = point->z - tri[0].z;
    {
        register const GteVector *inputVector asm("$7");
        inputVector = &D_800E2844->edge[0];
        gte_ldopv1_psyq(inputVector);
        inputVector = &FIELD_TRI_SCRATCH->rel[0];
        gte_lwc2_11_8(inputVector);
        gte_lwc2_9_0(inputVector);
        gte_lwc2_10_4(inputVector);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point->x - tri[1].x;
    pointZ = point->z;
    vertexZ = tri[1].z;
    asm volatile("" : : : "$7");
    FIELD_TRI_SCRATCH->rel[1].z = pointZ - vertexZ;
    {
        register GteVector *cross asm("$7");
        cross = &FIELD_TRI_SCRATCH->cross[0];
        gte_swc2_25_0(cross);
        gte_swc2_26_4(cross);
        gte_swc2_27_8(cross);
    }

    second = D_800E2844;
    {
        const GteVector *edge;
        edge = &second->edge[1];
                gte_ldopv1_psyq(edge);
    }
    {
        const GteVector *relative;
        relative = &second->rel[1];
        gte_lwc2_11_8(relative);
        gte_lwc2_9_0(relative);
        gte_lwc2_10_4(relative);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point->x - tri[2].x;
    second->rel[2].z = point->z - tri[2].z;
    {
        GteVector *cross;
        cross = &second->cross[1];
        gte_swc2_25_0(cross);
        gte_swc2_26_4(cross);
        gte_swc2_27_8(cross);
    }

    third = D_800E2844;
    {
        const GteVector *edge;
        edge = &third->edge[2];
                gte_ldopv1_psyq(edge);
    }
    {
        const GteVector *relative;
        relative = &third->rel[2];
        gte_lwc2_11_8(relative);
        gte_lwc2_9_0(relative);
        gte_lwc2_10_4(relative);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    {
        GteVector *cross;
        cross = &third->cross[2];
        gte_swc2_25_0(cross);
        gte_swc2_26_4(cross);
        gte_swc2_27_8(cross);
    }

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
