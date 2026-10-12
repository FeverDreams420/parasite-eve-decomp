#include "pe1/battle_status.h"
#include "pe1/global_slot.h"

#include "pe1/battle_main.h"

void Battle_DrawStatusValue(int value, int yOffset)
{
    int drawSlot;
    int anchorOffset;
    int markerOffset;
    u8 *markerBase;
    int activeSlot;
    BattleStatusMarkerBody *marker;
    int savedValue;
    register u64 x;
    u16 y;
    u32 drawSlotOffset;

    activeSlot = g_ActiveDrawSlot;
    savedValue = value;
    drawSlot = activeSlot;
    markerBase = (u8 *)D_8009E888;
    anchorOffset = (drawSlot * 3) << 4;
    markerOffset = drawSlot * sizeof(BattleStatusMarkerBody);
    do {
        do {
            marker = (BattleStatusMarkerBody *)(markerBase + markerOffset);
        } while (0);
        drawSlotOffset = drawSlot << 2;
        markerBase -= 8;
        {
            BattleStatusPrimAddress anchor;

            anchor.line = D_8009E358;
            anchor.address = anchorOffset + anchor.address;
            y = anchor.line->y0;
        }
        x = (marker->x =
            *(u16 *)(D_8009E360 + anchorOffset) + 8);
        y += yOffset;
    } while (0);
    marker->y = y;

    {
        BattleStatusPrimAddress entry;

        entry.ordering = D_800B0E38.ordering;
        entry.address = drawSlotOffset + entry.address;
        AddPrim(*entry.ordering + 4, (unsigned int *)(markerBase + markerOffset));
    }
    drawSlot = g_ActiveDrawSlot;
    Battle_DrawDecimalNumber(
        D_8009E8B8[drawSlot],
        D_8009E358[drawSlot * 3].x0 + 0x40,
        (s16)--y, (s16)savedValue, 1);
}

int Battle_DrawDecimalNumber(void *buffer, short x, short y, short value, short mode)
{
    unsigned char digits[6];
    signed char index = 0;
    signed char last;
    short remaining;
    short quotient;
    BattleGaugePrim *packet;
    /* Collect least-significant digits, then draw from the highest place. */
    remaining = value;
    do {
        quotient = remaining / 10;
        digits[index] = remaining - quotient * 10;
        remaining = quotient;
        if (!remaining) {
            break;
        }
        ++index;
    } while (1);
    last = index;
    x -= last * 6;
    for (; index >= 0; --index) {
        unsigned digit;
        /* Retail computes the packet address as a 32-bit integer sum. */
        {
            BattleStatusPrimAddress at;

            at.words = buffer;
            at.address = index * sizeof(*packet) + at.address;
            packet = at.gauge;
        }
        digit = digits[index];
        packet->sprite.v = 0xf2;
        packet->sprite.x = x;
        packet->sprite.y = y;
        packet->sprite.u = digit * 8;
        if (mode == 1) {
            packet->sprite.color.bytes.r = D_8009E460[g_ActiveDrawSlot].sprite.color.bytes.r + 40;
            packet->sprite.color.bytes.g = D_8009E460[g_ActiveDrawSlot].sprite.color.bytes.g;
            packet->sprite.color.bytes.b = D_8009E460[g_ActiveDrawSlot].sprite.color.bytes.b;
        }
        x += 6;
        AddPrim((unsigned *)D_800B0E38.ordering[g_ActiveDrawSlot] + 4, (unsigned *)packet);
    }
    return last;
}
