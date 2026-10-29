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

/* Moves the cursor right by `dx`, rewriting y as the stored pair. */
static inline void AdvanceCursor(int dx)
{
    int x = g_TextCursorX;
    int y = g_TextCursorY;

    g_TextCursorX = x + dx;
    g_TextCursorY = y;
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

static inline int DecodeGlyph(u8 code) {
    int glyph = code;

    if (g_TextRenderMode) {
        glyph += g_TextRenderMode << 8;
        g_TextRenderMode = 0;
    }
    if (code >= 250) {
        g_TextRenderMode = code - 250;
        glyph = -1;
    }
    return glyph;
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

void Draw_AllocSprite(int index);



void Draw_AllocTexturedQuad(int code) {
    int glyph;
    int row;
    int column;
    int metrics;
    int width;
    int nibble;
    RenderTexturedQuad *prim;
    RenderPacketTag *ot;

    glyph = DecodeGlyph(code);
    if (glyph == 0x100) {
        Draw_AllocSprite(0x77);
        AdvanceCursor(0xC);
        return;
    }

    if (glyph < 0) {
        return;
    }

    if (glyph >= 0x101) {
        glyph -= 0x13;
    }

    glyph %= 0x1B9;
    row = glyph / 0x15;
    column = glyph - row * 0x15;

    prim = AllocBarQuad();
    metrics = Draw_LookupGlyphMetrics(glyph);
    nibble = metrics & 0xF;
    width = (metrics >> 4) & 0xF;
    prim->x0 = prim->x2 = g_TextCursorX + (g_DrawGlyphAdvance >> 1);
    prim->y0 = prim->y1 = g_TextCursorY;
    prim->u0 = prim->u2 = column * 0xC + nibble;
    prim->clut = 0x89C;
    prim->v0 = prim->v1 = row * 0xC;
    prim->u1 = prim->u3 = prim->u0 + width;
    prim->v2 = prim->v3 = prim->v0 + 0xC;
    prim->x1 = prim->x3 = prim->x0 + width;
    prim->y2 = prim->y3 = prim->y0 + 0xC;
    prim->tpage = GetTPage(0, 0, 0x140, 0);

    ot = (RenderPacketTag *)g_DrawOrderingTableEntry;
    prim->tag.link.address = ot->link.address;
    ot->link.address = (u32)prim;
    AdvanceCursor(width + g_DrawGlyphAdvance);
}

int Draw_MeasureTextWidth(u8 *text) {
    int width = 0;
    u8 code;

    while ((code = *text) != 255) {
        int glyph;
        text++;
        glyph = DecodeGlyph(code);
        if (glyph >= 0) {
            if (glyph >= 256) glyph -= 19;
            width += ((Draw_LookupGlyphMetrics(glyph) >> 4) & 15) + g_DrawGlyphAdvance;
        }
    }
    return width;
}