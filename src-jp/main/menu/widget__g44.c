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

static inline int MenuWidget_CursorCell(MenuWidgetNode *node)
{
    int cell;
    int x;
    int y;

    cell = -1;
    if (node != 0) {
        x = node->cursor_x;
        if (x >= 0) {
            y = node->cursor_y;
            if (y >= 0)
                cell = node->grid_width * y + x;
        }
    }
    return cell;
}

static inline int MenuWidget_TargetCell(MenuWidgetNode *node)
{
    int cell;
    int x;
    int y;

    cell = -1;
    if (node != 0) {
        x = node->appearance.cursorTargetX;
        if (x >= 0 && (y = node->target_y) >= 0)
            cell = node->grid_width * y + x;
        else
            cell = -1;
    }
    return cell;
}

/* Rows past the last selectable one: column 1 of a list with a trailing
 * scroll row has one cell fewer. */
static inline int MenuWidget_EndMargin(MenuWidgetNode *node)
{
    return (node->cursor_x == 1 && node->has_scroll) + 1;
}

static inline void MenuWidget_ScrollToCursor(MenuWidgetNode *node)
{
    int y;
    int top;
    int height;

    y = node->cursor_y;
    top = node->scroll_y;
    if (y < top) {
        node->scroll_y = y;
    } else {
        height = node->visible_rows;
        if (y >= top + height)
            node->scroll_y = y - height + 1;
    }
}

static inline int MenuWidget_HeldShoulder(void)
{
    if (g_MenuInputActive)
        return (Draw_RemapStatusFlags() & 0x5000) != 0;
    return 0;
}

/* Swaps the cursor cell of `node` with the marked cell of `other`. */
static inline int MenuWidget_SwapMarked(MenuWidgetNode *node, MenuWidgetNode *other)
{
    int done;
    int from;
    int to;

    done = 0;
    if (other != 0 && other->appearance.cursorTargetX >= 0) {
        from = MenuWidget_CursorCell(node);
        to = MenuWidget_TargetCell(other);
        if (node->selected_base != other->selected_base || from != to) {
            if (node->itemAction(node->selected_base, from, other->selected_base, to)) {
                other->appearance.cursorTargetX = -1;
                Menu_PlayConfirmSound();
                done = 1;
            } else {
                Menu_PlayErrorSound();
                done = 1;
            }
        }
    }
    return done;
}

/* Restores the marked cell of a linked list as its cursor and focuses it. */
static inline int MenuWidget_RestoreLinked(MenuWidgetNode *node, MenuWidgetNode *other)
{
    int done;

    done = 0;
    if (other != 0 && other->appearance.cursorTargetX >= 0) {
        other->cursor_x = other->appearance.cursorTargetX;
        other->appearance.cursorTargetX = -1;
        other->cursor_y = other->target_y;
        node->cursor_x = -1;
        g_MenuWidgetCurrentNode = other;
        done = 1;
    }
    return done;
}

int g_MenuWidgetColumnLayoutMode;
void bzero(void *ptr, int size);

void Draw_AllocColorTri(int width, int height, int arg2);

MenuWidgetNode *g_MenuWidgetFreeListHead;

int g_DrawColorSelect, g_DrawSpriteX, g_DrawSpriteY;
int *g_TextCursorStack;

#define NULL ((void *)0)

int Menu_StepListNavigate(MenuWidgetListNavigation *node, unsigned int flags);



/* Default update handler of the grid list widgets: moves the cursor with the
 * d-pad (crossing into the linked lists at the edges), scrolls, pages the
 * popup list, and swaps or restores marked cells. Returns nonzero when the
 * input was consumed. */
