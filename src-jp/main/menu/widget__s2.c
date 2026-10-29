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
    }

MenuWidgetNode *g_MenuWidgetCurrentNode;

MenuWidgetNode *g_MenuWidgetSavedNode;

MenuWidgetNode *g_MenuWidgetActiveListHead;

void Draw_PushPrimToList(MenuWidgetNode *node);

int g_TextCursorX;
int g_TextCursorY;

void Draw_AllocColorTri(int, int, int);

typedef DrawTextCursorPair CursorPair;

static inline MenuWidgetNode *FindOwner(MenuWidgetNode *node)
{
    MenuWidgetNode *found = 0;
    MenuWidgetNode *scan = g_MenuWidgetActiveListHead;
    while (scan) {
        int i;
        for (i = 0; i < 4; i++) {
            if (scan->children[i] == node) break;
        }
        if (i < 4) {
            found = scan;
            break;
        }
        scan = scan->next;
    }
    return found;
}

static inline void SetCursor(u32 x, u32 y)
{
    g_TextCursorX = x;
    g_TextCursorY = y;
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

void Draw_SetPrimCallback(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *temp_a1_2;
    s32 temp_a2;
    s32 temp_v1;
    s32 t58b;
    s32 t48;
    s32 t5C;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 temp_v1_4;
    s32 var_a1;
    s32 var_a2;
    MenuWidgetNode *temp_a1;

    temp_v1 = arg0->grid_width;
    arg0->y_limit = (((arg1 + temp_v1) - 1) / temp_v1);
    var_a2 = 0;
    if (arg0->grid_width == 2) {
        var_a2 = arg1 & 1;
    }
    t48 = arg0->cursor_y;
    arg0->has_scroll = var_a2;
    temp_v1_2 = arg0->y_limit;
    if (t48 >= temp_v1_2) {
        arg0->cursor_y = temp_v1_2 - 1;
    }
    if ((arg0->has_scroll != 0) && (arg0->cursor_x == 1) &&
        (arg0->cursor_y == (arg0->y_limit - 1))) {
        arg0->cursor_x = 0;
    }
    var_a1 = arg0->visible_rows_mirror;
    temp_v1_3 = arg0->y_limit;
    temp_a2 = arg0->visible_rows;
    if (temp_v1_3 < var_a1) {
        var_a1 = temp_v1_3;
        temp_v1_3 = *(volatile s32 *)&arg0->y_limit;
    }
    temp_v1_4 = temp_v1_3 - var_a1;
    arg0->visible_rows = var_a1;
    if (temp_v1_4 < arg0->scroll_y) {
        arg0->scroll_y = temp_v1_4;
    }
    temp_a1 = arg0->parent;
    if ((temp_a1 != 0) && (temp_a1->mode == 1)) {
        temp_a1->visible_rows += arg0->disabled * (arg0->visible_rows - temp_a2);
    }
    temp_a1_2 = arg0->popup_node;
    if (temp_a1_2 != 0) {
        t58b = arg0->y_limit;
        if (arg0->visible_rows_mirror >= t58b) {
            MenuWidget_DestroyPopupNode(temp_a1_2);
        }
    } else {
        t58b = arg0->y_limit;
        if (arg0->visible_rows_mirror < t58b) {
            Draw_SwapPrimBuffers(arg0);
        }
    }
}

int g_MenuWidgetColumnLayoutMode;
void bzero(void *ptr, int size);

/* Saved cursor/scroll positions of menu widgets (one 4-byte slot per
 * aux_index): reset, mode, save and restore. Menu_InitStateBuffer (historical
 * name) is the unconditional reset; the eight slots set to -1 start with no
 * saved column. */

void Menu_InitStateBuffer(void) {
    bzero(g_MenuWidgetColumnLayoutTable, 0x120);
    g_MenuWidgetColumnLayoutTable[6].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[16].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[20].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[22].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[24].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[25].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[49].cursorX = -1;
    g_MenuWidgetColumnLayoutTable[53].cursorX = -1;
}

void MenuWidget_SetColumnLayoutMode(int arg0) {
    g_MenuWidgetColumnLayoutMode = arg0;
    if (arg0 == 0) {
        bzero(g_MenuWidgetColumnLayoutTable, 0x120);
        g_MenuWidgetColumnLayoutTable[6].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[16].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[20].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[22].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[24].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[25].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[49].cursorX = -1;
        g_MenuWidgetColumnLayoutTable[53].cursorX = -1;
    }
}

int MenuWidget_GetColumnLayoutMode(void) {
    return g_MenuWidgetColumnLayoutMode;
}

void MenuWidget_SaveColumnLayout(MenuWidgetNode *node) {
    int index;
    MenuWidgetSavedLayout *slot;

    if (g_MenuWidgetColumnLayoutMode == 0 && ((node->layout_flags & 0x20) == 0)) {
        return;
    }

    index = node->aux_index;
    if ((unsigned int)index < 0x48) {
        slot = &g_MenuWidgetColumnLayoutTable[index];
        slot->cursorX = node->cursor_x;
        slot->cursorY = node->cursor_y;
        slot->scrollY = node->scroll_y;
    }
}

void MenuWidget_ApplyColumnLayout(MenuWidgetNode *node) {
    MenuWidgetSavedLayout *entry;
    int flag;

    if (node == 0) {
        return;
    }
    if (node->aux_index < 0) {
        return;
    }

    entry = &g_MenuWidgetColumnLayoutTable[node->aux_index];
    flag = g_MenuWidgetColumnLayoutMode;
    node->cursor_x = entry->cursorX;
    if ((flag != 0) || ((node->layout_flags & 0x20) != 0)) {
        node->cursor_y = entry->cursorY;
        if (node->y_limit <= node->cursor_y) {
            node->cursor_y = node->y_limit - 1;
        }
        if ((node->has_scroll != 0) &&
            (node->cursor_x == 1) &&
            (node->cursor_y == (node->y_limit - 1))) {
            node->cursor_x = 0;
        }
        node->scroll_y = entry->scrollY;
    }
}

void MenuWidget_SetColumnLayout(MenuWidgetNode *node, int arg1) {
    MenuWidgetSavedLayout *entry;
    int flag;

    node->aux_index = arg1;
    if (node == 0) {
        return;
    }
    if (arg1 < 0) {
        return;
    }

    entry = &g_MenuWidgetColumnLayoutTable[arg1];
    flag = g_MenuWidgetColumnLayoutMode;
    node->cursor_x = entry->cursorX;
    if ((flag != 0) || ((node->layout_flags & 0x20) != 0)) {
        node->cursor_y = entry->cursorY;
        if (node->y_limit <= node->cursor_y) {
            node->cursor_y = node->y_limit - 1;
        }
        if ((node->has_scroll != 0) &&
            (node->cursor_x == 1) &&
            (node->cursor_y == (node->y_limit - 1))) {
            node->cursor_x = 0;
        }
        node->scroll_y = entry->scrollY;
    }
}

void MenuWidget_ClearColumnLayout(MenuWidgetNode *node) {

    node->aux_index = -1;
    node->cursor_x = -1;
}

void MenuWidget_DrawCenteredText(u8 *text) {
    Draw_PrintCenteredTextInWidth(text, g_DrawTextPosX);
}

void MenuWidget_DrawCenteredTableText(int text_id) {
    Draw_PrintCenteredTextInWidth(Text_LookupTable4(text_id), g_DrawTextPosX);
}

void Draw_AllocColorTri(int width, int height, int arg2);

void Draw_DropShadow(void) {
    g_DrawPrimColor = 0xFF4000;
    g_DrawColorShaded = 0x7F2000;
    g_DrawSpriteX -= 2;
    g_DrawSpriteY -= 2;
    Draw_AllocColorTri(g_DrawTextPosX, g_MenuListRowHeight, 0);
    g_DrawPrimColor = 0x808080;
    g_DrawColorShaded = 0x404040;
    g_DrawSpriteX += 2;
    g_DrawSpriteY += 2;
}

MenuWidgetNode *g_MenuWidgetFreeListHead;

static inline MenuWidgetNode *Allocate(MenuWidgetNode *parent,
                                       MenuWidgetNode *owner)
{
    MenuWidgetNode *node = g_MenuWidgetFreeListHead;
    MenuWidgetNode *next;
    MenuWidgetNode *old;
    int i;

    if (!node)
        Inv_BoundsCheckAssertStub(10);
    next = node->next;
    old = g_MenuWidgetActiveListHead;
    g_MenuWidgetActiveListHead = node;
    node->parent = parent;
    node->update = 0;
    node->draw = 0;
    g_MenuWidgetFreeListHead = next;
    node->next = old;
    for (i = 3; i >= 0; i--)
        node->children[i] = 0;
    node->y = 0;
    node->x = 0;
    node->selected_base = 0;
    node->mode = 0;
    node->flags = 0;
    if (owner) {
        for (i = 0; i < 4; i++) {
            if (!owner->children[i])
                break;
        }
        if (i < 4)
            owner->children[i] = node;
        else
            Inv_BoundsCheckAssertStub(11);
    }
    return node;
}

/* Historical symbol name: constructs a mode-3 list-navigation widget. */
void Draw_SwapPrimBuffers(MenuWidgetNode *list)
{
    MenuWidgetNode *owner = FindOwner(list);
    MenuWidgetNode *node = Allocate(list->parent, owner);
    MenuWidgetListNavigation *navigation = (MenuWidgetListNavigation *)node;

    if (!node)
        Inv_BoundsCheckAssertStub(16);
    node->mode = 3;
    node->draw = Draw_FlushFrontBuffer;
    node->update = Menu_StepListNavigate;
    navigation->list = list;
    navigation->flags = list->layout_flags;
    list->popup_node = node;
}

void MenuWidget_DestroyPopupNode(MenuWidgetNode *node) {
    MenuWidgetNode *owner = ((MenuWidgetListNavigation *)node)->list;

    owner->popup_node = 0;
    MenuWidget_DestroyNodeRecursive(node);
}

int g_DrawColorSelect, g_DrawSpriteX, g_DrawSpriteY;
int *g_TextCursorStack;

static inline void Offset(int x, int y) {
    g_DrawSpriteX += x;
    g_DrawSpriteY += y;
}

void Draw_FlushFrontBuffer(MenuWidgetListNavigation *node) {
    MenuWidgetNode *list = node->list;
    if (list) {
        MenuWidgetNode *owner = FindOwner((MenuWidgetNode *)node);
        DrawTextCursorPair *stack;
        g_DrawColorSelect = owner->mode == 1 && owner->draw_state != 0;
        stack = (DrawTextCursorPair *)g_TextCursorStack;
        if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
            int x = g_DrawSpriteX;
            int y = g_DrawSpriteY;
            g_TextCursorStack = (int *)(stack + 1);
            stack->x = x;
            stack->y = y;
        } else {
            Inv_BoundsCheckAssertStub(2);
        }
        Offset(list->draw_state * list->grid_width + 2,
               node->visibleRows + 2);
        if (g_MenuWidgetCurrentNode == (MenuWidgetNode *)node && (VSync(-1) & 8))
            Draw_AllocColorTri(8, node->drawState, 0);
        Draw_AllocColorGradient(8, node->drawState, 0, 1);
        if (list->scroll_y) {
            Offset(0, -6);
            Draw_AllocSprite(0x4a);
            Offset(0, 6);
        }
        if (list->scroll_y < list->y_limit - list->visible_rows) {
            Offset(0, node->drawState + 2);
            Draw_AllocSprite(0x4b);
        }
        PopCursor();
    }
}

