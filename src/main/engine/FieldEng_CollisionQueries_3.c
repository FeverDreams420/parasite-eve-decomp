#include "common.h"
#include "pe1/field_tri_test.h"
#include "pe1/gte.h"
#include "pe1/gte_types.h"
#include "pe1/field_actor.h"
#include "pe1/field_anim_pool.h"
#include "pe1/battle.h"

/* func_800C62DC specialised for the player position (16.16 integer halves). */

/* Matching debt: 3 register pins and 5 empty constraints.
 * Diagonal loads use gte_ldopv1_psyq; the other vector loads stay C. */
int func_800C689C(GteShortVector *tri)
{
    int playerX;
    register int playerZ asm("$6");
    int pointZ, vertexZ;
    GteShortVector point;
    FieldTriScratch *second;
    FieldTriScratch *third;
    FieldTriScratch *result;

    playerX = ((FieldTriEntity *)D_8009D254)->pos[0].integer;
    point.x = playerX;
    point.y = ((FieldTriEntity *)D_8009D254)->pos[1].integer;
    playerZ = ((FieldTriEntity *)D_8009D254)->pos[2].integer;
    point.z = playerZ;
    FIELD_TRI_SCRATCH->edge[0].y = 0;
    FIELD_TRI_SCRATCH->edge[1].y = 0;
    FIELD_TRI_SCRATCH->edge[2].y = 0;
    FIELD_TRI_SCRATCH->rel[0].y = 0;
    FIELD_TRI_SCRATCH->rel[1].y = 0;
    FIELD_TRI_SCRATCH->rel[2].y = 0;
    FIELD_TRI_SCRATCH->edge[0].x = tri[1].x - tri[0].x;
    FIELD_TRI_SCRATCH->edge[0].z = tri[1].z - tri[0].z;
    D_800E2844 = FIELD_TRI_SCRATCH;
    FIELD_TRI_SCRATCH->rel[0].x = point.x - tri[0].x;
    asm volatile("" : "=m"(D_800E2844) : "m"(D_800E2844));
    FIELD_TRI_SCRATCH->rel[0].z = point.z - tri[0].z;
    {
        register const GteVector *inputVector asm("$8");
        inputVector = &D_800E2844->edge[0];
                gte_ldopv1_psyq(inputVector);
        inputVector = &FIELD_TRI_SCRATCH->rel[0];
        gte_ldir3_precise(inputVector);
        gte_ldir1_precise(inputVector);
        gte_ldir2_precise(inputVector);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    FIELD_TRI_SCRATCH->edge[1].x = tri[2].x - tri[1].x;
    FIELD_TRI_SCRATCH->edge[1].z = tri[2].z - tri[1].z;
    FIELD_TRI_SCRATCH->rel[1].x = point.x - tri[1].x;
    pointZ = point.z;
    vertexZ = tri[1].z;
    asm volatile("" : : : "$8");
    FIELD_TRI_SCRATCH->rel[1].z = pointZ - vertexZ;
    {
        register GteVector *cross asm("$8");
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
        gte_ldir3_precise(relative);
        gte_ldir1_precise(relative);
        gte_ldir2_precise(relative);
    }
    gte_cop2_hazard_slot();
    gte_op0();
    second->edge[2].x = tri[0].x - tri[2].x;
    second->edge[2].z = tri[0].z - tri[2].z;
    second->rel[2].x = point.x - tri[2].x;
    second->rel[2].z = point.z - tri[2].z;
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
        gte_ldir3_precise(relative);
        gte_ldir1_precise(relative);
        gte_ldir2_precise(relative);
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

int func_800C6B20(GteShortVector *arg0) {
    GteShortVector pos;
    FieldActor *entity = (FieldActor *)D_8009D254;
    int first;
    register int value asm("$2");
    GteShortVector *ptr;
    value = Pe1Fixed_Integer(&entity->pos_x);
    ptr = &pos;
    pos.x = value;
    value = Pe1Fixed_Integer(&entity->pos_y);
    pos.y = value;
    value = Pe1Fixed_Integer(&entity->pos_z);
    pos.z = value;
    first = func_800C62DC(ptr, arg0);
    return first | func_800C62DC(ptr, arg0 + 1);
}

int func_800C6B90(s16 *pos, int extraRadius) {
    int delta[3];
    s16 local[4];
    BattleEntity *entity = D_8009D254;
    int x = entity->posX.parts.integer;
    int radius = (s16)entity->renderObject.hit_cylinder.radius;
    int z;
    int x_sq;
    int z_sq;
    int radius_sq;
    local[0] = x;
    {
        int y;
        y = entity->posY.parts.integer;
        local[1] = y;
    }
    z = entity->posZ.parts.integer;
    local[2] = z;
    x -= pos[0];
    x_sq = x * x;
    delta[0] = x;
    z -= pos[2];
    z_sq = z * z;
    radius += extraRadius;
    radius_sq = radius * radius;
    delta[2] = z;
    return x_sq + z_sq < radius_sq;
}

int FieldEng_GetStatus(void *obj);

int func_800C6C18(char *obj) {
    int enabled;

    if (FieldEng_GetStatus(obj) == 3) {
        enabled = (((FieldAnimObjectPrefix *)obj)->actor->state->action_state[0] == 2);
    } else {
        enabled = 1;
    }

    if (enabled != 0) {
        if (FieldEng_GetStatus(obj) == 3) {
            ((Combatant *)D_8009D254->core)->stateFlags |= 0x4000;
            ((FieldAnimObjectPrefix *)obj)->actor->state->core_flags |= 0x80000000;
        }
    }

    return enabled;
}

int FieldEng_GetStatus(void *object) {
    FieldAnimObjectPrefix *obj = object;
    FieldActor *data = obj->actor;
    int status = 0;

    if (data != 0) {
        FieldActorState *inner = data->state;

        if (inner == 0) {
            status = 1;
        } else {
            status = 3;
            /* The enemy record keeps its signed alive HP here. */
            if (inner->control10.command_value <= 0) {
                status = 2;
            }
        }
    }

    if (obj->asset_type == 7) {
        status = 4;
    }
    if (obj->asset_type == 0xE) {
        status = 4;
    }
    if (data == (FieldActor *)D_8009D254) {
        status = 5;
    }

    return status;
}
