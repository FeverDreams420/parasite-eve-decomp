/* CC1_FLAGS: -G8 -fno-cse-skip-blocks */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/bounds_check.h"
#include "pe1/draw_area.h"
#include "pe1/draw_buffers.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/menu_inventory.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_prim.h"
#include "pe1/text.h"
#include "m2c_macros.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_DrawGlyphAdvance;
int g_TextRenderMode;
unsigned char *g_DrawPacketCursor;
unsigned char *g_DrawPacketArenaBase;
int g_DrawTextDimmed;
unsigned int g_DrawPrimColor;
unsigned int g_DrawColorShaded;
unsigned int *g_DrawOrderingTableEntry;
int g_TextCursorX;
int g_TextCursorY;
int *g_TextCursorStackPtr;
int g_DrawDigitFontTpageClut;
int g_DrawDigitFontBaseTexU;
int g_DrawDigitFontBaseTexV;
int g_DrawBufferIndex;
int g_DrawDigitFontBaseTexU;
int g_DrawDigitFontBaseTexV;
int g_DrawDigitFontTpageClut;
int g_DrawTextBoxWidth;
int *g_TextCursorStackPtr;
int g_TextRenderMode;

int Draw_SetBaseY(int arg0, unsigned int arg1);

int g_TextCursorX;
int g_TextCursorY;

/* The glyph renderers move the cursor at g_TextCursorX/g_TextCursorY and save it on
 * the same stack as Draw_StatePush. */
static inline void SetCursor(int x, int y)
{
    g_TextCursorX = x;
    g_TextCursorY = y;
}

/* Moves the cursor right by `dx`, rewriting y as the stored pair. */
static inline void AdvanceCursor(int dx)
{
    int x = g_TextCursorX;
    int y = g_TextCursorY;

    g_TextCursorX = x + dx;
    g_TextCursorY = y;
}

static inline void PushCursor(void)
{
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;
    if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
        int x = g_TextCursorX, y = g_TextCursorY;
        g_TextCursorStack = (int *)(stack + 1);
        stack->x = x;
        stack->y = y;
    } else {
        Inv_BoundsCheckAssertStub(2);
    }
}

static inline void PopCursor(void)
{
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;
    if ((DrawTextCursorPair *)g_TextCursorStackBottom < stack) {
        g_TextCursorStack = (int *)(stack - 1);
        SetCursor(stack[-1].x, stack[-1].y);
    } else {
        Inv_BoundsCheckAssertStub(3);
    }
}

unsigned int g_DrawPrimColor;
unsigned int g_DrawColorShaded;

#define NULL ((void *)0)


int g_DrawTextDimmed;

int g_DrawGlyphAdvance;

/* Allocates one bar quad from the packet arena and sets its colour and
 * command (0x2C: textured four-point polygon). */
static inline RenderTexturedQuad *AllocBarQuad(void)
{
    RenderTexturedQuad *prim = 0;
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + sizeof(RenderTexturedQuad);

    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        prim = (RenderTexturedQuad *)old;
    } else {
        Inv_BoundsCheckAssertStub(1);
    }
    if (prim != 0) {
        if (g_DrawTextDimmed != 0)
            prim->color.word = g_DrawColorShaded;
        else
            prim->color.word = g_DrawPrimColor;
        prim->tag.bytes.length = 9;
        prim->color.bytes.code = 0x2C;
    }
    return prim;
}

void Draw_SetFontVariant(int variant) {
    g_DrawDigitFontTpageClut = variant != 0 ? 0x3A1C : 0x395D;
    g_DrawDigitFontBaseTexU = variant != 0 ? 0xCC : 0x84;
    g_DrawDigitFontBaseTexV = 0xA4;
}

void Draw_EmitDigitSprite(int digit) {
    RenderTexturedQuad *quad = 0;
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + 40;
    DrawPrimLink link;

    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        quad = (RenderTexturedQuad *)old;
    } else {
        Inv_BoundsCheckAssertStub(1);
    }
    if (quad) {
        if (g_DrawColorSelect) quad->color.word = g_DrawAlternateColor;
        else quad->color.word = g_DrawPrimaryColor;
        quad->tag.bytes.length = 9;
        quad->color.bytes.code = 0x2C;
    }
    {
        RenderTexturedQuad *p;
        u16 x = (u16)g_DrawSpriteX, y = (u16)g_DrawSpriteY;
        p = quad;
        p->x0 = p->x2 = x;
        p->y0 = p->y1 = y;
        p->x1 = p->x3 = p->x0 + 5;
        p->y2 = p->y3 = p->y0 + 7;
        if (digit >= 0) p->u2 = g_DrawDigitFontBaseTexU + (digit % 10) * 5;
        else p->u2 = 88;
        {
            u8 value = p->u2;
            p->u2 = value;
            p->u0 = value;
        }
        if (digit >= 0) p->v1 = g_DrawDigitFontBaseTexV;
        else p->v1 = 164;
        {
            u8 value = p->v1;
            p->v0 = value;
            p->v1 = value;
            p->u1 = p->u3 = p->u0 + 5;
            p->v2 = p->v3 = p->v0 + 7;
        }
        p->clut = g_DrawDigitFontTpageClut;
        p->tpage = 7;
        {
            RenderPacketTag *ot = (RenderPacketTag *)g_DrawOrderingTableEntry;
            p->tag.link.address = ot->link.address;
            link.packet = p;
            ot->link.address = link.word;
        }
    }
}

void Draw_PrintNumberWidth2Unk(int value) {
    int width = 2;
    int place = 1;
    int i;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 1;
        AdvanceCursor(5);
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        AdvanceCursor(5);
        place /= 10;
    }
}

