/* CC1_FLAGS: -G8 */
/* cc1 -G8 makes the small texture slot word a cheap address, so its
 * volatile read stays an absolute load. */
#include "pe1/field_sprite_params.h"
#include "room_m350_shared.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
#include "pe1/psyq_gpu.h"

#include "RoomEffect_ConfiguredPulseCallback_externs.h"
#include "pe1/gte_sine_table.h"
#include "RoomEffect_ConfiguredPulseCallback_externs_1.h"
/* Signed high-half view avoids LHU followed by explicit sign extension. */

int func_80196C64(int event, void *object)
{
    /* Match note: retail leaves sp+0x28..0x2F unused; original local unknown. */
    char frameGap[8];
    if (event == 1) {
        if (D_800E27EC >= 8) return 1;
    } else if (event == 2) {
        volatile RenderEffectParameters *state = &D_800F3368;
        int unit = 32;
        s16 size;
        int paletteIndex;
        int sampleOffset;
        u16 palette;
        int frame;
        s16 mode;
        int clutX;
        unsigned short handle;
        asm("" : "=r"(state) : "0"(state));
        state->parameter00 = unit;
        paletteIndex = *(volatile u16 *)&D_800E11E8;
        mode = 2;
        D_800F3368.parameter02 = mode;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        D_800F3368.extent_x = unit;
        D_800F3368.extent_y = unit;
        palette = D_800E2850[paletteIndex];
        /* Capture the palette before reading the animation frame. */
        asm volatile("" : : "r"(palette));
        frame = D_800E27EC;
        clutX = 0;
        D_800F3368.palette = mode;
        D_800F3368.parameter06 = 0;
        D_800F3368.parameter0A = 0;
        D_800F3368.depth = unit;
        sampleOffset = ((unsigned int)(frame - 1) << 9) & 0x3E00;
        asm("" : "=r"(sampleOffset) : "0"(sampleOffset) : "memory");
        D_800F3368.tpage = palette;
        size = *(short *)((char *)D_800966EC + sampleOffset);
        size = size * 2 + 4096;
        handle = GetClut(clutX, D_800E1208);
        {
            int frame = D_800E27EC - 1;
            int phase = (frame >> 2) & 3;
            int *colors;
            unsigned int drawSampleOffset;
            drawSampleOffset = ((unsigned int)frame << 9) & 0x3E00;
            colors = D_8019A59C;
            func_800CEE20(object, 0, (short)size, (short)size,
                (s16)((RenderEffectParameters *)state)->parameter02 * phase + 192,
                handle, 1, D_800966EC[drawSampleOffset / 4].bits.cos >> 5, (struct RenderColor *)colors);
        }
    }
    return 0;
}
