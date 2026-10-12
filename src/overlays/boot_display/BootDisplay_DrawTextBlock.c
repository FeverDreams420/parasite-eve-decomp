#include "pe1/boot_display.h"
s32 BootDisplay_DrawTextBlock(void) {
    const u8 *line;
    u32 x = 24;
    u32 width;
    const u8 *scan;
    u32 rows = 0;
    s32 y = g_BootDisplayTextScrollX >> 8;
    g_BootDisplayTextCursor = D_80125C88;
    line = *g_BootDisplayTextCursor;
    while (*line != 255) {
        if (y >= -7) {
            if (y >= 225) break;
            for (; *line; ++line) {
                switch (*line) {
                case ' ':
                    break;
                case '\\':
                    width = 0;
                    if (line[1]) {
                        scan = line + 1;
                        do {
                            width += BootDisplay_GetGlyphWidth(*scan++);
                        } while (*scan);
                    }
                    x = (320 - width) >> 1;
                    continue;
                case '\t':
                    x += BootDisplay_GetGlyphWidth(' ') * 14;
                    continue;
                default:
                    g_BootDisplayCallbacks->glyph((s16)x, (s16)y, *line);
                    break;
                }
                x += BootDisplay_GetGlyphWidth(*line);
            }
            g_BootDisplayFontStyle = 0;
            x = 24;
            ++rows;
        }
        ++g_BootDisplayTextCursor;
        line = *g_BootDisplayTextCursor;
        y += 18;
    }
    g_BootDisplayTextScrollX += g_BootDisplayTextScrollStepX;
    return rows < 1;
}
