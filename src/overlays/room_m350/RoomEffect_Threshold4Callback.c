#include "pe1/field_anim_callback_list.h"
#include "pe1/field_sprite_draw.h"
#include "RoomEffect_Threshold4Callback_externs.h"

int func_8019A134(int event, int **position) {
    short point[4];
    int shade;
    int *colors;
    if (event == 1) {
        if (D_800E27EC >= 4) return 1;
    } else if (event == 2) {
        point[0] = (*position)[0];
        point[1] = (*position)[1];
        point[2] = (*position)[2];
        shade = D_800966EE[((D_800E27EC - 1) << 9) & 0x1E00];
        colors = D_8019A614;
        func_800D004C((struct GteShortVector *)point, 512, 512, 16, 0, (short)shade, (short)shade,
                     (struct RenderColor *)colors, (struct RenderColor *)(colors + 1), (short)shade >> 5, 1);
    }
    return 0;
}
