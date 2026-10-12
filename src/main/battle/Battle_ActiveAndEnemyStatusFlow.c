/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
/* MASPSX_FORCE_G0: 1 */
/* ASPSX_VERSION: 2.70 */
#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"
#include "pe1/battle_update.h"

/* COMMON supplies the small-data metadata of the window origin to MASPSX. */
volatile u16 D_8009CE84;

/* Clamp the active combatant's gauges, draw the ATB and PE bars and the
 * status background, then the HP numbers and status icons. Each background
 * branch writes the whole tile; the compiler merges their common tails. */
void Battle_DrawActiveStatus(void)
{
    s16 maximumHP;
    s32 maximumGauge;
    s32 currentGauge;
    s32 atbWidth;
    s32 peWidth;
    s32 peProduct;
    RenderColorTilePacket *atbTrack;
    RenderGouraudQuad *atbFill;
    RenderSpritePacket *atbMarker;
    RenderSpritePacket *atbLabel;
    RenderGouraudQuad *peFill;
    RenderGouraudQuad *peRemainder;
    RenderSpritePacket *peLabel;
    BattleTileSlot *tiles;

    if (!(D_8009D1A0 & 2)) {
        D_8009D278 = D_8009D254->core;
    }
    if ((u16)D_8009D278->hpAlive >= 9001) {
        D_8009D278->hpAlive = 9000;
    }
    currentGauge = D_8009D278->exp_or_acc;
    maximumGauge = D_8009D278->maxAtk;
    if (maximumGauge < currentGauge) {
        D_8009D278->exp_or_acc = maximumGauge;
    } else if (currentGauge < 0) {
        D_8009D278->exp_or_acc = 0;
    }
    maximumHP = D_8009D278->maxHP;
    if (maximumHP < (s16)D_8009D278->curHP) {
        D_8009D278->curHP = maximumHP;
    }
    peProduct = (s16)(D_8009D278->exp_or_acc >> 16) * 0x38;
    atbWidth = (D_8009D278->hpAlive * 0x38) / 9000;
    peWidth = peProduct / (s16)(D_8009D278->maxAtk >> 16);
    if (D_8009D1A0 & 2) {
        atbTrack = &D_8009E098[D_8009CDDC];
        atbTrack->x0 = g_HudOrigin.x + 8;
        atbTrack->y0 = g_HudOrigin.y + 4;
        atbFill = &D_800B00E8[D_8009CDDC];
        atbFill->x0 = g_HudOrigin.x + 8;
        atbFill->y0 = g_HudOrigin.y + 4;
        atbFill->x1 = g_HudOrigin.x + 8 + atbWidth;
        atbFill->y1 = g_HudOrigin.y + 4;
        atbFill->x2 = g_HudOrigin.x + 8;
        atbFill->y2 = g_HudOrigin.y + 7;
        atbFill->x3 = g_HudOrigin.x + 8 + atbWidth;
        atbFill->y3 = g_HudOrigin.y + 7;
        atbMarker = &D_800B6920[D_8009CDDC].sprite;
        atbMarker->x = D_800B00E8[D_8009CDDC].x1;
        atbMarker->y = D_800B00E8[D_8009CDDC].y1 - 2;
        atbLabel = &D_8009E0B8[D_8009CDDC].sprite;
        atbLabel->x = g_HudOrigin.x + 0x44;
        atbLabel->y = g_HudOrigin.y + 3;
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 6, (u32 *)atbTrack);
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 5, (u32 *)&D_800B00E8[D_8009CDDC]);
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 4, (u32 *)&D_800B6920[D_8009CDDC]);
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 4, (u32 *)&D_8009E0B8[D_8009CDDC]);
    }
    if (Menu_GetItemContextFlag() & 2) {
        peFill = &D_800B0130[D_8009CDDC][0];
        peFill->x0 = g_HudOrigin.x + 8;
        peFill->y0 = g_HudOrigin.y + 0x19;
        peFill->x1 = g_HudOrigin.x + 8 + peWidth;
        peFill->y1 = g_HudOrigin.y + 0x19;
        peFill->x2 = g_HudOrigin.x + 8;
        peFill->y2 = g_HudOrigin.y + 0x1C;
        peFill->x3 = g_HudOrigin.x + 8 + peWidth;
        peFill->y3 = g_HudOrigin.y + 0x1C;
        peRemainder = &D_800B0130[D_8009CDDC][1];
        peRemainder->x0 = D_800B0130[D_8009CDDC][0].x1;
        peRemainder->y0 = g_HudOrigin.y + 0x19;
        peRemainder->x1 = (u16)D_800B0130[D_8009CDDC][0].x1 + 0x38 - peWidth;
        peRemainder->y1 = g_HudOrigin.y + 0x19;
        peRemainder->x2 = D_800B0130[D_8009CDDC][0].x1;
        peRemainder->y2 = g_HudOrigin.y + 0x1C;
        peLabel = &D_8009E320[D_8009CDDC].sprite;
        peRemainder->x3 = (u16)D_800B0130[D_8009CDDC][0].x1 + 0x38 - peWidth;
        peRemainder->y3 = g_HudOrigin.y + 0x1C;
        peLabel->x = g_HudOrigin.x + 0x44;
        peLabel->y = g_HudOrigin.y + 0x18;
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 5, (u32 *)peFill);
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 5, (u32 *)&D_800B0130[D_8009CDDC][1]);
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 4, (u32 *)&D_8009E320[D_8009CDDC]);
        if (D_8009D1A0 & 2) {
            tiles = D_8009E070;
            tiles[D_8009CDDC].tile.x0 = g_HudOrigin.x;
            tiles[D_8009CDDC].tile.y0 = g_HudOrigin.y;
            tiles[D_8009CDDC].tile.w = 0x54;
            tiles[D_8009CDDC].tile.h = 0x20;
        } else {
            tiles = D_8009E070;
            tiles[D_8009CDDC].tile.x0 = g_HudOrigin.x;
            tiles[D_8009CDDC].tile.y0 = g_HudOrigin.y + 7;
            tiles[D_8009CDDC].tile.w = 0x54;
            tiles[D_8009CDDC].tile.h = 0x19;
        }
    } else if (D_8009D1A0 & 2) {
        tiles = D_8009E070;
        tiles[D_8009CDDC].tile.x0 = g_HudOrigin.x;
        tiles[D_8009CDDC].tile.y0 = g_HudOrigin.y;
        tiles[D_8009CDDC].tile.w = 0x54;
        tiles[D_8009CDDC].tile.h = 0x19;
    } else {
        tiles = D_8009E070;
        tiles[D_8009CDDC].tile.x0 = g_HudOrigin.x;
        tiles[D_8009CDDC].tile.y0 = g_HudOrigin.y + 7;
        tiles[D_8009CDDC].tile.w = 0x54;
        tiles[D_8009CDDC].tile.h = 0x12;
    }
    AddPrim(D_800B0E38.ordering[D_8009CDDC] + 7, (u32 *)&D_8009E068[D_8009CDDC]);
    Battle_DrawEnemyHP(D_8009D278->maxHP, D_8009D278->curHP);
    Gpu_DrawStatusIcons();
}


