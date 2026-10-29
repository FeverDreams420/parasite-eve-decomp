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

static inline GpuCmdPacket *AllocRect(DrawAreaRect *rect)
{
    u8 *old = g_DrawPacketCursor;
    u8 *next = old + sizeof(GpuCmdPacket);
    GpuCmdPacket *packet = 0;
    if (next < g_DrawPacketArenaBase + 0x4000) {
        g_DrawPacketCursor = next;
        packet = (GpuCmdPacket *)old;
    } else {
        Inv_BoundsCheckAssertStub(1);
    }
    if (packet) SetDrawArea(packet, rect);
    return packet;
}

static inline void GetPosition(MenuWidgetNode *node, u32 *x, u32 *y)
{
    *y = 0;
    *x = 0;
    while (node) {
        *x += node->x;
        *y += node->y;
        node = FindOwner(node);
    }
}

static inline GpuCmdPacket *AllocArea(u32 x, u32 y, u32 width, u32 height)
{
    DrawAreaRect rect;
    if (g_DrawBufferIndex) y += 0xE0;
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
    return AllocRect(&rect);
}

static inline u32 Dimension(u32 size, u32 count)
{
    return size * count;
}

static inline void RowMoveCursor(u32 dx, u32 dy)
{
    g_TextCursorX = g_TextCursorX + dx;
    g_TextCursorY = g_TextCursorY + dy;
}

static inline void RowPopCursor(void)
{
    CursorPair *stack = (CursorPair *)g_TextCursorStack;
    if ((CursorPair *)g_TextCursorStackBottom < stack) {
        g_TextCursorStack = (int *)(stack - 1);
        g_TextCursorX = stack[-1].x;
        g_TextCursorY = stack[-1].y;
    } else {
        Inv_BoundsCheckAssertStub(3);
    }
}

static inline void SetCursor(u32 x, u32 y)
{
    g_TextCursorX = x;
    g_TextCursorY = y;
}

static inline void MoveCursor(u32 dx, u32 dy)
{
    SetCursor(g_TextCursorX + dx, g_TextCursorY + dy);
}

static inline void PushCursor(void)
{
    DrawTextCursorPair *stack = (DrawTextCursorPair *)g_TextCursorStack;
    if (stack < (DrawTextCursorPair *)g_TextCursorStackTop) {
        /* Capture both coordinates before writing into the shared stack. */
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

/* Coordinate offsets use the low 32 bits of the original MIPS arithmetic. */
static inline u32 Product(u32 size, u32 count)
{
    return size * count;
}

static inline u32 Difference(u32 a, u32 b)
{
    return a - b;
}

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
        MenuWidget_ScrollToCursor(other);
        g_MenuWidgetCurrentNode = other;
        done = 1;
    }
    return done;
}

int g_MenuWidgetColumnLayoutMode;
void bzero(void *ptr, int size);

void Draw_AllocColorTri(int width, int height, int arg2);

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

int g_DrawColorSelect, g_DrawSpriteX, g_DrawSpriteY;
int *g_TextCursorStack;

static inline void Offset(int x, int y) {
    g_DrawSpriteX += x;
    g_DrawSpriteY += y;
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

#define NULL ((void *)0)
