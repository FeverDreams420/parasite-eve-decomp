/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/boot_disc_check.h"
#include "pe1/field_pad.h"
#include "pe1/menu_flags.h"
#include "pe1/psyq_bios.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_prim.h"
#include "pe1/textbox.h"
#include "pe1/textbox_open.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_ScriptCollisionFlag;
u8 *g_TextboxMessageSource;
unsigned char g_TextboxKernGlyph0F;
TextboxRect g_TextboxPendingRect;
u8 g_DialogueChoiceCursor;
u8 g_PlayerMovementEnabled;

/* A draw-mode word pair followed by the sprite primitive it prefixes. */
typedef struct GpuModeSprtPacket {
    u32 tag;
    u32 mode;
    u8 sprt[1];
} GpuModeSprtPacket;

/* A draw-mode word pair followed by the tile primitive it prefixes. */
typedef struct GpuModeTilePacket {
    u32 tag;
    u32 mode;
    u8 tile[1];
} GpuModeTilePacket;

void Draw_InitDrawModeSprtPacket(GpuModeSprtPacket *packet, int tpage) {
    u8 *prim = packet->sprt;

    SetDrawTPage(packet, 0, 1, tpage);
    SetSprt(prim);
    if (MargePrim(packet, prim) != 0) {
        exit(-1);
    }
}

void Draw_InitDrawModeTilePacket(GpuModeTilePacket *packet, int tpage) {
    u8 *prim = packet->tile;

    SetDrawTPage(packet, 0, 1, tpage);
    SetTile(prim);
    if (MargePrim(packet, prim) != 0) {
        exit(-1);
    }
}

void Menu_SetEquipSlotIndex(int index) {
    g_TextboxKernGlyph0F = index;
}

void SetDrawTPage(void *, int, int, int);
void SetSprt(void *);
int MargePrim(void *, void *);
void SetShadeTex(void *, int);
void SetSemiTrans(void *, int);
void exit(int);

