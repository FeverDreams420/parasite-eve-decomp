#include "pe1/field_sprite_params.h"
#include "pe1/room_m350_effects.h"
#include "pe1/player_entity.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/gte.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"

extern GteMatrix D_8019A870;
extern short D_8019A890[];
#include "pe1/gte_sine_table.h"
extern int D_8019A690[];

int func_801981D0(int event, RoomM350FlareParticle *particle)
{
    GteShortVector position, rotation;
    if (event == 1) {
        int timer = D_800E27EC;
        if (timer >= 8) return 1;
        particle->shade = (short)D_800966EC[((unsigned int)timer << 8) & 0xF00].word >> 5;
    } else if (event == 2) {
        int kind, palette;
        unsigned short clut;
        position.x = 0;
        position.y = particle->y;
        position.z = particle->z;
        {
            register GteMatrix *matrix asm("$8") = &D_8019A870;
            GteShortVector *out;
            gte_ldrotmatrix(matrix);
            gte_ldtransmatrix(matrix);
            out = &position;
            gte_ldv0(out);
            gte_rtv0tr_mac();
            gte_stsv(out);
        }
        position.x += D_8019A890[0];
        position.y += D_8019A890[1];
        position.z += D_8019A890[2];
        rotation.x = (unsigned int)D_800E27EC * 384;
        rotation.y = ((FieldActor *)ANIM_OWNER_AS(RoomM350EffectActor)->instance)->rot_y;
        rotation.z = 0;
        rotation.pad = 1;
        kind = D_800F3368.palette;
        palette = D_800E1204[kind];
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(0, palette);
        {
            int timer = D_800E27EC;
            register int scale asm("$4") = (s16)D_800F3368.parameter02;
            register int phase asm("$3") = timer & 7;
            register int product asm("$8") = scale * phase;
            int texture = product + 64;
            asm("" : : "r"(product), "r"(texture));
            func_800CEE20(&position, (GteRotation *)&rotation, 0x2200, 4096,
                texture, clut, 1, particle->shade, (struct RenderColor *)&D_8019A690[timer & 3]);
        }
    }
    return 0;
}

/* Spawns flares and tests the quad swept by the actor's two side endpoints. */

extern unsigned char D_8019A8C0;
extern short D_8019A898[4],D_8019A8A0[4];
/* Separate field-store views preserve reloads of the complete records. */
extern volatile short rightX asm("D_8019A898");
extern volatile short leftX asm("D_8019A8A0");
extern GteShortVector D_8019A8A8,D_8019A8B0;
extern volatile short D_8019A89C,D_8019A89E,D_8019A8A4;
extern short D_8019A8AE;
extern unsigned short D_800E11EA;
extern int Inv_ScrambleGrid(void);

int func_80198400(int event) {
    GteShortVector offset,vertices[4];
    int area;
    FieldActor *instance=(FieldActor *)ANIM_OWNER_AS(RoomM350EffectActor)->instance;
    switch (event) {
    case 0:

        return func_800CE560(ANIM_POOL_AS(RoomM350EffectEmitter)->pool,8,4,
                             (FieldAnimCallbackListCallback)func_801981D0);
        case 1:
    if(instance->anim.parts.integer>=31) return 2;
    if(!(D_800E27EC&1)) {
        RoomM350FlareParticle *particle=func_800CE610(ANIM_POOL_AS(RoomM350EffectEmitter)->pool);
        if(particle) {
            int random=Inv_ScrambleGrid()&255;
            particle->y=random;
            particle->z=(random>>2)+64;
        }
    }
    if(D_8019A8C0 || instance->mode!=9) return 0;
    {
        register GteShortVector *from asm("$5")=(GteShortVector *)&D_8019A898;
        register GteShortVector *to asm("$4")=&D_8019A8A8;
        asm("" : "=r"(from),"=r"(to) : "0"(from),"1"(to));
        *to=*from;
    }
    {
        register GteShortVector *from asm("$5")=(GteShortVector *)&D_8019A8A0;
        register GteShortVector *to asm("$4")=&D_8019A8B0;
        asm("" : "=r"(from),"=r"(to) : "0"(from),"1"(to));
        *to=*from;
    }
    offset.x=256; offset.y=0; offset.z=0;
    ApplyMatrixSV((GteMatrix *)&instance->render_object.model_matrix,&offset,&offset);
    {
        int positionX=instance->render_object.model_matrix.translation[0];
        unsigned short x;
        unsigned short z;
        int positionZ;
        x=offset.x;
        z=offset.z;
        rightX=x+positionX;
        positionZ=((volatile FieldActor *)instance)->render_object.model_matrix.translation[2];
        D_8019A89E=1;
        positionZ=z+positionZ;
        D_8019A89C=positionZ;
        leftX=((volatile FieldActor *)instance)->render_object.model_matrix.translation[0]-x;
        D_8019A8A4=((volatile FieldActor *)instance)->render_object.model_matrix.translation[2]-z;
    }
    asm volatile("" ::: "memory");
    vertices[0]=*(GteShortVector *)&D_8019A898;
    vertices[1]=*(GteShortVector *)&D_8019A8A0;
    vertices[2]=D_8019A8B0;
    {
        int valid=D_8019A8AE;
        vertices[3]=D_8019A8A8;
        if(!valid) return 0;
        {
            int i=0;
            int previous=((unsigned int)vertices[3].z<<16)|(unsigned short)vertices[3].x;
            int point=((unsigned int)g_PlayerEntity->render_object.model_matrix.translation[2]<<16)|(unsigned short)g_PlayerEntity->render_object.model_matrix.translation[0];
            int *out=&area;
            for(;i<4;) {
                int current=((unsigned int)vertices[i].z<<16)|(unsigned short)vertices[i].x;
                gte_ldsxy0(point); gte_ldsxy2(previous); gte_ldsxy1(current);
                gte_nclip(); gte_stmac0(out);
                if(area<0) break;
                i++;
                previous=current;
            }
            if(i<4) return 0;
            {
                FieldActor *player=g_PlayerEntity;
                asm("" : "=r"(player) : "0"(player));
                D_8019A8C0=1;
                asm volatile("" ::: "memory");
                player->state->flags|=0x4000;
                if(instance->state) instance->state->core_flags|=0x80000000u;
            }
        }
    }
        break;
    case 2:

    {
        short i=0;
        int index,color;
        short *destination=D_8019A890;
        GteMatrix *transforms;
        transforms=(GteMatrix *)instance->render_object.matrices;
        for(;i<3;i++) destination[i]=(transforms[11].t[i]+transforms[15].t[i])>>1;
        index=D_800E11EA;
        D_800F3368.parameter00=32; D_800F3368.parameter02=2;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        color=D_800E2850[index];
        D_800F3368.palette=3; D_800F3368.parameter06=0;
        D_800F3368.parameter0A=0; D_800F3368.depth=0;
        D_800F3368.tpage=color;
    }
    }
    return 0;
}
