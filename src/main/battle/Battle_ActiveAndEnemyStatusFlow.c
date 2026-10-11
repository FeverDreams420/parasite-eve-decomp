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
 * status background, then the HP numbers and status icons. Matching debt:
 * two v0 pins. The background pointer and the ordering-table index of the
 * last AddPrim are both born in v0 in retail; as pseudos they take a0/v1
 * and the index load moves above the background store (lev 37 without). */
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
    register RenderColorTilePacket *background asm("$2");
    u16 backgroundY;
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
            background = &tiles[D_8009CDDC].tile;
            background->x0 = g_HudOrigin.x;
            backgroundY = g_HudOrigin.y;
            background->w = 0x54;
            background->h = 0x20;
        } else {
            tiles = D_8009E070;
            background = &tiles[D_8009CDDC].tile;
            background->x0 = g_HudOrigin.x;
            backgroundY = g_HudOrigin.y;
            background->w = 0x54;
            background->h = 0x19;
            backgroundY += 7;
        }
    } else if (D_8009D1A0 & 2) {
        tiles = D_8009E070;
        background = &tiles[D_8009CDDC].tile;
        background->x0 = g_HudOrigin.x;
        backgroundY = g_HudOrigin.y;
        background->w = 0x54;
        background->h = 0x19;
    } else {
        tiles = D_8009E070;
        background = &tiles[D_8009CDDC].tile;
        background->x0 = g_HudOrigin.x;
        backgroundY = g_HudOrigin.y;
        background->w = 0x54;
        background->h = 0x12;
        backgroundY += 7;
    }
    background->y0 = backgroundY;
    {
        register s32 index asm("$2") = D_8009CDDC;
        AddPrim(D_800B0E38.ordering[index] + 7, (u32 *)&D_8009E068[index]);
    }
    Battle_DrawEnemyHP(D_8009D278->maxHP, D_8009D278->curHP);
    Gpu_DrawStatusIcons();
}


/* Draw current/maximum HP from reversed decimal digits and the HP label.
 * Matching debt: 14 register pins and 9 empty barriers preserve retail scheduling
 * and the volatile label view the final coordinate order. */
