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

void Draw_SetBaseOffsetPosition(int x, int y) {
    Draw_SetBaseY(g_DrawBaseX[0] + x, g_DrawBaseY + y);
}

int Draw_GetBaseY(void) {
    return g_DrawBaseY;
}

int g_TextCursorX;
int g_TextCursorY;

void Draw_SetCursor(int arg0, int arg1) {
    g_TextCursorX = arg0;
    g_TextCursorY = arg1;
}

void Draw_OffsetCursor(int x, int y) {
    g_TextCursorX += x;
    g_TextCursorY += y;
}

void Draw_StatePush(void) {
    DrawTextCursorPair *cursor;
    int x;
    int y;

    cursor = (DrawTextCursorPair *)g_TextCursorStackPtr;
    if (cursor < (DrawTextCursorPair *)g_TextCursorStackTop) {
        x = g_TextCursorX;
        y = g_TextCursorY;
        g_TextCursorStackPtr = (int *)(cursor + 1);
        cursor->x = x;
        cursor->y = y;
    } else {
        Inv_BoundsCheckAssertStub(2);
    }
}

void Draw_StatePop(void) {
    DrawTextCursorPair *cursor;
    int x;
    int y;

    cursor = (DrawTextCursorPair *)g_TextCursorStackPtr;
    if ((DrawTextCursorPair *)g_TextCursorStackBottom < cursor) {
        x = cursor[-1].x;
        y = cursor[-1].y;
        g_TextCursorStackPtr = (int *)(cursor - 1);
        g_TextCursorX = x;
        g_TextCursorY = y;
    } else {
        Inv_BoundsCheckAssertStub(3);
    }
}

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

