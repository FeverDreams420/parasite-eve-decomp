#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
extern s32 D_800966EC[];
extern void D_8019A410;

s32 func_80077AA4(s32 arg0, s32 arg1);

s32 func_80199B94(s32 mode, void *ctx) {
    s32 tex;

    if (mode == 1) {
        if (D_800E27EC < 8) {
            goto ret0;
        }
        goto ret1;
    }

    if (mode != 2) {
        return 0;
    }

    tex = D_800E1204[D_800F3368.palette];
    if (D_800F3368.palette == 4 && D_800F3428 != 0) {
        tex += 4;
    }

    func_800CEE20(ctx, 0, 0x1800, 0x1800, (s16)D_800F3368.parameter02 + 0xDC, (u16)func_80077AA4(0x30, tex), 1,
                  (*(s32 *)((char *)D_800966EC + (((D_800E27EC - 1) << 10) & 0x3C00)) << 16) >> 21, &D_8019A410);
ret0:
    return 0;

ret1:
    return 1;
}