void Render_SetupFogLayer(void *source) {
    int i;
    u16 *page_source;
    u8 *tile_base;
    register u8 *draw_base asm("$23");
    u8 *tile_payload_base;
    int sentinel;
    u8 *sprite_base;
    int offset;
    u8 *draw_mode;
    u8 *sprite;
    RenderSpritePacket *sprite_fields;
    u8 *tile_mode;
    u8 *tile;
    u8 *payload;
    u32 flags;
    int tpage;
    int color;
    int zero_arg;
    int x_arg;
    int y_arg;
    int dither_enabled;
    u32 mask1;
    u32 mask2;
    u32 mask3;
    u32 mask4;
    u32 mask5;
    i = 0;
    mask1 =0xFFF0FFFF;
    mask2 =0xFFEFFFFF;
    mask3 =0xFFDFFFFF;
    mask4 =0xFE3FFFFF;
    sprite_base = (u8 *)&g_TextboxChoiceCursor[0].sprite;
    mask5 =0xFDFFFFFF;
    sentinel = -1;
    for (; (u8)i < 4; ++i) {
        g_TextboxEntries[(u8)i].control.parts.delay = 0;
        g_TextboxEntries[(u8)i].control.parts.elapsed = 0;
        flags = g_TextboxEntries[(u8)i].control.flags;
        g_TextboxEntries[(u8)i].state = 0;
        g_TextboxEntries[(u8)i].message = 0;
        g_TextboxEntries[(u8)i].style = 0;
        g_TextboxEntries[(u8)i].background = 0;
        g_TextboxEntries[(u8)i].page_id = sentinel;
        flags &= mask1;
        flags &= mask2;
        flags &= mask3;
        flags &= mask4;
        flags &= mask5;
        g_TextboxEntries[(u8)i].control.flags = flags;
    }
    i = 0;
    page_source = &g_TextboxFontPages[3].tpage;
    draw_base = (u8 *)g_TextboxChoiceCursor;
    tile_base = g_TextboxFogTilePackets;
    tile_payload_base = tile_base + 8;
    g_DialogueChoiceCursor = 0;
    g_PlayerMovementEnabled = 0;
    g_ScriptCollisionFlag = 0;
    g_TextboxMessageSource = source;
    for (; (u8)i < 2; ++i) {
        {
            TextboxTableAddress at;

            at.bytes = draw_base;
            at.address = (u8)i * 28 + at.address;
            draw_mode = at.bytes;
        }
        sprite = draw_mode + 8;
        SetDrawTPage(draw_mode, 0, 1, page_source[0]);
        SetSprt(sprite);
        if (MargePrim(draw_mode, sprite)) exit(-1);

        sprite_fields = (RenderSpritePacket *)(((u8)i * 28) + (int)sprite_base);
        SetShadeTex(sprite_fields, 1);
        x_arg = 0;

        y_arg = 0;

        zero_arg = 0;

        sprite_fields->u = 0x70;
        color = *(page_source - 3);
        sprite_fields->width = 0x18;
        sprite_fields->height = 0xC;
        sprite_fields->v = color;
        *(u16 *)((u8)i * 28 + (u8 *)&g_TextboxChoiceCursor[0].sprite.clut) = page_source[1];
        mask1 = x_arg;
        tpage = GetTPage(mask1, y_arg, zero_arg, 0);
        {
            TextboxTableAddress at;

            at.bytes = tile_base;
            at.address = (u8)i * 24 + at.address;
            tile_mode = at.bytes;
        }
        tile = tile_mode + 8;
        SetDrawTPage(tile_mode, 0, 1, tpage & 0xFFFF);
        SetTile(tile);
        if (MargePrim(tile_mode, tile)) exit(-1);
        payload = (u8 *)(((u8)i * 24) + (int)tile_payload_base);
        dither_enabled = 1;
        {
            int two = 2;

            payload[4] = two;
            payload[5] = two;
            payload[6] = two;
            ((RenderColorTilePacket *)payload)->w = 0x140;
            ((RenderColorTilePacket *)payload)->h = 0x36;
            ((RenderColorTilePacket *)payload)->x0 = 0;
            ((RenderColorTilePacket *)payload)->y0 = 0xAA;
            SetSemiTrans(payload, dither_enabled);
        }
    }
}

void Menu_SetTextCursorRect(int x, int y, int w, int h) {
    g_TextboxPendingRect.x = x;
    g_TextboxPendingRect.y = y;
    g_TextboxPendingRect.width = w;
    g_TextboxPendingRect.height = h;
}

void Draw_CloseTextboxPage(int arg0) {
    int i;
    int idx;

    i = 0;
    arg0 = (s16)arg0;
    while ((unsigned char)i < 4) {
        idx = (unsigned char)i;
        if (g_TextboxEntries[idx].page_id == arg0) {
            if (g_TextboxEntries[idx].state != 0) {
                g_TextboxEntries[idx].state = 0;
                break;
            }
        }
        i++;
    }
}

void Draw_ResetTextboxes(void) {
    int i;
    int idx;
    u32 value;

    for (i = 0; (unsigned char)i < 4; i++) {
        idx = (unsigned char)i;
        value = g_TextboxEntries[idx].control.flags;
        g_TextboxEntries[idx].state = 0;
        value &= 0xFDFFFFFF;
        g_TextboxEntries[idx].control.flags = value;
    }
}

s8 Draw_GetTextboxPageState(int arg0) {
    int i;
    int idx;
    int value;

    value = 0;
    i = 0;
    arg0 = (s16)arg0;
    while ((unsigned char)i < 4) {
        idx = (unsigned char)i;
        if (g_TextboxEntries[idx].page_id == arg0) {
            value = g_TextboxEntries[idx].state;
            break;
        }
        i++;
    }
    return value;
}

void Script_EnableMovement(void) {
    g_PlayerMovementEnabled = 1;
}

void Script_DisableMovement(void) {
    g_PlayerMovementEnabled = 0;
}

