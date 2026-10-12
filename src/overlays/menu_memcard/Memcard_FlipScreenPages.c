/* CC1_FLAGS: -fno-cse-skip-blocks */
#include "common.h"
#include "pe1/boot_disc_check.h"
#include "pe1/boot_display.h"
#include "pe1/signed_rect.h"
#include "pe1/psyq_gpu.h"
#include "menu_memcard_screen.h"
#include "menu_memcard_image.h"

typedef PsxSignedRect Rect;

#include "Memcard_FlipScreenPages_externs.h"

void func_80074A44(int);

void Memcard_FlipScreenPages(void) {
    int i;
    int page;
    int y;
    MemcardScreenBuffer *screen;
    Rect main_rect;

    for (i = 0; i < 2; i++) {
        page = D_801D11C8 == 0;
        D_801D11C4 = D_801D11BC[page];
        D_801D11C8 = page;
        main_rect.x = 0;
        y = 0x14;
        if (page == 0) y = 0x104;
        main_rect.w = 0x1E0;
        main_rect.h = 0xCC;
        main_rect.y = y;
        LoadImage(&main_rect, &((u8 *)&D_80193268)[D_80193258[0]]);
        DrawSync(0);

        screen = D_801D11C4;
        if (screen->overlay.w > 0) {
            RECT clipped;
            clipped = screen->overlay;
            clipped.x = (clipped.x * 3) >> 1;
            if (D_801D11C8 == 0) clipped.y = (u16)clipped.y + 0xF0;
            clipped.w = (clipped.w * 3) >> 1;
            LoadImage(&clipped, D_801D11C4->overlayImage);
        }

        VSync(0);
        func_80074A44(1);
        PutDrawEnv(&D_801D11C4->draw);
        PutDispEnv(&D_801D11C4->disp);
    }
}