int Menu_StepScrollCursor(MenuWidgetNode *node, unsigned int buttons)
{
    MenuWidgetNode *link;
    int changed;
    int flags;
    int wrap;
    int limit;
    int row;
    int held;
    int margin;

    changed = 0;
    if (g_MenuInputActive)
        flags = Draw_RemapStatusFlags();
    else
        flags = 0;
    if (flags & 0x20) {
        changed = 1;
    } else if (buttons & 0x1000) {
        if (node->cursor_y > 0) {
            node->cursor_y--;
            Menu_PlayMoveSound();
            changed = 1;
        } else if (node->layout_flags & 0x10) {
            changed = 1;
            node->cursor_y = node->y_limit - 1;
            Menu_PlayMoveSound();
        }
        if (node->cursor_y < node->scroll_y && node->scroll_adjust == 0) {
            int old = node->scroll_y;
            int next = old - 1;

            node->scroll_y = next;
            if (next < 0)
                node->scroll_y = 0;
            else if (node->y_limit - node->visible_rows < next)
                node->scroll_y = node->y_limit - node->visible_rows;
            if (old != node->scroll_y)
                node->scroll_adjust = -node->disabled / 2;
        }
        changed |= !(node->layout_flags & 4);
    } else if (buttons & 0x4000) {
        wrap = 0;
        if (node->cursor_x)
            wrap = node->has_scroll != 0;
        margin = wrap + 1;
        if (node->cursor_y < node->y_limit - margin) {
            node->cursor_y++;
            Menu_PlayMoveSound();
            changed = 1;
        } else if (node->layout_flags & 0x10) {
            node->cursor_y = 0;
            Menu_PlayMoveSound();
            changed = 1;
        }
        {
            int old = node->scroll_y;

            if (node->cursor_y >= old + node->visible_rows
                                      - (wrap && node->cursor_y == node->y_limit - 2)
                && node->scroll_adjust == 0) {
                int next = old + 1;

                node->scroll_y = next;
                if (next < 0)
                    node->scroll_y = 0;
                else if (node->y_limit - node->visible_rows < next)
                    node->scroll_y = node->y_limit - node->visible_rows;
                if (old != node->scroll_y)
                    node->scroll_adjust = node->disabled / 2;
            }
        }
        changed |= !(node->layout_flags & 8);
    } else if (buttons & 0x8000) {
        int x = node->cursor_x;

        if (x > 0) {
            node->cursor_x = x == 6 ? 4 : x - 1;
            Menu_PlayMoveSound();
            changed = 1;
        } else {
            link = node->linkedPrevious;
            if (link != 0) {
                int last;

                node->cursor_x = -1;
                limit = link->y_limit - 1;
                link->cursor_x = link->grid_width - 1;
                row = node->cursor_y - node->scroll_y + link->scroll_y;
                if (row < limit)
                    limit = row;
                last = 0;
                link->cursor_y = limit;
                if (link->has_scroll)
                    last = limit == link->y_limit - 1;
                changed = 1;
                g_MenuWidgetCurrentNode = link;
                margin = last + 1;
                link->cursor_x = link->grid_width - margin;
                Menu_PlayMoveSound();
            }
        }
        held = MenuWidget_HeldShoulder();
        if (!(node->layout_flags & 1))
            changed |= 1;
        else
            changed |= held;
    } else if (buttons & 0x2000) {
        int x = node->cursor_x;

        if (x >= 0) {
            int last;

            last = 0;
            if (node->cursor_y == node->y_limit - 1)
                last = node->has_scroll != 0;
            margin = last + 1;
            if (x < node->x_limit - margin) {
                node->cursor_x = x == 4 ? 6 : x + 1;
                Menu_PlayMoveSound();
                changed = 1;
            } else {
                link = node->linkedNext;
                if (link != 0) {
                    node->cursor_x = -1;
                    link->cursor_x = 0;
                    limit = link->y_limit - 1;
                    row = node->cursor_y - node->scroll_y + link->scroll_y;
                    if (row < limit)
                        limit = row;
                    link->cursor_y = limit;
                    g_MenuWidgetCurrentNode = link;
                    Menu_PlayMoveSound();
                    changed = 1;
                }
            }
        }
        held = MenuWidget_HeldShoulder();
        if (!(node->layout_flags & 2))
            changed |= 1;
        else
            changed |= held;
    } else if (buttons & 0x10000) {
        int from;
        int to;

        if (node->itemAction == 0)
            return changed;
        if (node->appearance.cursorTargetX >= 0) {
            from = MenuWidget_CursorCell(node);
            to = MenuWidget_TargetCell(node);
            if (from != to) {
                if (node->itemAction(node->selected_base, from, node->selected_base, to)) {
                    node->appearance.cursorTargetX = -1;
                    Menu_PlayConfirmSound();
                    changed = 1;
                } else {
                    Menu_PlayErrorSound();
                    changed = 1;
                }
            }
        }
        changed |= MenuWidget_SwapMarked(node, node->linkedPrevious);
        changed |= MenuWidget_SwapMarked(node, node->linkedNext);
    } else if (buttons & 0x40) {
        if (node->appearance.cursorTargetX >= 0) {
            node->cursor_x = node->appearance.cursorTargetX;
            node->cursor_y = node->target_y;
            node->appearance.cursorTargetX = -1;
            MenuWidget_ScrollToCursor(node);
            if (node->refreshItems != 0)
                node->refreshItems();
            Menu_PlayCancelSound();
            changed = 1;
        }
        changed |= MenuWidget_RestoreLinked(node, node->linkedPrevious);
        changed |= MenuWidget_RestoreLinked(node, node->linkedNext);
    } else if (node->popup_node != 0 && node == g_MenuWidgetCurrentNode) {
        if (buttons & 4) {
            row = node->cursor_y - node->visible_rows;
            if (row < 0)
                row = 0;
            node->cursor_y = row;
            Menu_StepListNavigate((MenuWidgetListNavigation *)node->popup_node, 0x1000);
            return changed;
        }
        if (buttons & 8) {
            node->cursor_y =
                node->y_limit - MenuWidget_EndMargin(node) < node->cursor_y + node->visible_rows
                    ? node->y_limit - MenuWidget_EndMargin(node)
                    : node->cursor_y + node->visible_rows;
            Menu_StepListNavigate((MenuWidgetListNavigation *)node->popup_node, 0x4000);
            return changed;
        }
    } else if (buttons & 0x20) {
        changed = 1;
    }
    return changed;
}