void Script_SetCollisionFlag(int value) {
    g_ScriptCollisionFlag = value != 0;
}

/* Opens the first free textbox for message `index`. A styled box takes the
 * current text rectangle; `values` (ended by -1, at most five) fill the
 * message's number slots as decimal digits.
 * Matching debt: five register pins and three empty barriers. The reciprocal
 * operand preserves constant-hoist order; the digit memory operand keeps its
 * base across the inner loop; the final count operand preserves the counter.
 * The page-index register is reused for slot initialization. No CPU ASM. */
void Render_SetupColorTable(int inputIndex, int inputStyle, short *inputValues)
{
    unsigned char style = inputStyle;
    register short *values asm("$12") = inputValues;
    unsigned char i;
    register short index asm("$9") = inputIndex;

    for (i = 0; i < 4; i++) {
        unsigned char slot;

        if (g_TextboxEntries[i].state != 0)
            continue;

        g_TextboxEntries[i].state = 1;
        g_TextboxEntries[i].background = 0;
        g_TextboxEntries[i].page_id = index;
        g_DialogueChoiceCursor = 0;
        {
            int terminator = -1;
            g_MenuEquipSlotIndex = terminator;
        }
        g_TextboxEntries[i].style = style;
        g_TextboxEntries[i].control.flags &= ~0x100000;
        {
            int reciprocal = 0x66666667;
            asm("" : : "r"(reciprocal));
        }
        g_TextboxEntries[i].control.flags &= ~0x200000;
        if (style) {
            g_TextboxEntries[i].x = g_TextboxPendingRect.x;
            g_TextboxEntries[i].y = g_TextboxPendingRect.y;
            g_TextboxEntries[i].width = g_TextboxPendingRect.width;
            g_TextboxEntries[i].height = g_TextboxPendingRect.height;
            if (g_TextboxEntries[i].style == 3)
                g_TextboxEntries[i].control.flags |= 0x100000;
        } else if (g_PlayerMovementEnabled) {
            g_TextboxEntries[i].background = 1;
        }

        index = 0;
        for (slot = index; slot < 5; slot++) {
            register unsigned short inputNumber asm("$7") = *values++;
            short value = inputNumber;
            register unsigned short quotient asm("$5");
            register unsigned char count asm("$8");

            if (value == -1)
                return;
            count = 0;
            g_TextboxEntries[i].numbers[slot].digits[count] = value - (quotient = value / 10) * 10;
            asm volatile("" : : "m"(g_TextboxEntries[i].numbers[slot].digits[count]) : "$6");
            value = quotient;
            while (value != 0) {
                unsigned short quotient;
                quotient = value / 10;
                count++;
                g_TextboxEntries[i].numbers[slot].digits[count] = value - quotient * 10;
                value = quotient;
            }
            g_TextboxEntries[i].numbers[slot].count = count + 1;
            asm("" : : "r"(count));
        }
        return;
    }
}

int Menu_GetEquipSlotIndex(void) {
    return g_MenuEquipSlotIndex;
}

/* Tentative small-data definition: stock maspsx must recognize this store
 * as GP-relative to retain its load-delay nop. The linker pins the COMMON
 * symbol to the retail address, as for other unplaced game globals. */
signed char g_MenuEquipSlotIndex;
u8 g_TextboxGlyphColorR;
u8 g_TextboxGlyphColorG;
u8 g_TextboxGlyphColorB;

/* Stored only by Draw_SetLightingParams, which nothing calls; no reader. */
unsigned char g_DrawLightingParamsSetWriteOnly;
short g_DrawLightingParam0WriteOnly;
short g_DrawLightingParam1WriteOnly;
short g_DrawLightingParam2WriteOnly;
unsigned char g_TextboxHighlightColorR;
unsigned char g_TextboxHighlightColorG;
unsigned char g_TextboxHighlightColorB;

unsigned char g_TextboxHighlightColorR;
unsigned char g_TextboxHighlightColorG;
unsigned char g_TextboxHighlightColorB;