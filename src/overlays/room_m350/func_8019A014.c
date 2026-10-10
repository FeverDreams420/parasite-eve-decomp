#include "common.h"
#include "pe1/field_sprite_params.h"
#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_texture_tables.h"
extern void D_8019A630;

s32 func_80077AA4(s32 arg0, s32 arg1);

s32 func_8019A014(s32 mode, void *ctx) {
    s32 tex;

    if (mode == 1) {
        if (D_800E27EC < 5) {
            goto ret0;
        }
        return 1;
    }

    if (mode != 2) {
        return 0;
    }

    if (D_800E27EC < 2) {
        return 0;
    }

    tex = D_800E1204[D_800F3368.palette];
    if (D_800F3368.palette == 4 && D_800F3428 != 0) {
        tex += 4;
    }

    func_800CEE20(ctx, 0, 0x1000, 0x1000, 0xDC, (u16)func_80077AA4(0x30, tex), 1, (6 - D_800E27EC) << 5, &D_8019A630);
ret0:
    return 0;
}