void Draw_PrintNumberWidth3Unk(int value) {
    int width = 3;
    int place = 1;
    int i;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 2;
        AdvanceCursor(5);
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        AdvanceCursor(5);
        place /= 10;
    }
}

void Draw_PrintSignedNumberWidth3(int value) {
    int width = 3;
    int place = 1;
    int i;
    int sprite;

    if (value < 0) {
        value = -value;
        sprite = 0x52;
        Draw_AllocSprite(sprite);
        width = 2;
        AdvanceCursor(5);
    } else if (value > 0) {
        sprite = 0x89;
        Draw_AllocSprite(sprite);
        width = 2;
        AdvanceCursor(5);
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        AdvanceCursor(5);
        place /= 10;
    }
}

void Draw_PrintNumberWidth4Unk(int value) {
    int width = 4;
    int place = 1;
    int i;

    if (value < 0) {
        value = -value;
        Draw_AllocSprite(0x52);
        width = 3;
        AdvanceCursor(5);
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        AdvanceCursor(5);
        place /= 10;
    }
}

void Draw_PrintSignedNumberWidth4(int value) {
    int width = 4;
    int place = 1;
    int i;
    int sprite;

    if (value < 0) {
        value = -value;
        sprite = 0x52;
        Draw_AllocSprite(sprite);
        width = 3;
        AdvanceCursor(5);
    } else if (value > 0) {
        sprite = 0x89;
        Draw_AllocSprite(sprite);
        width = 3;
        AdvanceCursor(5);
    }
    for (i = 1; i < width; i++) {
        place *= 10;
    }
    for (i = 0; i < width; i++) {
        int q = value / place;
        int digit = (i < width - 1 && q == 0) ? -1 : q;
        Draw_EmitDigitSprite(digit);
        AdvanceCursor(5);
        place /= 10;
    }
}

/* hh:mm:ss from a count of tenths of a second, clamped to 99:59:59.9 when it
 * exceeds 100 hours; `flag` selects the alternate digit font and separator. */
void Draw_PrintTimeValue(int value, int flag) {
    if (value > 360000) {
        value = 359999;
    }
    g_DrawDigitFontTpageClut = flag != 0 ? 0x3A1C : 0x395D;
    g_DrawDigitFontBaseTexU = flag != 0 ? 0xCC : 0x84;
    g_DrawDigitFontBaseTexV = 0xA4;

    Draw_EmitDigitSprite(value / 36000);
    AdvanceCursor(5);
    Draw_EmitDigitSprite(value / 3600);
    AdvanceCursor(5);
    if (flag) {
        Draw_AllocSprite(0x9B);
        AdvanceCursor(3);
    } else {
        Draw_AllocSprite(0x4F);
        AdvanceCursor(5);
    }
    Draw_EmitDigitSprite((value / 600) % 6);
    AdvanceCursor(5);
    Draw_EmitDigitSprite(value / 60);
    AdvanceCursor(5);
    if (flag) {
        Draw_AllocSprite(0x9B);
        AdvanceCursor(3);
    } else {
        Draw_AllocSprite(0x4F);
        AdvanceCursor(5);
    }
    Draw_EmitDigitSprite((value / 10) % 6);
    AdvanceCursor(5);
    Draw_EmitDigitSprite(value);

    g_DrawDigitFontTpageClut = 0x395D;
    g_DrawDigitFontBaseTexU = 0x84;
    g_DrawDigitFontBaseTexV = 0xA4;
}

static inline void PrintSign(u8 *text)
{
    if (text) {
        PushCursor();
        while (*text != 255) {
            Draw_AllocTexturedQuad(*text++);
        }
        PopCursor();
    }
}

static inline int Digit(int leading, int value)
{
    if (!leading || value != 0) {
        return (u8)(value % 10);
    }
    return 15;
}

/* Print a signed decimal field; glyph 15 is padding, table entry 0x70 the
 * minus sign. The sign occupies one place and restores its own text cursor.
 * Retail callers use bounded widths; powers of ten and negation must fit int.
 */
void Draw_AllocTexturedRect(int value, int width)
{
    int divisor = 1;
    int i;

    for (i = 1; i < width; i++) {
        divisor *= 10;
    }
    if (value < 0) {
        value = -value;
        divisor /= 10;
        width--;
        while (value < divisor) {
            Draw_AllocTexturedQuad(15);
            divisor /= 10;
            width--;
        }
        PrintSign(Text_LookupTable4(0x70));
        SetCursor(g_TextCursorX + 9u, g_TextCursorY);
    }
    for (i = 0; i < width; i++) {
        Draw_AllocTexturedQuad(Digit(i < width - 1, value / divisor));
        divisor /= 10;
    }
}

/* One textured quad of the bar: glyph texels starting `u` columns into the
 * glyph, from `left` to the right edge computed from the stored left edge. */
#define DRAW_BAR_SEGMENT(left, right_from_x0, u_offset) \
    { \
        u32 *ot; \
        quad = AllocBarQuad(); \
        quad->x0 = quad->x2 = left; \
        quad->y0 = quad->y1 = g_TextCursorY + 1; \
        quad->x1 = quad->x3 = right_from_x0; \
        quad->y2 = quad->y3 = quad->y0 + glyph->height; \
        quad->u0 = quad->u2 = glyph->u + (u_offset); \
        quad->v0 = quad->v1 = glyph->v; \
        quad->u1 = quad->u3 = quad->u0; \
        quad->v2 = quad->v3 = quad->v0 + glyph->height; \
        quad->clut = glyph->clut; \
        quad->tpage = 7; \
        quad->tag.link.address = *g_DrawOrderingTableEntry; \
        ot = g_DrawOrderingTableEntry; \
        link.quad = quad; \
        *ot = (*ot & 0xFF000000) | (link.word & 0xFFFFFF); \
    }