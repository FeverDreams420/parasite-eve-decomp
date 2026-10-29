/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/bounds_check.h"
#include "pe1/draw_area.h"
#include "pe1/draw_state.h"
#include "pe1/menu_draw_list.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_scroll_cursor.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_cd.h"
#include "pe1/psyq_gpu.h"
#include "pe1/text.h"
#include "pe1/textbox.h"

/* Contiguous widget construction, pool initialization, list drawing and layout. */

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
unsigned char *g_DrawPacketCursor;
unsigned char *g_DrawPacketArenaBase;
unsigned int g_DrawPrimColor;
unsigned int g_DrawColorShaded;
unsigned int *g_DrawOrderingTableEntry;
int g_DrawTextPosX;
int g_MenuListRowHeight;
int g_DrawBufferIndex;
int g_TextRenderMode;
int g_DrawGlyphAdvance;
int g_DrawTextBoxWidth;
int g_DrawTextDimmed;
int g_MenuInputActive;
int *g_TextCursorStackPtr;
#define NULL ((void *)0)


MenuWidgetNode *g_MenuWidgetActiveListHead;
MenuWidgetNode *g_MenuWidgetFreeListHead;

#define NULL ((void *)0)


MenuWidgetNode *g_MenuWidgetActiveListHead;

/* The text cursor stack holds (x, y) word pairs. */
#define PUSH_CURSOR()                                                      \
    do {                                                                   \
        int *cursor = g_TextCursorStackPtr;                                \
        if (cursor < g_TextCursorStackTop) {                               \
            int x = g_TextCursorX;                                         \
            int y = g_TextCursorY;                                         \
            g_TextCursorStackPtr = cursor + 2;                             \
            cursor[0] = x;                                                 \
            cursor[1] = y;                                                 \
        } else {                                                           \
            Inv_BoundsCheckAssertStub(2);                                     \
        }                                                                  \
    } while (0)

#define PUSH_CURSOR2()                                     \
    do {                                                   \
        int *cursor = g_TextCursorStackPtr;                \
        if (cursor < g_TextCursorStackTop) {               \
            int *next = cursor + 2;                        \
            int x = g_TextCursorX;                         \
            int y = g_TextCursorY;                         \
            cursor[0] = x;                                 \
            cursor[1] = y;                                 \
            g_TextCursorStackPtr = next;                   \
        } else {                                           \
            Inv_BoundsCheckAssertStub(2);                     \
        }                                                  \
    } while (0)

#define POP_CURSOR()                                       \
    do {                                                   \
        int *cursor = g_TextCursorStackPtr;                \
                                                           \
        if (g_TextCursorStackBottom < cursor) {            \
            g_TextCursorX = cursor[-2];                    \
            g_TextCursorY = cursor[-1];                    \
            g_TextCursorStackPtr = cursor - 2;             \
        } else {                                           \
            Inv_BoundsCheckAssertStub(3);                     \
        }                                                  \
    } while (0)


MenuWidgetNode *g_MenuWidgetCurrentNode;

MenuWidgetNode *g_MenuWidgetSavedNode;

MenuWidgetNode *g_MenuWidgetActiveListHead;

void Draw_PushPrimToList(MenuWidgetNode *node);

int g_TextCursorX;
int g_TextCursorY;

void Draw_AllocColorTri(int, int, int);

typedef DrawTextCursorPair CursorPair;

int g_MenuWidgetColumnLayoutMode;
void bzero(void *ptr, int size);

void Draw_AllocColorTri(int width, int height, int arg2);

MenuWidgetNode *g_MenuWidgetFreeListHead;

int g_DrawColorSelect, g_DrawSpriteX, g_DrawSpriteY;
int *g_TextCursorStack;

#define NULL ((void *)0)



void Draw_FlushPrimList(int textId) {
    u8 *text;
    u8 *cursor;
    int width;
    int glyph;
    u8 ch;
    int first;

    text = Text_LookupTable4(textId);

    PUSH_CURSOR();

    first = *text;
    cursor = text;
    width = 0;
    if (first != 0xFF) {
        do {
            ch = *cursor++;
            glyph = ch;
            if (g_TextRenderMode != 0) {
                glyph += g_TextRenderMode << 8;
                g_TextRenderMode = 0;
            }

            if (ch >= 0xFA) {
                g_TextRenderMode = ch - 0xFA;
                glyph = -1;
            }

            if (glyph >= 0) {
                if (glyph >= 0x100) {
                    glyph -= 0x13;
                }
                width += ((Draw_LookupGlyphMetrics(glyph) >> 4) & 0xF) + g_DrawGlyphAdvance;
            }
        } while (*cursor != 0xFF);
        cursor = text;
    }

    {
        int adjusted = width + 4;
        int center = g_DrawTextBoxWidth - adjusted;
        int y = g_TextCursorY;
        int x = g_TextCursorX;
        center >>= 1;
        g_TextCursorY = y;
        g_TextCursorX = x + center;
    }

    if (cursor != 0) {
        PUSH_CURSOR2();

        {
            u8 code = *cursor;
            if ((code & 0xFF) != 0xFF) {
                for (;;) {
                    Draw_AllocTexturedQuad(code);
                    code = *++cursor;
                    if (code == 0xFF) break;
                }
            }
        }

        POP_CURSOR();
    }

    POP_CURSOR();
}