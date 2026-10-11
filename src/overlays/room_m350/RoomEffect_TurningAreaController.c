#include "pe1/field_sprite_params.h"
#include "pe1/field_actor.h"
#include "room_m350_shared.h"
#include "pe1/scene_assets.h"
#include "pe1/gte.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_model_draw.h"
#include "pe1/field_vector_math.h"
#include "pe1/field_anim_pool.h"
#include "pe1/scene_archive.h"
#include "pe1/psyq_libc.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_gpu.h"
#include "room_m350_data.h"

/* Steers a rotating footprint, tests contact, and draws three model/sprite layers. */
typedef RoomM350Vector Vector;
typedef GteMatrix Matrix;
typedef struct { int x,y,z,pad; } Scale;
typedef struct { void *asset; int yaw; short size,brightness,unknown,turn; unsigned char initialized; } State;
typedef FieldActorState Owner;
typedef FieldActor Instance;
typedef RoomM350Actor Actor;
#include "RoomEffect_TurningAreaController_externs.h"
#include "pe1/gte_sine_table.h"
#include "RoomEffect_TurningAreaController_externs_1.h"
int func_801960C4(int event,State *state)
{
    union { struct { Vector vertices[4]; int area; } collision; Matrix matrix; } local;
    Matrix base;
    Vector position,rotation;
    Scale scale;
    Instance *instance=(Instance *)ANIM_OWNER_AS(Actor)->instance;
    switch (event) {
case 0:
    {
        state->asset=Asset_FindTable08ByU32Key(D_800B0E64[0],0xC54E0704);
        func_800C6D5C(state->asset,0,0);
        state->size=0;
        {
            int yaw=((short )D_8019A82A[0]);
            state->initialized=0;
            state->yaw=yaw<<8;
        }
        return 0;
    }
case 1:
    {
        int timer=D_800E27EC,size;
        register int high asm("$9");
        register int quotient asm("$2");
        int intermediate;
        if(timer<16) size=((4096-D_800966EC[((unsigned int)timer<<6)&0xFC0].part.sin)>>1)+2048;
        else if(timer>=91) {
            int sample=D_800966EC[((unsigned int)(timer-90)<<6)&0xFC0].part.cos;
            asm volatile("" : "=r"(sample) : "0"(sample));
            size=sample>>1;
        }
        else size=2048;
        /* Expose the high product; the final sign correction still implements /3. */
        intermediate=size/3+(size>>31);
        asm("" : "=h"(intermediate) : "0"(intermediate));
        high=intermediate;
        asm volatile("" : "=r"(high) : "0"(high), "r"(size>>31));
        quotient=high-(size>>31);
        state->size=quotient;
        if(state->size<=0) return 1;
        state->brightness=size<=2048 ? size>>4:(4096-size)>>4;
        if(!state->initialized) {
            if(D_8019A831) {
                int distance=func_800DFE20((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x);
                int angle=FieldEng_VecToAngle((Vec3Fixed *)&g_PlayerEntity->pos_x,(Vec3Fixed *)&instance->pos_x);
                int yaw=state->yaw;
                int rawDelta=((unsigned int)angle-(yaw>>8))&4095;
                register int delta asm("$4")=rawDelta;
                int speed,factor;
                if(rawDelta<512) { state->yaw=(unsigned int)yaw-0x20000; delta=rawDelta+512; }
                if((short)delta>3584) { delta-=512; state->yaw=(unsigned int)state->yaw+0x20000; }
                if((short)delta>2048) delta=4096-delta;
                speed=(short)distance-512;
                if(speed<0) speed=0;
                factor=1536; if(speed>1024) factor=1024;
                speed=D_800966EE[((speed/4)&4095)*2]*factor/4096;
                if(speed<0) speed=0;
                speed+=(short)delta;
                asm volatile("" : : "r"(delta), "r"(speed));
                if(((short)angle-(state->yaw>>8))&2048) speed=-speed;
                state->turn=((int)((unsigned int)speed<<8))/128;
            } else state->turn=0;
            state->initialized=1;
        }
        {
            int cosine,sine,i;
            Matrix *matrix;
            register int *out asm("$6");
            Instance *source;
            state->yaw=(unsigned int)state->yaw+state->turn;
            cosine=rcos(state->yaw>>8);
            sine=rsin(state->yaw>>8);
            i=0; matrix=&D_8019A808; out=matrix->t; source=instance;
            matrix->m[0][0]=cosine;
            asm volatile("" : : : "memory");
            D_8019A80C=sine; D_8019A814=0u-sine; D_8019A818=cosine;
            D_8019A824=0; D_8019A820=0; D_8019A81C=0;
            D_8019A816=0; D_8019A812=0; D_8019A80E=0; D_8019A80A=0; D_8019A810=4096;
            for(;i<3;i++) { *out++=source->render_object.model_matrix.translation[0]; source=(Instance *)((char *)source+4); }
        }
        if(state->brightness<64) return 0;
        {
            Matrix *matrix=&D_8019A808;
            Vector *in;
            Vector *out;
            int i;
            asm("" : : "r"(matrix) : "$2", "$3", "$4", "$5", "$6", "$7", "$8");
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
            i=0; out=local.collision.vertices; in=D_8019A574;
            for(;i<4;i++) {
                gte_ldv0(in);
                gte_rtv0tr_mac();
                gte_stsv(out);
                out++; in++;
            }
        }
        {
            int i,previous,point;
            register Vector *out asm("$6");
            int *area;
            i=0; area=&local.collision.area; out=local.collision.vertices;
            previous=((unsigned int)local.collision.vertices[3].z<<16)|(unsigned short)local.collision.vertices[3].x;
            point=((unsigned int)g_PlayerEntity->render_object.model_matrix.translation[2]<<16)|(unsigned short)g_PlayerEntity->render_object.model_matrix.translation[0];
            for(;i<4;i++) {
                int current=((unsigned int)out->z<<16)|(unsigned short)out->x;
                gte_ldsxy0(point); gte_ldsxy2(previous); gte_ldsxy1(current);
                gte_nclip(); gte_stmac0(area);
                if(local.collision.area<0) break;
                out++; previous=current;
            }
            if(i>=4) {
                unsigned char *inside=&D_8019A82F;
                if(!*inside) {
                    Owner *owner;
                    g_PlayerEntity->state->flags|=0x4000;
                    owner=instance->state;
                    if(owner) owner->core_flags|=0x80000000;
                    *inside=1; D_8019A830=1;
                }
            } else D_8019A82F=0;
        }
    }
    break;
case 2:
    {
        int page,palette;
        int rawPage;
        int kind;
        int i;
        int *in,*out;
        Matrix *transforms;
        i=0;
        base=D_8019A808;
        transforms=(Matrix *)instance->render_object.matrices;
        out=base.t; in=transforms[33].t;
        for(;i<3;i++) *out++=*in++;
        rawPage=((int (*)(int,int,int,int))GetTPage)(0,1,0,0);
        page=(unsigned short)(D_800E2850[D_800E11FA]|rawPage);
        kind=D_800F3368.palette;
        palette=D_800E1204[kind];
        palette=GetClut(0,(kind==4 && D_800F3428!=0) ? palette+7 : palette+3);
        GsSetOrign(page,(unsigned short)palette);
        func_800C6ED8(1);
        func_800C6EF8(state->asset);
        func_800C6FA0(state->asset,(unsigned short)state->brightness);
        i=0;
        for (;i<3;) {
            register int size asm("$17");
            register int next asm("$7");
            register int product asm("$3");
            register int high asm("$9");
            int intermediate;
            register void *clearTarget asm("$4");
            register int clearValue asm("$5");
            register int clearLength asm("$6");
            local.matrix=base;
            next=i+1;
            product=state->size*next;
            intermediate=product/3+(product>>31);
            asm("" : "=h"(intermediate) : "0"(intermediate));
            clearTarget=&scale;
            clearValue=0;
            clearLength=16;
            i=next;
            asm volatile("" : : "r"(clearTarget), "r"(clearValue), "r"(clearLength), "r"(i));
            product>>=31;
            asm("" : "=r"(high) : "0"(intermediate), "r"(product));
            size=high-product;
            memset(clearTarget,clearValue,clearLength);
            scale.x=size; scale.y=size; scale.z=2048;
            ScaleMatrix((GteMatrix *)&local.matrix, (const GteVector *)&scale);
            func_800C71E4(state->asset,(GteMatrix *)&local.matrix);
        }
        func_800C6F4C(state->asset);
        i=0;
        position.x=instance->render_object.model_matrix.translation[0]; position.y=instance->render_object.model_matrix.translation[1]; position.z=instance->render_object.model_matrix.translation[2];
        {
            int yaw;
            rotation.x=0;
            yaw=state->yaw;
            rotation.z=0;
            rotation.y=(yaw>>8)+2024;
        }
        {
            register int triple asm("$2")=state->size*3;
            register int size asm("$17");
            asm("" : "=r"(triple) : "0"(triple));
            size=triple*2;
            for (;i<3;i++) {
                func_800D0E88(&position,(GteRotation *)&rotation,size,512,(RenderColor *)D_8019A594,&D_8019A3C8,&D_8019A3C8,state->brightness,1);
                rotation.y=(unsigned short)rotation.y+24;
            }
        }
    }
    }
    return 0;
}