/* Saves a known cursor position: the entry is filled before the stack moves. */
static inline void SaveCursor(int x, int y)
{
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;
    if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
        stack->x = x;
        stack->y = y;
        g_TextCursorStack = (int *)(stack + 1);
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

void Draw_SetColor(int value) {
    g_DrawPrimColor = value;
    g_DrawColorShaded = (value >> 1) & 0x7F7F7F;
}

void Draw_SetStatCompareColor(int arg0, int arg1) {
    unsigned int color;

    if (arg0 >= arg1) {
        color = 0x808080;
        if (arg1 < arg0) {
            color = 0x404080;
        }
    } else {
        color = 0x408080;
    }

    g_DrawPrimColor = color;
    g_DrawColorShaded = color >> 1;
}

#define NULL ((void *)0)

void Draw_AllocPrimRectFull(s32 arg0, s32 arg1, s32 arg2, s32 arg3) {
    DrawAreaRect rect;
    u8 *oldPacket;
    u8 *nextPacket;
    GpuCmdPacket *packet;
    DrawAreaRect *rectPtr;
    DrawPrimLink link;

    if (g_DrawBufferIndex != 0) {
        arg1 += 0xE0;
    }
    packet = NULL;
    rect.x = arg0;
    oldPacket = g_DrawPacketCursor;
    rectPtr = &rect;
    rect.y = arg1;
    rect.w = arg2;
    nextPacket = oldPacket + sizeof(GpuCmdPacket);
    rect.h = arg3;
    if (nextPacket < (g_DrawPacketArenaBase + 0x4000)) {
        g_DrawPacketCursor = nextPacket;
        packet = (GpuCmdPacket *) oldPacket;
    } else {
        Inv_BoundsCheckAssertStub(1);
    }
    if (packet != NULL) {
        SetDrawArea(packet, rectPtr);
    }
    link.packet = packet;
    packet->u0.tag = (packet->u0.tag & 0xFF000000) | (*g_DrawOrderingTableEntry & 0xFFFFFF);
    *g_DrawOrderingTableEntry = (*g_DrawOrderingTableEntry & 0xFF000000) | (link.word & 0xFFFFFF);
}

void Draw_AllocPrimRect(void) {
    DrawAreaRect rect;
    u8 *oldPacket;
    u8 *nextPacket;
    GpuCmdPacket *packet;
    DrawAreaRect *rectPtr;
    DrawPrimLink link;

    rect.w = 0x140;
    rect.x = 0;
    rect.y = 0;
    rect.h = 0xE0;
    if (g_DrawBufferIndex != 0) {
        rect.y = 0xE0;
    }
    packet = NULL;
    oldPacket = g_DrawPacketCursor;
    nextPacket = oldPacket + sizeof(GpuCmdPacket);
    rectPtr = &rect;
    if (nextPacket < (g_DrawPacketArenaBase + 0x4000)) {
        g_DrawPacketCursor = nextPacket;
        packet = (GpuCmdPacket *) oldPacket;
    } else {
        Inv_BoundsCheckAssertStub(1);
    }
    if (packet != NULL) {
        SetDrawArea(packet, rectPtr);
    }
    link.packet = packet;
    packet->u0.tag = (packet->u0.tag & 0xFF000000) | (*g_DrawOrderingTableEntry & 0xFFFFFF);
    *g_DrawOrderingTableEntry = (*g_DrawOrderingTableEntry & 0xFF000000) | (link.word & 0xFFFFFF);
}

int g_DrawTextDimmed;

void Draw_SetTextDimmed(int value) {
    g_DrawTextDimmed = value;
}

void Draw_AllocSprite(int index) {
    DrawGlyphDescriptor *glyph;
    RenderSpritePacket *sprite;
    u32 *page;
    int mode;
    DrawPrimLink link;

    glyph = Draw_LookupGlyphDescriptor(index);
    sprite = 0;
    {
        u8 *old = g_DrawPacketCursor;
        u8 *next = old + sizeof(RenderSpritePacket);

        if (next < g_DrawPacketArenaBase + 0x4000) {
            g_DrawPacketCursor = next;
            sprite = (RenderSpritePacket *)old;
        } else Inv_BoundsCheckAssertStub(1);
    }
    if (sprite) {
        if (g_DrawColorSelect) sprite->color.word = g_DrawAlternateColor;
        else sprite->color.word = g_DrawPrimaryColor;
        sprite->tag.bytes.length = 4;
        sprite->color.bytes.code = 0x64;
    }
    sprite->x = g_DrawSpriteX;
    sprite->y = g_DrawSpriteY;
    sprite->u = glyph->u;
    sprite->v = glyph->v;
    sprite->clut = glyph->clut;
    sprite->width = glyph->width;
    sprite->height = glyph->height;
    {
        RenderPacketTag *ot = (RenderPacketTag *)g_DrawOrderingTableEntry;

        sprite->tag.link.address = ot->link.address;
        link.packet = sprite;
        ot->link.address = link.word;
    }
    mode = glyph->mode;
    page = 0;
    {
        u8 *old = g_DrawPacketCursor;
        u8 *next = old + 8;

        if (next < g_DrawPacketArenaBase + 0x4000) {
            g_DrawPacketCursor = next;
            page = (u32 *)old;
        } else Inv_BoundsCheckAssertStub(1);
    }
    if (page) {
        SetDrawTPage(page, 0, 0, ((mode & 3) << 7) | 7);
    }
    {
        u32 mask24, maskTop;
        u32 *ot;
        mask24 = 0xffffff;
        maskTop = 0xff000000;
        ot = g_DrawOrderingTableEntry;
        *page = (*page & maskTop) | (*ot & mask24);
        link.packet = page;
        *ot = (*ot & maskTop) | (link.word & mask24);
    }
}

void Draw_EmitGlyph(s32 arg0, s32 arg1) {
    DrawGlyphDescriptor *glyph;
    u8 *oldPacket;
    u8 *nextPacket;
    RenderTexturedQuad *packet;
    u16 base_x;
    u16 base_y;
    s32 temp;

    packet = 0;
    glyph = Draw_LookupGlyphDescriptor(arg0);
    /* Preserve the descriptor's register across packet setup. */
    asm volatile("" : "=r"(glyph) : "0"(glyph));
    oldPacket = g_DrawPacketCursor;
    nextPacket = oldPacket + sizeof(RenderTexturedQuad);
    if (nextPacket < (g_DrawPacketArenaBase + 0x4000)) {
        g_DrawPacketCursor = nextPacket;
        packet = (RenderTexturedQuad *)oldPacket;
    } else {
        Inv_BoundsCheckAssertStub(1);
    }

    if (packet != 0) {
        if (g_DrawTextDimmed != 0) {
            packet->color.word = g_DrawColorShaded;
        } else {
            packet->color.word = g_DrawPrimColor;
        }
        packet->tag.bytes.length = 9;
        packet->color.bytes.code = 0x2C;
    }

    {
        register RenderTexturedQuad *ptr asm("$4");

        base_x = (u16)g_TextCursorX;
        base_y = (u16)g_TextCursorY;
        ptr = packet;
        ptr->x2 = base_x;
        ptr->x0 = base_x;
        ptr->y1 = base_y;
        ptr->y0 = base_y;

        {
            s32 sum;
            s32 glyphDim;

            glyphDim = glyph->width;
            sum = ptr->x0;
            temp = sum + glyphDim;
        }
        ptr->x3 = temp;
        ptr->x1 = temp;

        {
            s32 sum;
            s32 glyphDim;

            glyphDim = glyph->height;
            sum = ptr->y0;
            temp = sum + glyphDim;
        }
        ptr->y3 = temp;
        ptr->y2 = temp;

        if (arg1 == 2) {
            temp = glyph->u + glyph->width - 1;
            ptr->u2 = temp;
            ptr->u0 = temp;
            temp = glyph->v + glyph->height - 1;
            ptr->v1 = temp;
            ptr->v0 = temp;
            temp = glyph->u - 1;
            ptr->u3 = temp;
            ptr->u1 = temp;
            temp = glyph->v - 1;
            ptr->v3 = temp;
            ptr->v2 = temp;
        }
    }

    {
        RenderPacketTag *ot;
        DrawPrimLink link;

        packet->clut = glyph->clut;
        packet->tpage = 7;
        ot = (RenderPacketTag *)g_DrawOrderingTableEntry;
        packet->tag.link.address = ot->link.address;
        link.packet = packet;
        ot->link.address = link.word;
    }
}

int g_DrawGlyphAdvance;

void Draw_SetGlyphAdvance(int arg0) {
    g_DrawGlyphAdvance = arg0;
}

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

static inline int DecodeGlyph(int input) {
    register int code asm("$4") = input;
    int glyph;
    glyph = (u8)code;
    if (g_TextRenderMode) {
        int page = g_TextRenderMode << 8;
        glyph += page;
        g_TextRenderMode = 0;
    }
    if ((u8)code >= 250) {
        g_TextRenderMode = (u8)code - 250;
        glyph = -1;
    }
    return glyph;
}

/* Draws text from the current cursor position without saving it first. */
static inline void PrintTextHere(u8 *text) {
    if (text) {
        SaveCursor(g_TextCursorX, g_TextCursorY);
        while (*text != 255) {
            Draw_AllocTexturedQuad(*text++);
        }
        PopCursor();
    }
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