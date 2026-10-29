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

void MenuWidget_SetCurrentNode(MenuWidgetNode *node) {
    g_MenuWidgetCurrentNode = node;
}

MenuWidgetNode *MenuWidget_GetCurrentNode(void) {
    return g_MenuWidgetCurrentNode;
}

void MenuWidget_SaveAndSetCurrentNode(MenuWidgetNode *arg0) {
    g_MenuWidgetSavedNode = g_MenuWidgetCurrentNode;
    g_MenuWidgetCurrentNode = arg0;
}

void MenuWidget_RestoreSavedCurrentNode(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *pending;

    node = g_MenuWidgetActiveListHead;
    if (node != 0) {
        pending = g_MenuWidgetSavedNode;
        while (node != 0) {
            if (node == pending) {
                break;
            }
            node = node->next;
        }
        if (node != 0) {
            g_MenuWidgetCurrentNode = node;
        }
    }
    g_MenuWidgetSavedNode = 0;
}

static inline MenuWidgetNode *MenuWidget_AllocNode(MenuWidgetNode *arg0, MenuWidgetNode *arg1) {
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

MenuWidgetNode *MenuWidget_CreateSimpleNode(s32 mode, MenuWidgetNode *parent_arg, MenuWidgetNode *parent, s32 arg_flag) {
    MenuWidgetSimpleDescriptor *desc;
    MenuWidgetNode *node;

    desc = MenuWidget_LookupSimpleDescriptor(mode);
    if (desc == 0) {
        Inv_BoundsCheckAssertStub(0xC);
    }

    node = MenuWidget_AllocNode(parent_arg, parent);

    if (node == 0) {
        Inv_BoundsCheckAssertStub(0xD);
    }

    if (arg_flag == 0) {
        MenuWidgetNode *found;

        found = MenuWidget_FindLastMode1WithCursorX();
        if (found != 0) {
            MenuWidgetNode *head;
            MenuWidgetNode *found_next;
            MenuWidgetNode *node_next;

            head = g_MenuWidgetActiveListHead;
            found_next = found->next;
            node_next = head->next;
            head->next = found_next;
            found->next = head;
            g_MenuWidgetActiveListHead = node_next;
        }
    }

    node->mode = 1;
    node->selected_base = mode;
    if (desc->x != 0) {
        node->x = desc->x;
        node->y = desc->y;
    } else {
        node->x = 0xA0 - (desc->width >> 1);
        node->y = (desc->y != 0 ? 0x50 : 0x78) - (desc->height >> 1);
    }

    node->grid_width = desc->width;
    node->visible_rows = desc->height;
    node->draw_state = 0;
    node->disabled = 0;
    node->cursor_x = arg_flag;
    node->cursor_y = 0;
    node->appearance.cursorTargetX = 0;
    return node;
}

void MenuWidget_DestroyNode(MenuWidgetNode *node) {
    MenuWidget_DestroyNodeRecursive(node);
}

void MenuWidget_NavScrollTo(int selected_base) {
    MenuWidgetNode *node;
    int one;

    node = g_MenuWidgetActiveListHead;
    one = 1;
    while (node != 0) {
        if (node->mode == one && node->selected_base == selected_base) {
            break;
        }
        node = node->next;
    }

    MenuWidget_DestroyNodeRecursive(node);
}

void MenuWidget_InitPool(void) {
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
    g_MenuWidgetFreeListHead = g_MenuWidgetNodePool;
    g_MenuWidgetCurrentNode = 0;
    g_MenuWidgetActiveListHead = 0;
}

void Draw_PushPrimToList(MenuWidgetNode *node);

int g_TextCursorX;
int g_TextCursorY;

void MenuWidget_UpdateAndDraw(void) {
    MenuWidgetNode *node;
    int any_flag;
    int order_flag;

    node = g_MenuWidgetActiveListHead;
    order_flag = 0;
    any_flag = 0;
    if (node != 0) {
        do {
            if (node->mode == 1) {
                node->draw_state = 0;
                any_flag |= node->cursor_x;
            }
            node = node->next;
        } while (node != 0);
    }

    node = g_MenuWidgetCurrentNode;
    if (node != 0) {
        do {
            if (node->mode == 1) {
                node->draw_state = order_flag;
                order_flag = 1;
            }
            node = node->parent;
        } while (node != 0);
    }

    node = g_MenuWidgetActiveListHead;
    if (node != 0) {
        do {
            int enabled;
            int extra;

            if (node->mode == 1 && node->cursor_y == 0) {
                if (node->disabled != 0) {
                    enabled = 0;
                } else {
                    enabled = node->draw_state & 1;
                }
                node->draw_state = enabled;

                extra = 0;
                if (any_flag != 0) {
                    extra = (unsigned int)node->cursor_x < 1;
                }

                node->draw_state = enabled | extra;
                g_DrawTextDimmed = enabled | extra;
                g_TextCursorX = 0;
                g_TextCursorY = 0;
                g_DrawTextBoxWidth = node->grid_width;
                Draw_PushPrimToList(node);
                g_TextCursorX = node->x;
                g_TextCursorY = node->y;
                g_DrawTextDimmed = node->draw_state;
                Draw_AllocColorGradient(node->grid_width, node->visible_rows, node->appearance.gradientPoints, node->cursor_x);
            }
            node = node->next;
        } while (node != 0);
    }
}

void MenuWidget_OffsetPosition(MenuWidgetNode *ptr, int dx, int dy) {
    if (ptr != 0) {
        ptr->x += dx;
        ptr->y += dy;
        g_TextCursorX += dx;
        g_TextCursorY += dy;
    }
}

void MenuWidget_ClearCursorY(MenuWidgetNode *ptr) {
    if (ptr) {
        ptr->cursor_y = 0;
    }
}

void MenuWidget_SetCursorY(MenuWidgetNode *ptr) {
    if (ptr) {
        ptr->cursor_y = 1;
    }
}

int MenuWidget_IsCursorYClear(MenuWidgetNode *ptr) {
    if (ptr) {
        return ptr->cursor_y == 0;
    }

    return 0;
}

MenuWidgetNode *MenuWidget_FindLastMode1WithCursorX(void) {
    MenuWidgetNode *node;
    MenuWidgetNode *result;

    node = g_MenuWidgetActiveListHead;
    result = 0;
    while (node != 0) {
        if (node->mode == 1 && node->cursor_x != 0) {
            result = node;
        }
        node = node->next;
    }
    return result;
}

MenuWidgetNode *MenuWidget_CreateNode(s32 mode, MenuWidgetNode *parent_arg, MenuWidgetNode *parent) {
    MenuWidgetGridDescriptor *desc;
    MenuWidgetNode *node;
    int tmp;

    desc = MenuWidget_LookupGridDescriptor(mode);
    if (desc == 0) {
        Inv_BoundsCheckAssertStub(0xE);
    }

    node = MenuWidget_AllocNode(parent_arg, parent);

    if (node == 0) {
        Inv_BoundsCheckAssertStub(0xF);
    }

    node->mode = 2;
    node->aux_index = mode;
    node->selected_base = mode;
    {
        int t;
        int u;

        t = desc->x;
        node->x = t;
        u = desc->y;
        node->update = Menu_StepScrollCursor;
        node->y = u;
        t = desc->gridWidth;
        node->x_limit = t;
        node->grid_width = t;
        t = desc->visibleRows;
        node->visible_rows_mirror = t;
        node->visible_rows = t;
        t = desc->yLimit;
        node->y_limit = t;
        t = desc->initialDrawState;
        node->draw_state = t;
        u = desc->initialDisabled;
        tmp = u;
    }
    node->cursor_y = 0;
    node->cursor_x = 0;
    node->target_y = -1;
    node->appearance.cursorTargetX = -1;
    node->scroll_y = 0;
    node->scroll_adjust = 0;
    node->disabled = tmp;
    tmp = desc->layoutFlags;
    node->linkedNext = 0;
    node->linkedPrevious = 0;
    node->itemAction = 0;
    node->refreshItems = 0;
    node->has_scroll = 0;
    node->layout_flags = tmp;
    node->selectionAvailable = 0;
    node->popup_node = 0;
    if (node->visible_rows < node->y_limit) {
        Draw_SwapPrimBuffers(node);
    }
    MenuWidget_ApplyColumnLayout(node);
    return node;
}

int MenuWidget_GridCellIndex(MenuWidgetNode *ptr) {
    int ret;
    int x;
    int y;

    ret = -1;
    if (ptr != 0) {
        x = ptr->cursor_x;
        if (x >= 0) {
            y = ptr->cursor_y;
            if (y >= 0) {
                ret = (ptr->grid_width * y) + x;
            }
        }
    }

    return ret;
}

int MenuWidget_GetCellIndex(MenuWidgetNode *ptr) {
    int idx;
    int ret;
    int x;
    int y;

    idx = -1;
    if (ptr != 0) {
        x = ptr->cursor_x;
        if (x >= 0) {
            y = ptr->cursor_y;
            idx = y;
            if (y >= 0) {
                idx = (ptr->grid_width * y) + x;
            } else {
                idx = -1;
            }
        }
    }

    ret = -1;
    if (((ptr->cell_mask >> idx) & 1) != 0) {
        ret = idx;
    }
    return ret;
}

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

void MenuWidget_DrawListRow(MenuWidgetNode *node,
                            void (*draw_callback)(int),
                            int row, int draw_cursor)
{
    u32 index = node->x_limit * (row + node->scroll_y);
    /* Stock MIPS GCC emits SLLV, which masks the shift count to five bits. */
    u32 bit = 1u << index;
    int x, enabled, dimmed;
    CursorPair *stack = (CursorPair *)g_TextCursorStack;
    int (*select_callback)(int);

    if (stack < (CursorPair *)g_TextCursorStackTop) {
        g_TextCursorStack = (int *)(stack + 1);
        stack->x = g_TextCursorX;
        stack->y = g_TextCursorY;
    } else {
        Inv_BoundsCheckAssertStub(2);
    }
    g_TextCursorX += 2;
    g_TextCursorY += 2;
    for (x = 0; x < node->grid_width; x++) {
        select_callback = node->selectionAvailable;
        enabled = 1;
        if (select_callback) {
            enabled = select_callback(index++);
            node->cell_mask &= ~bit;
            if (enabled) node->cell_mask |= bit;
            bit <<= 1;
        }
        dimmed = 0;
        if (!enabled || (draw_cursor &&
            (x != node->cursor_x || row + node->scroll_y != node->cursor_y)))
            dimmed = 1;
        g_DrawTextDimmed = dimmed;
        if (draw_callback)
            draw_callback(node->grid_width * (node->scroll_y + row) + x);
        if ((VSync(-1) & 8) && x == node->appearance.cursorTargetX &&
            row + node->scroll_y == node->target_y) {
            g_TextCursorX -= 2;
            g_TextCursorY -= 2;
            Draw_AllocColorTri(node->draw_state, node->disabled, 0);
            g_TextCursorX += 2;
            g_TextCursorY += 2;
        }
        RowMoveCursor(node->draw_state, 0);
    }
    RowPopCursor();
    RowMoveCursor(0, node->disabled);
}

void Draw_AllocPrimWithMask(MenuWidgetNode *node)
{
    u32 x, y;
    GpuCmdPacket *packet;
    GetPosition(node, &x, &y);
    packet = AllocArea(x, y, Dimension(node->draw_state, node->grid_width),
                      Dimension(node->disabled, node->visible_rows));
    Draw_LinkDrawArea(g_DrawOrderingTableEntry, packet);
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

void MenuWidget_DrawList(MenuWidgetNode *node, void (*draw_callback)(int))
{
    MenuWidgetNode *parent = FindOwner(node);
    /* Snapshot once: callbacks may change the parent's draw state. */
    int draw_cursor = parent->draw_state;
    DrawAreaRect rect;
    GpuCmdPacket *packet;
    int row, offset, highlight;
    g_DrawTextPosX = node->draw_state;
    g_MenuListRowHeight = node->disabled;
    rect.x = 0; rect.y = 0; rect.w = 320; rect.h = 224;
    if (g_DrawBufferIndex) rect.y = 224;
    packet = AllocRect(&rect);
    Draw_LinkDrawArea(g_DrawOrderingTableEntry, packet);
    PushCursor();
    MoveCursor(0, node->scroll_adjust);
    if (node->scroll_adjust > 0) {
        MoveCursor(0, 0u - node->disabled);
        MenuWidget_DrawListRow(node, draw_callback, -1, draw_cursor);
    }
    /* Row callbacks may change the viewport; reload its bounds each time. */
    for (row = 0; row < node->visible_rows; row++)
        MenuWidget_DrawListRow(node, draw_callback, row, draw_cursor);
    if (node->scroll_adjust < 0)
        MenuWidget_DrawListRow(node, draw_callback, row, draw_cursor);
    PopCursor();
    Draw_AllocPrimWithMask(node);
    MenuWidget_EaseNodePosition(node->popup_node);
    offset = node->scroll_adjust;
    if (offset) {
        if (offset > 0) {
            node->scroll_adjust = offset - node->disabled / 2;
            if (node->scroll_adjust < 0) node->scroll_adjust = 0;
        } else {
            node->scroll_adjust = offset + node->disabled / 2;
            if (node->scroll_adjust > 0) node->scroll_adjust = 0;
        }
        if (node->scroll_adjust) return;
    }
    if (node->cursor_x >= 0 && node->cursor_y >= node->scroll_y &&
        node->cursor_y < node->scroll_y + node->visible_rows) {
        PushCursor();
        g_TextCursorX += Product(node->draw_state, node->cursor_x);
        g_TextCursorY += Product(node->disabled, Difference(node->cursor_y, node->scroll_y));
        highlight = 0;
        if (!(node->layout_flags & 0x80) && g_MenuWidgetCurrentNode == node) {
            int blink = g_MenuInputActive && (Draw_RemapStatusFlags() & 0x20);
            if (blink) highlight = 1;
        }
        Draw_AllocColorTriGradient(node->draw_state, node->disabled, highlight,
                                   g_MenuWidgetCurrentNode == node);
        PopCursor();
    }
}

void MenuWidget_ClampScroll(MenuWidgetNode *ptr) {
    int y;
    int top;
    int height;
    int next;

    y = ptr->cursor_y;
    top = ptr->scroll_y;
    if (y < top) {
        ptr->scroll_y = y;
    } else {
        height = ptr->visible_rows;
        if (y >= top + height) {
            next = y - height;
            ptr->scroll_y = next + 1;
        }
    }
}

void MenuWidget_ClampCursor(MenuWidgetNode *ptr, int x, int y) {
    int limit;
    int top;
    int height;
    int next;

    if (x < 0) {
        x = 0;
    } else {
        limit = ptr->x_limit;
        if (x >= limit) {
            x = limit - 1;
        }
    }

    if (y < 0) {
        y = 0;
    } else {
        limit = ptr->y_limit;
        if (y >= limit) {
            y = limit - 1;
        }
    }

    top = ptr->scroll_y;
    ptr->cursor_x = x;
    ptr->cursor_y = y;
    if (y < top) {
        ptr->scroll_y = y;
    } else {
        height = ptr->visible_rows;
        if (y >= top + height) {
            next = y - height;
            ptr->scroll_y = next + 1;
        }
    }
}

int g_MenuWidgetColumnLayoutMode;
void bzero(void *ptr, int size);

void Draw_AllocColorTri(int width, int height, int arg2);

MenuWidgetNode *g_MenuWidgetFreeListHead;

int g_DrawColorSelect, g_DrawSpriteX, g_DrawSpriteY;
int *g_TextCursorStack;

#define NULL ((void *)0)