void Battle_DrawEnemyHP(s16 maximum, s16 current)
{
    u8 digits[8];
    register u8 *sprites asm("$18");
    register s16 savedCurrent asm("$19") = current;
    s16 screenY;
    s16 screenX;
    register s32 digitIndex asm("$4");
    s32 pulseByte;
    s32 labelOffset;
    s32 maximumDigitOffset;
    register s32 currentDigitOffset asm("$6");
    register s32 shiftedIndex asm("$3");
    register s8 slashIndex asm("$4");
    s8 maximumLastIndex;
    s8 nextMaximumIndex;
    s8 missingDigits;
    s8 nextCurrentIndex;
    register s8 maximumLastDigit asm("$16");
    s8 maximumIndex;
    s8 currentIndex;
    register u32 maximumPhase asm("$3");
    register u32 currentPhase asm("$3");
    u32 maximumPulse;
    u32 currentPulse;
    volatile RenderSpritePacket *label;
    RenderSpritePacket *maximumSprite;
    RenderSpritePacket *currentSprite;

    /* Extract maximum HP digits least-significant first, then append slash. */
    asm("" : : "r"(maximum), "r"(savedCurrent));
    maximumLastDigit = 0;
    screenY = g_HudOrigin.y + 11;
    {
        s16 value = maximum;
        s16 quotient;
        s8 index = 0;
        for (;;) {
            quotient = value / 10;
            digits[index] = value - (s16)quotient * 10;
            value = quotient;
            if ((s16)value == 0)
                break;
            index = maximumLastDigit + 1;
            maximumLastDigit = index;
        }
    }
    slashIndex = maximumLastDigit + 1;
    maximumIndex = slashIndex;
    asm volatile("" : "=r"(slashIndex) : "0"(slashIndex));
    digits[slashIndex] = 0xA;
    maximumLastIndex = maximumIndex;
    screenX = (g_HudOrigin.x - (slashIndex * 6)) + 56;
    if (slashIndex >= 0) {
        u32 gray = 128;
        sprites = D_8009E1D8;
        asm volatile("" : "=r"(maximumIndex) : "0"(maximumIndex), "r"(gray));
        shiftedIndex = maximumIndex << 24;
        do {
            s32 frame = D_8009CDDC;
            s32 frameOffset;
            digitIndex = shiftedIndex >> 24;
            frameOffset = frame * 140;
            {
                s32 column = digitIndex * 7;
                maximumDigitOffset = column * 4;
            }
            ((RenderSpritePacket *)(frameOffset + maximumDigitOffset + (u32)sprites))->u = (s8) (digits[digitIndex] * 8);
            ((RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites))->v = 0xE8;
            maximumSprite = (RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites);
            maximumSprite->x = screenX;
            maximumSprite->y = screenY;
            if (!(((Combatant *)D_8009D254->core)->stateFlags & 0x800) || (digitIndex == maximumLastIndex)) {
                maximumSprite->color.bytes.r = gray;
                ((RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites))->color.bytes.g = gray;
            }
            else {
                {
                    register u32 clock asm("$2") = D_8009D1E8;
                    maximumPhase = clock & 31;
                }
                maximumPulse = maximumPhase;
                asm volatile("" : "=r"(maximumPulse) : "0"(maximumPulse));
                if (maximumPulse >= 17U) {
                    maximumPulse = 32 - maximumPhase;
                }
                maximumSprite->color.bytes.r = (s8) (((maximumPulse & 255) * 6) + 32);
                ((RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites))->color.bytes.g = (s8) (maximumPulse + 96);
            }
            ((RenderSpritePacket *)((D_8009CDDC * 140) + maximumDigitOffset + (u32)sprites))->color.bytes.b = gray;
            screenX += 6;
            {
                s32 frameEnd = D_8009CDDC;
                u32 otOffset = frameEnd * 4;
                s32 frameOffset = frameEnd * 140;
                {
                    s32 index = maximumIndex;
                    RenderTexturePagePacket *page = &((RenderPagedSpritePacket *)D_8009E1D0)[index].page;
                    AddPrim((u32 *)(*(u8 **)((u8 *)D_800B0E38.ordering + otOffset) + 20), (u32 *)(frameOffset + (u32)page));
                }
            }
            nextMaximumIndex = maximumIndex - 1;
            maximumIndex = nextMaximumIndex;
            asm volatile("" : "=r"(maximumIndex) : "0"(maximumIndex));
            shiftedIndex = maximumIndex << 24;
        } while (nextMaximumIndex >= 0);
    }

    /* Align current HP against the maximum field and draw it from left to right. */
    currentIndex = 0;
    {
        s16 x = g_HudOrigin.x - ((((maximumLastIndex << 24) >> 23) + 1) * 6);
        screenX = x + 62;
    }
    {
        s16 value = savedCurrent;
        s16 quotient;
        s8 index = 0;
        for (;;) {
            quotient = value / 10;
            digits[index] = value - (s16)quotient * 10;
            value = quotient;
            if ((s16)value == 0)
                break;
            index = currentIndex + 1;
            currentIndex = index;
        }
    }
    missingDigits = (maximumLastIndex - currentIndex) - 1;
    if (missingDigits > 0) {
        screenX += missingDigits * 6;
    }
    if (currentIndex >= 0) {
        sprites = D_8009E0F8;
        do {
            register s32 frame asm("$2") = D_8009CDDC;
            s32 frameOffset;
            digitIndex = currentIndex;
            frameOffset = frame * 112;
            asm volatile("" : "=r"(frameOffset) : "0"(frameOffset));
            {
                register s32 column asm("$2") = digitIndex * 7;
                asm("" : "=r"(column) : "0"(column));
                currentDigitOffset = column * 4;
            }
            ((RenderSpritePacket *)(frameOffset + currentDigitOffset + (u32)sprites))->u = (s8) (digits[digitIndex] * 8);
            asm volatile("" : : "r"(digitIndex));
            ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->v = 0xE8;
            currentSprite = (RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites);
            currentSprite->x = screenX;
            currentSprite->y = screenY;
            if (((Combatant *)D_8009D254->core)->stateFlags & 0x400) {
                {
                    register u32 clock asm("$2") = D_8009D1E8;
                    currentPhase = clock & 31;
                }
                currentPulse = currentPhase;
                asm volatile("" : "=r"(currentPulse) : "0"(currentPulse));
                if (currentPulse >= 17U) {
                    currentPulse = 32 - currentPhase;
                }
                pulseByte = currentPulse & 255;
                currentSprite->color.bytes.r = (s8) (pulseByte * 8);
                ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->color.bytes.g = 128;
                ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->color.bytes.b = (s8) ((pulseByte * 6) + 32);
            }
            else {
                currentSprite->color.bytes.r = 128;
                ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->color.bytes.g = 128;
                ((RenderSpritePacket *)((D_8009CDDC * 112) + currentDigitOffset + (u32)sprites))->color.bytes.b = 128;
            }
            screenX += 6;
            {
                s32 frameEnd = D_8009CDDC;
                u32 otOffset = frameEnd * 4;
                s32 frameOffset = frameEnd * 112;
                {
                    s32 index = currentIndex;
                    RenderTexturePagePacket *page = &((RenderPagedSpritePacket *)D_8009E0F0)[index].page;
                    AddPrim((u32 *)(*(u8 **)((u8 *)D_800B0E38.ordering + otOffset) + 20), (u32 *)(frameOffset + (u32)page));
                }
            }
            nextCurrentIndex = currentIndex - 1;
            currentIndex = nextCurrentIndex;
        } while (nextCurrentIndex >= 0);
    }

    /* Place the HP label after the number pair. */
    {
        register s32 frame asm("$3") = D_8009CDDC;
        labelOffset = frame * 28;
        label = (RenderSpritePacket *)(labelOffset + D_8009E2F0);
        label->x = (s16) (g_HudOrigin.x + 0x44);
        label->y = (s16) (g_HudOrigin.y + 0xE);
        AddPrim((u32 *) (D_800B0E38.ordering[frame] + 4), (u32 *)(labelOffset + (D_8009E2F0 - 8)));
    }
}
