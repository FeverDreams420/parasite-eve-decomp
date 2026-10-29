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

void Draw_AllocTexturedQuad(int code);



void Draw_PrintTextWrapped(u8 *text, int width) {
    while (*text != 0xFF) {
        int used;
        DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStackPtr;
        if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
            int x = g_DrawSpriteX, y = g_DrawSpriteY;
            g_TextCursorStackPtr = (int *)(stack + 1);
            stack->x = x;
            stack->y = y;
        } else Inv_BoundsCheckAssertStub(2);
        used = 0;
        while (used < width) {
            int code, glyph;
            if (*text == 0xFF) break;
            Draw_AllocTexturedQuad(*text);
            code = *text++;
            glyph = (u8)code;
            if (g_TextRenderMode) {
                glyph += g_TextRenderMode << 8;
                g_TextRenderMode = 0;
            }
            if ((u8)code >= 250) {
                g_TextRenderMode = (u8)code - 250;
                glyph = -1;
            }
            if (glyph >= 0) {
                if (glyph >= 256) glyph -= 19;
                used += ((Draw_LookupGlyphMetrics(glyph) >> 4) & 15) + g_DrawGlyphAdvance;
            }
        }
        {
            DrawTextCursorPair *restore = (DrawTextCursorPair *)g_TextCursorStackPtr;
            if ((DrawTextCursorPair *)g_TextCursorStackBottom < restore) {
                int x = restore[-1].x, y = restore[-1].y;
                g_TextCursorStackPtr = (int *)(restore - 1);
                g_DrawSpriteX = x;
                g_DrawSpriteY = y;
            } else Inv_BoundsCheckAssertStub(3);
        }
        {
            int y = g_DrawSpriteY;
            /* Preserve the retail read/write of X during a vertical move. */
            int x = g_DrawSpriteX;
            g_DrawSpriteX = x;
            g_DrawSpriteY = y + 14;
        }
    }
}