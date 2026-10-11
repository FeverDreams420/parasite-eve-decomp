#include "pe1/field_sprite_params.h"
#include "room_m273_effects.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/psyq_gpu.h"
/* The batch emitter fills the eight-byte records rendered by this callback. */
#include "RoomEffect_BatchRising_externs.h"
#include "pe1/gte_sine_table.h"
#include "RoomEffect_BatchRising_externs_1.h"

int func_80196F2C(int mode, GteShortVector *position) {
    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        position->y -= 6;
    } else if (mode == 2) {
        int frame = D_800E27EC - 1;
        int kind = D_800F3368.palette;
        int palette = D_800E1204[kind];
        int clut;
        if (kind == 4 && D_800F3428) palette += 4;
        clut = GetClut(64, palette);
        func_800CEE20(position, 0, 4096, 4096,
            44 + (s16)D_800F3368.parameter02 * D_8019AD68[frame / 4], clut, 1,
            (short)D_800966EC[(((unsigned int)frame << 9) & 0x3E00) / 4].word >> 5, 0);
    }
    return 0;
}

int func_8019706C(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int stack_pad[2];
    switch(mode) {
    case 0:
        return func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,8,40,
                             (FieldAnimCallbackListCallback)func_80196F2C);
    case 1: {
        int i;
        unsigned short *x,*y;
        if(D_8019AF0C.stopped) return 2;
        i=0;
        if(D_8019AF0C.count>0) {
            RoomM273RisingBatchBuffer *batch=&D_8019AF0C;
            y=batch->y;
            x=batch->x;
            do {
                int offset;
                GteShortVector *out=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                if(!out) break;
                out->x=*x;
                out->y=*y++-128;
                asm("" : "=r"(i) : "0"(i), "r"(x));
                ++x;
                offset=i*2;
                out->z=*(unsigned short *)((char *)batch+offset+48);
                out->pad=0;
                ++i;
            } while(i<batch->count);
        }
        D_8019AF0C.count=0;
        break;
    }
    case 2: {
        unsigned int index=D_800E11EA;
        unsigned short palette;
        D_800F3368.parameter00=32; D_800F3368.parameter02=2;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        D_800F3368.extent_x=32; D_800F3368.extent_y=32;
        palette=D_800E2850[index];
        D_800F3368.palette=3; D_800F3368.parameter06=0; D_800F3368.parameter0A=0; D_800F3368.depth=0;
        D_800F3368.tpage=palette;
        break;
    }
    }
    return 0;
}