/* Digit sprites of the current draw buffer. Every use indexes again: the
 * byte stores to the packets make GCC reload the draw slot. */
#define MAX_HP_DIGIT(i) (&D_8009E1D8[D_8009CDDC][i].sprite)
#define CURRENT_HP_DIGIT(i) (&D_8009E0F8[D_8009CDDC][i].sprite)

/* Draw current/maximum HP from reversed decimal digits and the HP label.
 * The maximum is drawn right to left after a slash, the current value is
 * right-aligned against it; flashing digits mark the status flags. */
void Battle_DrawEnemyHP(s16 maximum, s16 current)
{
    u8 digits[8];
    s16 y;
    s16 x;
    s8 n;
    s8 last;
    s8 missing;
    s16 quotient;
    u8 pulse;
    RenderSpritePacket *label;

    y = g_HudOrigin.y + 11;
    n = 0;
    for (;;) {
        quotient = maximum / 10;
        digits[n] = maximum - quotient * 10;
        maximum = quotient;
        if (maximum == 0)
            break;
        n++;
    }
    n++;
    digits[n] = 10;
    last = n;
    x = g_HudOrigin.x - n * 6 + 56;
    for (; n >= 0; n--) {
        MAX_HP_DIGIT(n)->u = digits[n] * 8;
        MAX_HP_DIGIT(n)->v = 0xE8;
        MAX_HP_DIGIT(n)->x = x;
        MAX_HP_DIGIT(n)->y = y;
        if (!(((Combatant *)D_8009D254->core)->stateFlags & 0x800) || n == last) {
            MAX_HP_DIGIT(n)->color.bytes.r = 128;
            MAX_HP_DIGIT(n)->color.bytes.g = 128;
            MAX_HP_DIGIT(n)->color.bytes.b = 128;
        } else {
            pulse = D_8009D1E8 & 31;
            if (pulse >= 17)
                pulse = 32 - pulse;
            MAX_HP_DIGIT(n)->color.bytes.r = pulse * 6 + 32;
            MAX_HP_DIGIT(n)->color.bytes.g = pulse + 96;
            MAX_HP_DIGIT(n)->color.bytes.b = 128;
        }
        x += 6;
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 5, (u32 *)&D_8009E1D0[D_8009CDDC][n]);
    }

    x = g_HudOrigin.x - (last * 2 + 1) * 6 + 62;
    n = 0;
    for (;;) {
        quotient = current / 10;
        digits[n] = current - quotient * 10;
        current = quotient;
        if (current == 0)
            break;
        n++;
    }
    missing = last - n - 1;
    if (missing > 0)
        x += missing * 6;
    for (; n >= 0; n--) {
        CURRENT_HP_DIGIT(n)->u = digits[n] * 8;
        CURRENT_HP_DIGIT(n)->v = 0xE8;
        CURRENT_HP_DIGIT(n)->x = x;
        CURRENT_HP_DIGIT(n)->y = y;
        if (((Combatant *)D_8009D254->core)->stateFlags & 0x400) {
            pulse = D_8009D1E8 & 31;
            if (pulse >= 17)
                pulse = 32 - pulse;
            CURRENT_HP_DIGIT(n)->color.bytes.r = pulse * 8;
            CURRENT_HP_DIGIT(n)->color.bytes.g = 128;
            CURRENT_HP_DIGIT(n)->color.bytes.b = pulse * 6 + 32;
        } else {
            CURRENT_HP_DIGIT(n)->color.bytes.r = 128;
            CURRENT_HP_DIGIT(n)->color.bytes.g = 128;
            CURRENT_HP_DIGIT(n)->color.bytes.b = 128;
        }
        x += 6;
        AddPrim(D_800B0E38.ordering[D_8009CDDC] + 5, (u32 *)&D_8009E0F0[D_8009CDDC][n]);
    }

    /* Place the HP label after the number pair. */
    {
        s32 frame = D_8009CDDC;
        RenderSpritePacket *sprites = &D_8009E2E8[0].sprite;

        /* Step the sprite pointer by whole packets. */
        label = (RenderSpritePacket *)((RenderPagedSpritePacket *)sprites + frame);
        label->x = g_HudOrigin.x + 0x44;
        label->y = g_HudOrigin.y + 0xE;
        AddPrim(D_800B0E38.ordering[frame] + 4, (u32 *)&D_8009E2E8[frame]);
    }
}
