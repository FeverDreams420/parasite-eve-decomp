#include "pe1/field_sprite_params.h"
#include "room_m273.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/psyq_gpu.h"
#include "pe1/field_texture_tables.h"

int func_80195984(int mode, RoomM273PulseInput *input)
{
    GteShortVector position;
    int frame;
    int palette;
    int size;
    int sample;
    u16 clut;

    if (mode == 1) {
        if (D_800E27EC >= 16) return 1;
        input->position.pad += input->velocity;
        input->velocity += 2;
    } else if (mode == 2) {
        frame = D_800E27EC - 1;
        sample = D_800966EC[(((unsigned int)frame << 8) & 0x3F00) / sizeof(RoomM273TrigEntry)].low;
        position = input->position;
        palette = D_800E1204[D_800F3368.palette];
        size = sample * 2 + 2048;
        if (D_800F3368.palette == 4 && D_800F3428) palette += 4;
        clut = GetClut(16, palette);
        func_800CEE20(&position, 0, (s16)size, (s16)size,
                       (s16)D_800F3368.parameter02 * (frame / 2) + 128, clut, 1,
                       D_800966EC[(((unsigned int)frame << 8) & 0x3F00) / sizeof(RoomM273TrigEntry)].high >> 5, 0);
        position.y -= input->position.pad;
        {
            int clut2;
            palette = D_800E1204[D_800F3368.palette];
            if (D_800F3368.palette == 4 && D_800F3428) palette += 4;
            clut2 = GetClut(64, palette);
            func_800CEE20(&position, 0, 8192, 8192,
                           (s16)D_800F3368.parameter02 * D_8019ACC0[frame / 4], clut2, 1,
                           (((s32 *)D_800966EC)[(((unsigned int)frame << 9) & 0x3E00) / 4] << 16) >> 21, 0);
        }
    }
    return 0;
}

extern unsigned char D_8019AEF8;
extern short D_8019AEE8;
extern void *D_8019AE7C;
extern unsigned short D_800E11EA;
extern int func_80195984(),func_8019A290();
extern int func_800CE688(void *);

int func_80195BD0(int mode) {
    /* Layout only: purpose of these eight original frame bytes is unknown. */
    int unknownFrame[2];
    switch(mode) {
    case 0: {
        int size=func_800CE560(ANIM_POOL_AS(RoomM273PoolContext)->pool,12,10,
                               (FieldAnimCallbackListCallback)func_80195984);
        asm("" : : "r"(size));
        return size+func_800CE5AC(&D_8019AE7C,size,4,10,func_8019A290);
    }
    case 1: {
        int i;
        register unsigned char *stopped asm("$3")=&D_8019AEF8;
        if(*stopped) {
            func_800CE688(D_8019AE7C);
            return 2;
        }
        i=0;
        if(D_8019AEE8>0) {
            /* The stop symbol is the byte at offset 0x40 of this buffer. */
            register RoomM273PairedPulseBatch *batch asm("$19")=
                (RoomM273PairedPulseBatch *)(stopped-PE1_OFFSETOF(RoomM273PairedPulseBatch, stopped));
            unsigned short *x=batch->x;
            int count;
            do {
                RoomM273PulseEmitterView *effect=func_800CE610(ANIM_POOL_AS(RoomM273PoolContext)->pool);
                RoomM273PulseEmitterView **pointer;
                register int offset asm("$4");
                unsigned short y;
                if(!effect) break;
                pointer=func_800CE610(D_8019AE7C);
                offset=i*2;
                asm("" : : "r"(offset) : "memory");
                effect->x=*x;
                y=*(unsigned short *)((char *)batch+offset+16);
                asm("" : "=r"(offset) : "0"(offset));
                effect->y=y-128;
                effect->z=*(unsigned short *)((char *)batch+offset+32);
                effect->phase=0;
                effect->size=128;
                *pointer=effect;
                count=batch->count;
                asm("" : "=r"(i) : "0"(i), "r"(x) : "memory");
                ++i;
                ++x;
            } while(i<count);
        }
        D_8019AEE8=0;
        func_800CE688(D_8019AE7C);
        break;
    }
    case 2: {
        unsigned int index;
        unsigned short palette;
        func_800CE78C(D_8019AE7C);
        index=D_800E11EA;
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
