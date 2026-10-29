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

void MenuWidget_InitPoolUnk(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *end;
    MenuWidgetNode *next;

    node = g_MenuWidgetNodePool;
    end = node + 24;

    while (node < end) {
        next = node + 1;
        node->next = next;
        node = next;
    }

    g_MenuWidgetNodePoolSentinel.next = 0;
    g_MenuWidgetFreeListHead = &g_MenuWidgetNodePoolSentinel - 23;
    g_MenuWidgetCurrentNode = 0;
    g_MenuWidgetActiveListHead = 0;
}
#define NULL ((void *)0)

inline MenuWidgetNode *MenuWidget_AllocNode(MenuWidgetNode *arg0, MenuWidgetNode *arg1) {
    s32 var_a0;
    s32 var_v0;
    MenuWidgetNode *temp_s0;
    MenuWidgetNode *temp_v1;
    MenuWidgetNode *temp_next;
    MenuWidgetNode **var_a1;
    MenuWidgetNode *temp_s2;
    MenuWidgetNode *temp_s1;

    temp_s2 = arg0;
    temp_s1 = arg1;
    temp_s0 = g_MenuWidgetFreeListHead;
    if (temp_s0 == NULL) {
        Inv_BoundsCheckAssertStub(0xA);
    }
    var_a0 = 3;
    temp_next = temp_s0->next;
    temp_v1 = g_MenuWidgetActiveListHead;
    var_a1 = &temp_s0->children[1];
    g_MenuWidgetActiveListHead = temp_s0;
    temp_s0->parent = temp_s2;
    temp_s0->update = 0;
    temp_s0->draw = 0;
    g_MenuWidgetFreeListHead = temp_next;
    temp_s0->next = temp_v1;
    do {
        var_a1[2] = 0;
        var_a0 -= 1;
        var_a1--;
    } while (var_a0 >= 0);
    temp_s0->y = 0;
    temp_s0->x = 0;
    temp_s0->selected_base = 0;
    temp_s0->mode = 0;
    temp_s0->flags = 0;
    if (temp_s1 != NULL) {
        var_a0 = 0;
        while (var_a0 < 4 && temp_s1->children[var_a0] != 0) {
            var_a0 += 1;
        }
        var_v0 = var_a0 < 4;
        if (var_v0 != 0) {
            temp_s1->children[var_a0] = temp_s0;
        } else {
            Inv_BoundsCheckAssertStub(0xB, var_a1);
        }
    }
    return temp_s0;
}

MenuWidgetNode *g_MenuWidgetActiveListHead;
MenuWidgetNode *g_MenuWidgetFreeListHead;
void MenuWidget_DestroyNodeRecursive(MenuWidgetNode *node) {
    MenuWidgetNode *current = g_MenuWidgetActiveListHead;
    MenuWidgetNode *previous = 0;
    int i;
    if (current) {
        while (current) {
            if (current == node) {
                break;
            }
            previous = current;
            current = current->next;
        }
        if (current) {
            if (previous) {
                previous->next = current->next;
            } else {
                g_MenuWidgetActiveListHead = current->next;
            }
            current->next = g_MenuWidgetFreeListHead;
            g_MenuWidgetFreeListHead = current;
            if (current->mode == 2) {
                MenuWidget_SaveColumnLayout(current);
            }
            for (i = 0; i < 4; i++) {
                if (current->children[i]) {
                    MenuWidget_DestroyNodeRecursive(current->children[i]);
                }
            }
            if (MenuWidget_GetCurrentNode() == current) {
                MenuWidget_SetCurrentNode(current->parent);
            }
            for (current = g_MenuWidgetActiveListHead; current; current = current->next) {
                if (current->parent == node) current->parent = 0;
                if (current->mode == 2) {
                    if (current->linkedPrevious == node) current->linkedPrevious = 0;
                    if (current->linkedNext == node) current->linkedNext = 0;
                }
                for (i = 0; i < 4; i++) {
                    if (current->children[i] == node) current->children[i] = 0;
                }
            }
        }
    }
}