static inline int Move(MenuWidgetNode *node, int delta) {
    int changed = 0;

    if (!node->scroll_adjust) {
        int old = node->scroll_y;
        node->scroll_y += delta;
        if (node->scroll_y < 0) {
            node->scroll_y = 0;
        } else if (node->scroll_y > node->y_limit - node->visible_rows) {
            node->scroll_y = node->y_limit - node->visible_rows;
        }
        changed = old != node->scroll_y;
        if (changed) {
            int speed = node->disabled;
            node->scroll_adjust = (delta > 0 ? speed : -speed) / 2;
        }
    }
    return changed;
}

int Menu_StepListNavigate(MenuWidgetListNavigation *node, unsigned int flags) {
    MenuWidgetNode *list = node->list;
    int handled = 0;

    if (flags & 0x1004) {
        if (Move(list, -list->visible_rows)) Menu_PlayMoveSound();
        handled = 1;
    } else if (flags & 0x4008) {
        if (Move(list, list->visible_rows)) Menu_PlayMoveSound();
        handled = 1;
    }
    return handled || !(node->flags & 0x40);
}

#define NULL ((void *)0)
void MenuWidget_EaseNodePosition(MenuWidgetNode *arg0) {
    s32 temp_a2;
    s32 temp_v1;
    s32 temp_v1_2;
    MenuWidgetNode *temp_a1;
    MenuWidgetListNavigation *navigation = (MenuWidgetListNavigation *)arg0;

    temp_a1 = navigation->list;
    if (arg0 != NULL) {
        temp_v1 = temp_a1->visible_rows;
        temp_a2 = temp_a1->y_limit;
        if (temp_v1 < temp_a2) {
            navigation->drawState = ((temp_a1->disabled * temp_v1 * temp_v1) / temp_a2);
            temp_v1_2 = temp_a1->disabled;
            navigation->visibleRows = ((temp_v1_2 * temp_a1->visible_rows * ((temp_v1_2 * temp_a1->scroll_y) - temp_a1->scroll_adjust)) / (temp_a1->y_limit * temp_v1_2));
        }
    }
}