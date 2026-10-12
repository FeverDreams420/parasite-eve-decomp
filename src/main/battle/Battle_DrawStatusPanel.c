/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_status.h"

/* COMMON supplies small-data metadata to MASPSX; the draw slot stays
 * absolute. */
s32 D_8009D230;

/* Digit sprite i of the current draw buffer and panel slot. Every use
 * indexes again: the byte stores make GCC reload both indices. */
#define PANEL_DIGIT(i) (&D_800B01C0[D_8009CDDC][D_8009D230][i].sprite)

/* Draw a floating damage/recovery number, or the enemy MISS label.
 * Each frame has ten panel slots (0x578 / 0x8C), each with five 28-byte
 * texture-page/sprite packets. Retail sizes the digit buffer at 56 bytes
 * although a 16-bit value needs at most five digits.
 * Matching debt: the MISS branch keeps seven pins and two barriers; the
 * crutch-free form wastes stack slots (see CODE_ORGANIZATION.md).
 */
void Battle_DrawStatusPanel(s32 mode, BattleStatusPanel *input) {
    BattleStatusPanel *panel = input;
    u8 digits[56];
    s32 screenY;
    s16 panelValue;
    register s32 number asm("$6");
    s32 screenX;
    s32 missFrameOffset;
    s32 panelOffset;
    s32 timer;
    register RenderSpritePacket *missSprite asm("$2");
    s16 value;
    s16 quotient;
    s8 n;
    s32 rise;

    panelValue = panel->value;
    number = panelValue;
    if ((panelValue < 0) && (number = 0, ((mode & 0xFF) == 1))) {
        register s32 frame asm("$2") = D_8009CDDC;
        register s32 slotIndex asm("$4") = D_8009D230;
        register s32 x asm("$6") = panel->x;
        register s32 y asm("$7");
        timer = panel->timer;
        y = panel->y;
        *(D_800B01CC + ((slotIndex * 0x8C) + (frame * 0x578))) = timer * 4;
        asm("" : : : "memory");
        screenX = x - 16;
        asm("" : : "r"(screenX));
        timer -= 30;
        screenY = y + timer;
        *(D_800B01CD + ((D_8009D230 * 0x8C) + (D_8009CDDC * 0x578))) = 0;
        *(D_800B01CE + ((D_8009D230 * 0x8C) + (D_8009CDDC * 0x578))) = 0;
        *(D_800B01D4 + ((D_8009D230 * 0x8C) + (D_8009CDDC * 0x578))) = 0x50;
        *(D_800B01D5 + ((D_8009D230 * 0x8C) + (D_8009CDDC * 0x578))) = 0xE0;
        {
            register s32 slot asm("$7") = D_8009CDDC;
            missFrameOffset = slot * 0x578;
            panelOffset = D_8009D230 * 0x8C;
            missSprite = (RenderSpritePacket *)(missFrameOffset + panelOffset + ((u8 *)D_800B01C8));
            missSprite->width = 0x18;
            missSprite->x = screenX;
            missSprite->y = screenY;
            missSprite->height = 8;
            AddPrim(((u32 *)D_800B0E38.ordering[slot]) + 7, panelOffset + ((((u8 *)D_800B01C8) - 8) + missFrameOffset));
        }
    } else {
        value = number;
        n = 0;
        for (;;) {
            quotient = value / 10;
            digits[n] = value - quotient * 10;
            value = quotient;
            if (value == 0)
                break;
            n++;
        }
        screenX = panel->x - (n + 1) * 4;
        rise = panel->timer - 30;
        screenY = panel->y + rise;
        for (; n >= 0; n--) {
            switch (panel->style) {
            case 0:
                PANEL_DIGIT(n)->color.bytes.r = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.g = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.b = panel->timer * 4;
                break;
            case 1:
                PANEL_DIGIT(n)->color.bytes.r = 0;
                PANEL_DIGIT(n)->color.bytes.g = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.b = 0;
                break;
            case 2:
                PANEL_DIGIT(n)->color.bytes.r = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.g = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.b = 0;
                break;
            case 3:
                PANEL_DIGIT(n)->color.bytes.r = panel->timer * 4;
                PANEL_DIGIT(n)->color.bytes.g = 0;
                PANEL_DIGIT(n)->color.bytes.b = panel->timer * 4;
                break;
            }
            screenX += 8;
            PANEL_DIGIT(n)->u = digits[n] * 8;
            PANEL_DIGIT(n)->v = 0xE0;
            PANEL_DIGIT(n)->x = screenX;
            PANEL_DIGIT(n)->y = screenY;
            PANEL_DIGIT(n)->width = 8;
            PANEL_DIGIT(n)->height = 8;
            AddPrim(D_800B0E38.ordering[D_8009CDDC] + 7, (u32 *)&D_800B01C0[D_8009CDDC][D_8009D230][n]);
        }
    }
    D_8009D230 += 1;
}
