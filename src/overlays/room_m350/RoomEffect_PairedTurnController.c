#include "room_m350_shared.h"
#include "pe1/field_actor.h"
#include "pe1/player_entity.h"
#include "pe1/gte.h"
#include "pe1/field_vector_math.h"
#include "pe1/field_anim_pool.h"
#include "room_m350_data.h"
/* Animation 11 controller: paired model points and alternating player-facing turn. */
typedef RoomM350Vector Vector;
typedef RenderMatrix Matrix;
typedef FieldActorState Owner;
typedef FieldActor Instance;
typedef RoomM350Actor Actor;
extern short D_8019A77E[],D_8019A798,D_800966EC[];
extern unsigned char D_8019A79A,D_8019A79C;
int func_80193D6C(int event)
{
    register Instance *instance asm("$16")=(Instance *)ANIM_OWNER_AS(Actor)->instance;
    Vector offset;
    if(event==0) {
        Owner *owner=instance->state;
        unsigned int flags=owner->core_flags;
        owner->core_flags=flags|0x40000000;
        asm volatile("" ::: "memory");
        D_8019A798=3; D_8019A79A=0; D_8019A79C=0;
        asm volatile("" ::: "$2");
        return 0;
    }
    if(event==1) {
        short i;
        Vector *input;
        Vector *base;
        int frame;
        short *repeat;
        if(instance->state && instance->state->action_state[0]==1)
            instance->state->action_state[0]=2;
        i=0;
        input=&offset;
        base=D_8019A778;
        offset.x=-112; offset.y=0; offset.z=-80;
        for(;i<2;i++) {
            register Matrix *matrix asm("$2")=D_8019A774->render_object.matrices;
            Vector *out;
            if(i) matrix+=15; else matrix+=11;
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                int *words=(int *)matrix;
                /* Separate transfers and barriers keep GCC from hoisting the
                 * pad-array address. These are not GTE hardware hazards. */
                x=words[0];
                y=words[1];
                gte_ctc2_0(x);
                gte_ctc2_1(y);
                x=words[2];
                y=words[3];
                z=words[4];
                gte_ctc2_2(x);
                gte_ctc2_3(y);
                asm volatile("" ::: "memory");
                gte_ctc2_4(z);
                asm volatile("" ::: "memory");
                x=words[5];
                asm volatile("" ::: "memory");
                y=words[6];
                asm volatile("" ::: "memory");
                gte_ctc2_5(x);
                asm volatile("" ::: "memory");
                z=words[7];
                asm volatile("" ::: "memory");
                gte_ctc2_6(y);
                asm volatile("" ::: "memory");
                gte_ctc2_7(z);
                asm volatile("" ::: "memory");
            }
            gte_ldv0(input); gte_rtv0tr_mac();
            out=(Vector *)((i*8)+(unsigned int)base);
            {
                register int x asm("$12");
                register int y asm("$13");
                register int z asm("$14");
                gte_mfc2_9(x); gte_mfc2_10(y); gte_mfc2_11(z);
                out->x=x; out->y=y; out->z=z;
            }
            offset.x=0u-(unsigned short)offset.x;
            D_8019A77E[i*4]=i;
        }
        frame=instance->anim.parts.integer;
        if(instance->mode!=11) return 0;
        repeat=&D_8019A798;
        if(*repeat>0) {
            int speed,bias,angle;
            int signedFrame=(short)frame;
            if(signedFrame>=40) {
                instance->anim.fixed=0x30000;
                *repeat=*(volatile unsigned short *)repeat-1;
            }
            if(signedFrame<28) return 0;
            speed=*(short *)((char *)D_800966EC+(((signedFrame-28)*680)&0x3FF8))/32;
            bias=-128;
            if(*(unsigned short *)repeat&1) bias=128;
            angle=FieldEng_VecToAngle((Vec3Fixed *)g_PlayerEntity->render_object.model_matrix.translation,(Vec3Fixed *)instance->render_object.model_matrix.translation);
            instance->rot_y=FieldEng_TurnToward(instance->rot_y,(short)(bias+angle),speed);
        } else if((short)frame>=instance->action-1) {
            register Owner *owner asm("$4")=instance->state;
            if(owner) owner->action_state[0]=4;
            D_8019A79A=1;
            return 1;
        }
    }
    return 0;
}
