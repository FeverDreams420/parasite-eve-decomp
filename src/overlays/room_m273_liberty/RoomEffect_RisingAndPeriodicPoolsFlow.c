/* The rising-sprite callback and its poll/reset controller share one TU. */
#include "pe1/field_sprite_params.h"
#include "room_m273_particles.h"
#include "room_m273_boss_state.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/field_anim_pool.h"
#include "pe1/psyq_gpu.h"

typedef RoomM273RisingParticle Particle;
typedef struct { unsigned char r, g, b, unknown; } Color;
extern Color D_8018F1E4;
#include "pe1/gte_sine_table.h"

int func_80195E10(int mode, Particle *particle) {
    Color color = D_8018F1E4;
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        else {
            unsigned short y = particle->y;
            register int speed asm("$3") = ((volatile Particle *)particle)->speed;
            int delta = ((Particle *)particle)->speed;
            speed += 1;
            particle->y = y - delta;
            particle->speed = speed;
        }
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int sample = *(short *)((char *)D_800966EC + (((unsigned int)frame << 8) & 0x3F00));
        int size, kind, palette;
        register int specialKind asm("$3") = 4;
        unsigned short clut;
        kind = D_800F3368.palette;
        size = sample * 2;
        asm("" : : "r"(size) : "memory");
        palette = D_800E1204[kind];
        if (kind == specialKind && D_800F3428) palette += 9;
        else palette += 5;
        clut = GetClut(0, palette);
        func_800CEE20((struct GteShortVector *)particle, 0, size, size, 102, clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4].word >> 6, (struct RenderColor *)&color);
    }
    return 0;
}

#include "room_m273_effects.h"

#define ROOMLIB_POLL_RESET_POOL_CONTEXT_DECL
#define ROOMLIB_POLL_RESET_POOL_EXPR ANIM_POOL_AS(RoomM273PoolContext)->pool
#define ROOMLIB_POLL_RESET_FUNC func_80195F78
#define ROOMLIB_POLL_RESET_CALLBACK func_80195E10
#define ROOMLIB_POLL_RESET_FLAG D_8019AEF8
#define ROOMLIB_POLL_RESET_COUNTER D_8019AEB2
#define ROOMLIB_POLL_RESET_SEED D_8019AEAC
#include "../room_lib/RoomLib_PollAndResetActor.inc"

#include "room_m273_effects.h"
typedef GteShortVector Vector;
typedef RoomM273EffectModeState State;
extern unsigned char D_8019AEF8;
extern void *D_8019AE80;
extern int func_8019A398(),func_8019A4CC();
extern int func_800CE688(void *);

int func_801960F4(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    switch(mode) {
    case 0: {
        int size=func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,4,6,func_8019A398);
        asm("" : : "r"(size));
        return size+func_800CE5AC(&D_8019AE80,size,4,6,func_8019A4CC);
    }
    case 1: {
        unsigned char *stopped=&D_8019AEF8;
        State *state;
        if(*stopped) return 2;
        state=ANIM_OWNER_AS(RoomM273EffectStateContext)->state.mode;
        if(state->kind==11) {
            int count=state->value22;
            unsigned short value=*(volatile unsigned short *)&state->value22;
            if(count<35 && (value&7)==0) {
                short i=0;
                /* The shared positions start 0x5C bytes before the stop symbol. */
                Vector *positions=(Vector *)(stopped-92);
                do {
                    Vector **out=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                    unsigned long address;
                    if(!out) break;
                    address=i*8;
                    address+=(unsigned long)positions;
                    *out=(Vector *)address;
                    i++;
                } while(i<2);
                i=0;
                positions=D_8019AE9C.hands;
                do {
                    Vector **out=func_800CE610(D_8019AE80);
                    unsigned long address;
                    if(!out) break;
                    address=i*8;
                    address+=(unsigned long)positions;
                    *out=(Vector *)address;
                    i++;
                } while(i<2);
            }
            func_800CE688(D_8019AE80);
        }
        break;
    }
    case 2: {
        unsigned int index;
        unsigned short palette;
        func_800CE78C(D_8019AE80);
        index=D_800E11FA;
        D_800F3368.parameter00=32; D_800F3368.parameter02=2;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        palette=D_800E2850[index];
        asm("" : : : "$2");
        D_800F3368.palette=3; D_800F3368.parameter06=1; D_800F3368.parameter0A=0; D_800F3368.depth=0;
        D_800F3368.tpage=palette;
        break;
    }
    }
    return 0;
}