#define NULL ((void *)0)

/* Draws a widget and its children, each relative to its parent's origin,
 * saving the text cursor on the cursor stack around it. */
void Draw_PushPrimToList(MenuWidgetNode *node) {
    s32 temp_a2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_s1;
    u32 temp_a0;
    u32 temp_a0_3;
    int *temp_a1;
    u32 temp_a1_2;
    MenuWidgetNode *child;
    int *q;
    s32 t0;
    u32 t1;
    void (*f)();
    s32 offsetX, offsetY;
    s32 cursorX;
    u32 cursorY;

    temp_a1 = g_TextCursorStackPtr;
    if (temp_a1 < g_TextCursorStackTop) {
        t0 = g_TextCursorX;
        t1 = g_TextCursorY;
        g_TextCursorStackPtr = temp_a1 + 2;
        temp_a1[0] = t0;
        temp_a1[1] = t1;
    } else {
        Inv_BoundsCheckAssertStub(2, temp_a1);
    }
    offsetX = node->x;
    cursorX = g_TextCursorX;
    f = node->draw;
    temp_a2 = cursorX + offsetX;
    offsetY = node->y;
    cursorY = g_TextCursorY;
    g_TextCursorX = temp_a2;
    temp_a1_2 = cursorY + offsetY;
    g_TextCursorY = temp_a1_2;
    if (f != NULL) {
        q = g_TextCursorStackPtr;
        if (q < g_TextCursorStackTop) {
            q[0] = temp_a2;
            q[1] = temp_a1_2;
            g_TextCursorStackPtr = q + 2;
        } else {
            Inv_BoundsCheckAssertStub(2, temp_a1_2, temp_a2);
        }
        node->draw(node);
        if (g_TextCursorStackBottom < g_TextCursorStackPtr) {
            temp_v0 = g_TextCursorStackPtr[-2];
            temp_a0 = g_TextCursorStackPtr[-1];
            g_TextCursorStackPtr -= 2;
            g_TextCursorX = temp_v0;
            g_TextCursorY = temp_a0;
        } else {
            Inv_BoundsCheckAssertStub(3);
        }
    }
    var_s1 = 0;
    do {
        child = node->children[var_s1];
        if (child != NULL) {
            Draw_PushPrimToList(child);
        }
        var_s1 += 1;
    } while (var_s1 < 4);
    if (g_TextCursorStackBottom < g_TextCursorStackPtr) {
        temp_v0_2 = g_TextCursorStackPtr[-2];
        temp_a0_3 = g_TextCursorStackPtr[-1];
        g_TextCursorStackPtr -= 2;
        g_TextCursorX = temp_v0_2;
        g_TextCursorY = temp_a0_3;
        return;
    }
    Inv_BoundsCheckAssertStub(3);
}

MenuWidgetNode *g_MenuWidgetActiveListHead;

int MenuWidget_HasActiveNodes(void) {
    return g_MenuWidgetActiveListHead != 0;
}

MenuWidgetNode *MenuWidget_FindByFieldValue(int value) {
    MenuWidgetNode *node;
    MenuWidgetNode *result;
    int i;

    node = g_MenuWidgetActiveListHead;
    result = 0;
    while (node != 0) {
        i = 0;
        while (i < 4) {
            if ((int)node->children[i] == value) {
                break;
            }
            i++;
        }
        if (i < 4) {
            result = node;
            break;
        }
        node = node->next;
    }
    return result;
}

MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int index) {
    return node->children[index];
}

MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int selected_base) {
    MenuWidgetNode *node;

    node = g_MenuWidgetActiveListHead;
    while (node != 0) {
        if (node->mode == mode && node->selected_base == selected_base) {
            break;
        }
        node = node->next;
    }
    return node;
}

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
    }

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
