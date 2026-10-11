#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/render_object.h"
#include "pe1/field_actor.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"


typedef struct {
    short x, y, z, unused;
    short velocity, acceleration, brightness, size, phase, counter;
} Particle;

int func_80199148(int event, Particle *object)
{
    /* Retail initializes two unused stack words; their original type is unknown. */
    struct { int x, y; } unused;

    unused.x = 0;
    unused.y = 0;
    if (event == 1) {
        short phase = object->counter >> 1;
        object->phase = phase;
        if (phase >= 8) {
            return 1;
        }
        if (phase >= 4) {
            object->brightness -= 8;
        }
        {
            unsigned int counter = (unsigned short)object->counter;
            register short velocity;
            velocity = object->velocity >> 1;
            object->counter = counter + 1;
            object->y -= velocity;
            object->velocity += object->acceleration;
        }
    } else if (event == 2) {
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        int handle;
        handle = GetClut(0, (kind == 4 && D_800F3428 != 0) ? palette + 6 : palette + 2);
        func_800CEE20((GteShortVector *)object, 0, object->size, object->size, (s16)D_800F3368.parameter02 * object->phase,
            (unsigned short)handle, 1, object->brightness, 0);
    }
    return 0;
}

/* MASPSX_FLAGS: --expand-div */
#include "pe1/gte.h"
typedef RoomM350Vector Vector;
typedef RenderMatrix Matrix;
typedef struct { short count,delay; } State;
typedef FieldActor Instance;
typedef RoomM350Actor Actor;

extern unsigned char D_8019A8C4;
extern int Asset_Find08w(int,int,int,int,int);
extern int func_80199148(int,Particle *);
extern int rand(void),Inv_ScrambleGrid(void),rcos(int),rsin(int);
int func_801992A0(int event,State *state)
{
    Vector offset;
    Matrix rotation;
    /* Match note: original purpose of unused sp+0x40..0x47 is unknown. */
    char frameGap[8];
    switch (event) {
    case 0:

        {
            Instance *instance=(Instance *)ANIM_OWNER_AS(Actor)->instance;
            Matrix *matrix=instance->render_object.matrices;
            Asset_Find08w(0x549,instance->state->progress,(short)matrix->translation[0],(short)matrix->translation[1],(short)matrix->translation[2]);
            state->count=0; state->delay=0; D_8019A8C4=0;
            return func_800CE560(ANIM_POOL_AS(RoomM350Emitter)->pool,20,10,
                                 (FieldAnimCallbackListCallback)func_80199148);
        }
        case 1:
    {
        Particle *particle;
        Actor *actor;
        Instance *instance;
        int random;
        int scatterRandom;
        int index;
        int halfSize;
        int sizeBits;
        int cosine,divisor;
        register int size asm("$3");
        int sine;
        int product;
        if(D_8019A8C4) return 2;
        state->delay=(unsigned short)state->delay+1;
        if(state->delay<4) return 0;
        particle=func_800CE610(ANIM_POOL_AS(RoomM350Emitter)->pool);
        if(!particle) return 0;
        rand();
        particle->brightness=64;
        {
            int count=state->count;
                        particle->size=((unsigned int)count<<7)+4096;
        }
        random=Inv_ScrambleGrid();
        actor=ANIM_OWNER_AS(Actor);
        instance=(Instance *)actor->instance;
        {
            register int low asm("$5")=random&255;
            register int parts asm("$3")=instance->render_object.header->part_count-1;
            register int scaled asm("$2")=low*parts;
                        scaled=scaled/256;
                        {
                register int selected asm("$3")=scaled+1;
                asm volatile("" : : "r"(selected));
                index=selected;
            }
        }
        {
        Instance *ySource;
        Instance *zSource;
        particle->x=instance->render_object.matrices[index].translation[0];
                ySource=(Instance *)actor->instance;
        asm volatile("" : : "r"(ySource));
        size=particle->size;
        asm volatile("" ::: "memory");
        sizeBits=(unsigned short)particle->size;
        particle->y=ySource->render_object.matrices[index].translation[1];
        asm volatile("" : : : "$2");
        {
            u8 low7=random&127;
            product=low7*size;
        }
        zSource=(Instance *)actor->instance;
        asm volatile("" : : "r"(sizeBits), "r"(zSource));
        sizeBits=(unsigned int)sizeBits<<16;
                {
        Matrix *transforms=zSource->render_object.matrices;
        asm volatile("" : : "r"(sizeBits), "r"(transforms));
        halfSize=sizeBits>>17;
        asm volatile("" : : "r"(transforms), "r"(halfSize));
        particle->z=transforms[index].translation[2];
        }
        }
        particle->velocity=0; particle->acceleration=0; particle->counter=0;
        {
            int y=(unsigned short)particle->y;
            asm volatile("" : "=l"(product) : "0"(product), "r"(y), "r"(halfSize));
            particle->y=y-(product>>13);
        }
        offset.x=0;
        scatterRandom=rand(); divisor=96;
        offset.z=((scatterRandom%divisor+64)*halfSize)>>12;
        cosine=rcos(rand());
        sine=rsin(rand());
        rotation.rotation[0][2]=sine;
        sine=0u-(unsigned int)sine;
        rotation.rotation[2][0]=sine;
        sine=4096;
        rotation.rotation[1][1]=sine;
                rotation.rotation[0][0]=cosine; rotation.rotation[2][2]=cosine;
        rotation.translation[2]=0; rotation.translation[1]=0; rotation.translation[0]=0;
        rotation.rotation[2][1]=0; rotation.rotation[1][2]=0;
        rotation.rotation[1][0]=0; rotation.rotation[0][1]=0;
        {
            Matrix *matrix=&rotation;
            Vector *out;
            gte_ldrotmatrix(matrix); gte_ldtransmatrix(matrix);
            out=&offset;
            gte_ldv0(out); gte_rtv0tr_mac();
            gte_stsv(out);
        }
        particle->x=(unsigned short)particle->x+(unsigned short)offset.x;
        particle->z=(unsigned short)particle->z+(unsigned short)offset.z;
        state->delay=0; state->count=(unsigned short)state->count+1;
        Asset_Find08w(0x5FE,((Instance *)ANIM_OWNER_AS(Actor)->instance)->state->progress,particle->x,particle->y,particle->z);
    }
        break;
    case 2:

    D_800F3368.parameter00 = 32;
    D_800F3368.parameter02 = 2;
    D_800F3368.extent_x = 32;
    D_800F3368.extent_y = 32;
    D_800F3368.tpage = D_800E2850[D_800E11E4[11]];
    D_800F3368.palette = 3;
    D_800F3368.parameter06 = 1;
    D_800F3368.parameter0A = 0;
    D_800F3368.depth = 32;
    }
    return 0;
}
