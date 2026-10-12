#include "pe1/boot_display.h"

void BootDisplay_QueueTintedSprite(u16 x, u16 y, u8 color) {
    char *primitive = g_BootDisplayPrimitiveCursor;
    s32 packed = BootDisplay_GetGlyphIndex(color);

    func_8012462C(primitive, x, y, (packed & 15) << 4, packed & 0xF0);
    func_80077AC4(g_BootDisplayOrderingTable + 0x2C, primitive);
    g_BootDisplayPrimitiveCursor += 0x1C;
}

void BootDisplay_InitSprite16(BootDisplaySpritePacket *primitive, s16 x, s16 y,
                       u16 u, u16 v) {
    volatile BootDisplaySpritePacket *packet = primitive;

    SetDrawTPage(&primitive->draw_mode, 0, 0, g_BootDisplayTPage);
    ((u8 *)&primitive->sprite.tag)[3] = 4;
    primitive->sprite.code = 100;
    func_80077B04(&primitive->sprite, 1);
    func_80077CB4(&primitive->draw_mode, &primitive->sprite);
    packet->sprite.w = 16;
    packet->sprite.h = 16;
    packet->sprite.x0 = x;
    packet->sprite.y0 = y;
    packet->sprite.u0 = u;
    packet->sprite.v0 = v;
    packet->sprite.r0 = 255;
    packet->sprite.g0 = 255;
    packet->sprite.b0 = 255;
    packet->sprite.clut = g_BootDisplayClut;
}

void func_80124604(s32 color) {
    BootDisplaySpritePacket *primitive = (BootDisplaySpritePacket *)g_BootDisplayPrimitiveCursor;

    SetDrawTPage(&primitive->draw_mode, 0, 0, g_BootDisplayTPage);
    ((u8 *)&primitive->sprite.tag)[3] = 4;
    primitive->sprite.code = 100;
    func_80077B04(&primitive->sprite, 1);
    func_80077CB4(&primitive->draw_mode, &primitive->sprite);

    primitive->sprite.x0 = 120;
    primitive->sprite.y0 = 110;
    primitive->sprite.u0 = 0;
    primitive->sprite.v0 = 0x80;
    primitive->sprite.r0 = (u8)color;
    primitive->sprite.g0 = (u8)color;
    primitive->sprite.b0 = (u8)color;
    primitive->sprite.w = 0x60;
    primitive->sprite.h = 0x18;
    primitive->sprite.clut = g_BootDisplayClut;
    func_80077AC4(g_BootDisplayOrderingTable + 0x2C, primitive);
    g_BootDisplayPrimitiveCursor += 0x1C;
}

void func_801246F4(void) {
    func_80074DC0(0);
    func_80073A44(2);
    func_80074A44(1);
    PutDispEnv(&D_800BCE80[g_ActiveDrawSlot]);
    func_800754E4(g_BootDisplayOrderingTable + 0x3C,
                  &D_800BCDC8[g_ActiveDrawSlot]);
    g_ActiveDrawSlot ^= 1;
}

void BootDisplay_InitDisplayBuffers(void) {
    s16 rect[4];
    u16 line;
    /* Keep the original buffer offset in v0 across both address calculations. */
    register s32 delta asm("$2");
    char *base;
    char *start;
    char *end;
    s32 offset;
    u16 *screenY;
    void *resource;

    func_80074A44(1);
    func_80074BB8(0);
    func_80074D28(0);

    screenY = &D_80093168;
    g_BootDisplayTextScrollX = 0xE000;
    g_BootDisplayTextScrollStepX = -0x100;
    rect[0] = 0;
    rect[1] = 0;
    rect[2] = 0x3FF;
    rect[3] = 0x1FF;
    g_BootDisplayBufferIndex = 0;
    g_BootDisplayState = 0;
    g_BootDisplayTransferProgress = 0;
    g_BootDisplayStateTimer = 0;
    g_BootDisplayStateTimer = 0;

    line = *screenY;
    base = D_80011610;
    delta = (line - D_80093166) << 11;
    start = base + delta;
    delta += 0x54000;
    end = D_80011610 + delta;
    D_80125B88[0] = start;
    D_80125B90 = end;
    D_80125B98 = end;
    D_80125BA0 = start;
    ClearImage(rect, 0, 0, 0);
    func_80074DC0(0);
    func_8010BE3C(0);
    offset = 0x43000;
    func_8010BD4C(((*screenY - D_80093166) << 11) + offset + D_80011610);

    resource = D_8012682C;
    D_80172C98 = func_80071994(resource);
    D_80172C9C = func_800719C4(resource);
    D_80172C90 = func_80071964(resource);
    D_80172C94 = func_80071944(resource);
    g_BootDisplayTPage = GetTPage(0, 0, D_80172C90[0], D_80172C90[1]);
    g_BootDisplayClut = GetClut(D_80172C94[0], D_80172C94[1]);
    func_80124F40(0);
    func_80124F40(1);
    PutDispEnv(&D_800BCE80[g_ActiveDrawSlot]);
    func_80074D28(1);
}
