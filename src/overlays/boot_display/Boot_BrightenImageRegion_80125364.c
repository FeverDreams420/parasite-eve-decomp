#include "pe1/boot_display.h"

void func_80125364(void)
{
    s16 line;
    register u8 *image asm("$14") = (u8 *)D_80172C98 + 0x4000;
    register u8 *screen asm("$13") = *(u8 **)((u8 *)&D_80125B88 + g_BootDisplayBufferIndex * 16) + 0x19DAC;

    for (line = 110; line < 134; line++) {
        s16 row;
        u16 *source = (u16 *)image;
        u8 *pixel = screen;

        for (row = 0; row < 24; row++) {
            u32 bits = *source;
            if (bits) {
                s16 column;
                column = 0;
                for (; column < 4; column++) {
                    int index = bits & 15;
                    if (index) {
                        int brightness = (((u16 *)D_80172C9C)[index] & 31) << 3;
                        u32 blue = pixel[2];
                        register u32 green asm("$5") = pixel[1];
                        u32 red = pixel[0];
                        blue += brightness;
                        if (blue >= 256)
                            blue = 255;
                        green += brightness;
                        if (green >= 256)
                            green = 255;
                        red += brightness;
                        if (red >= 256)
                            red = 255;
                        pixel[2] = blue;
                        pixel[1] = green;
                        pixel[0] = red;
                    }
                    bits >>= 4;
                    pixel += 3;
                }
            } else {
                pixel += 12;
            }
            source++;
        }
        image += 0x80;
        screen += 0x3C0;
    }
}
