#include "common.h"
#include "pe1/gte.h"
#include "pe1/render_lighting.h"

/* Load the GTE back colour (scaled by 16) and clear the nine-halfword
 * caller block that mirrors it. */
void Gte_SetBackColor(u16 *dst, int r, int g, int b) {
    gte_SetBackColor(r, g, b);

    dst[0] = 0;
    dst[1] = 0;
    dst[2] = 0;
    dst[3] = 0;
    dst[4] = 0;
    dst[5] = 0;
    dst[6] = 0;
    dst[7] = 0;
    dst[8] = 0;
}

/* Store one light's colour in the light colour matrix column and the 8-bit
 * colour table, then reload the GTE light colour matrix. */
void Gte_SetLightColor(RenderLightingState *lighting, int index, int r, int g, u16 b) {
    if ((unsigned int)index < 3) {
        lighting->color.matrix.m[0][index] = r;
        lighting->color.matrix.m[1][index] = g;
        lighting->color.matrix.m[2][index] = b;
    }

    lighting->light_colors[index][0] = r;
    lighting->light_colors[index][1] = g;
    lighting->light_colors[index][2] = b;

    gte_SetColorMatrix(&lighting->color);
}
