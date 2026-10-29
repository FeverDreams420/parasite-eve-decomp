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

/* Moves the cursor and saves the new position. */
static inline void PushCursorAt(int x, int y)
{
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;

    SetCursor(x, y);
    if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
        stack->x = x;
        stack->y = y;
        g_TextCursorStack = (int *)(stack + 1);
    } else {
        Inv_BoundsCheckAssertStub(2);
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

/* Draws a stat value followed by a 48-pixel level bar filled up to
 * 48 - width pixels, built from slices of glyph 0x48. */
void Draw_AllocTexturedRectAlt(int value, int width)
{
    DrawGlyphDescriptor *glyph = Draw_LookupGlyphDescriptor(0x48);
    RenderDrawModePacket *mode;
    RenderTexturedQuad *quad;
    int drawMode;
    DrawLevelBarLink link;
    u8 *old, *next;
    u32 *ot;

    PushCursor();
    PushCursorAt(g_TextCursorX, g_TextCursorY + 2);
    SetCursor(g_TextCursorX + 0x27, g_TextCursorY - 2);
    Draw_PrintNumberWidth2Unk(value);
    g_DrawPrimColor = 0x808080;
    g_DrawColorShaded = 0x404040;
    PopCursor();
    Draw_AllocSprite(0x49);

    if (width < 0x2F)
        DRAW_BAR_SEGMENT(g_TextCursorX + 1, quad->x0 + 1, 0);
    if (width < 0x2E)
        DRAW_BAR_SEGMENT(g_TextCursorX + 2, (s16)quad->x0 + 0x2E - width, 1);
    if (width < 0x30)
        DRAW_BAR_SEGMENT(g_TextCursorX + 0x30 - width, quad->x0 + 1, 2);
    if (width > 0)
        DRAW_BAR_SEGMENT(g_TextCursorX + 0x31 - width, quad->x0 + 1, 3);
    if (width >= 3)
        DRAW_BAR_SEGMENT(g_TextCursorX + 0x32 - width, (s16)quad->x0 - 1 + width, 4);
    if (width >= 2)
        DRAW_BAR_SEGMENT(g_TextCursorX + 0x31, quad->x0 + 1, 5);
    PopCursor();

    drawMode = glyph->mode;
    mode = 0;
    old = g_DrawPacketCursor;
    next = old + sizeof(RenderDrawModePacket);
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        mode = (RenderDrawModePacket *)old;
    } else {
        Inv_BoundsCheckAssertStub(1);
    }
    if (mode != 0)
        SetDrawTPage(mode, 0, 0, ((drawMode & 3) << 7) | 7);
    ot = g_DrawOrderingTableEntry;
    mode->tag = (mode->tag & 0xFF000000) | (*ot & 0xFFFFFF);
    link.mode = mode;
    *ot = (*ot & 0xFF000000) | (link.word & 0xFFFFFF);
}