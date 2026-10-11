#include "room_m350_shared.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
#include "pe1/gte.h"
#include "pe1/field_vector_math.h"
#include "pe1/field_anim_pool.h"
#include "room_m350_data.h"
typedef RoomM350Vector Vector;
typedef RenderMatrix Matrix;
typedef FieldActorState Owner;
typedef FieldActor Instance;
typedef RoomM350Actor Actor;
#include "RoomEffect_SequenceController_externs.h"
#include "pe1/gte_sine_table.h"
int func_801958DC(int event) {
    Instance *instance=(Instance *)ANIM_OWNER_AS(Actor)->instance;
    Vector offset;
    if(event==0) {
        register int one=1;
        {
            Owner *owner=instance->state;
            register unsigned int flags=owner->core_flags;
            flags |= 0x40000000;
            owner->core_flags=flags;
        }
        one=2;
        D_8019A7FC.repeats=one;
        D_8019A7FC.completionFlags[0]=0; D_8019A7FC.completionFlags[1]=0; D_8019A7FC.completionFlags[2]=0;
        D_8019A7FC.pointerCount=0; D_8019A7FC.positionCount=0; D_8019A7FC.activeCount=0;
        asm volatile("" : : : "$2");
        return 0;
    }
    if(event==1) {
        Matrix *matrix;
        int frame;
        int repetitions;
        if(instance->state && instance->state->action_state[0]==1)
            instance->state->action_state[0]=2;
        matrix=&D_8019A7F8->render_object.matrices[17];
        offset.x=0; offset.y=-144; offset.z=128;
        gte_ldrotmatrix(matrix);
        gte_ldtransmatrix(matrix);
        gte_ldv0(&offset);
        gte_rtv0tr_mac();
        gte_stsv(&D_8019A7A0);
        if(instance->mode!=13) return 0;
        repetitions=D_8019A7FC.repeats;
        frame=instance->anim.parts.integer;
        if(repetitions>0) {
            s16 speed;
            int angle;
            frame=(short)frame;
            if(frame>=25) {
                instance->anim.fixed=0x60000;
                D_8019A7FC.repeats=(unsigned short)D_8019A7FC.repeats-1;
            }
            if(frame<19) return 0;
            speed=D_800966EC[((frame-19)*341)&4095].part.sin;
            speed/=32;
            angle=FieldEng_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation,(Vec3Fixed *)instance->render_object.model_matrix.translation);
            instance->rot_y=FieldEng_TurnToward(instance->rot_y,(short)angle,speed);
        } else {
            if((short)frame>=instance->action-1) {
                if(instance->state) instance->state->action_state[0]=4;
                D_8019A7FC.completionFlags[0]=1;
                return 1;
            }
        }
    }
    return 0;
}
