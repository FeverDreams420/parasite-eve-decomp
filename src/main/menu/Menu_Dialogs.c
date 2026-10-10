/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/memcard_save_state.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"

/* Notification and two-line dialogs, the deferred confirm callback they run,
 * and the memory-card port dialog with its error prompt. */

#define NULL ((void *)0)

void MemCard_InitState(void);
s32 MemCard_IsPortPresent(s32 port);
int MenuWidget_GetCellIndex(MenuWidgetNode *ptr);
s32 MemCard_CheckPresent(s32 port);
void MemCard_StartRead(s32 port, s32 slot);
s32 MemCard_IsOperationPending(void);
s32 Menu_MemCardPortSelectHandler(void *node, s32 flags);
int Menu_IsMemCardSlotSelectable(int arg0);
void Menu_DrawMemCardPortList(int arg0);

void (*g_MenuDeferredCallback)(void);
unsigned int g_McDialogMode;
unsigned int g_MenuMemCardConfirmPending;

void Menu_CreateNotificationDialog(int message, int suffix) {
    MenuWidgetNode *parent, *child;
    short alternate;
    int mode, width;
    u8 *text, *messageText;
    alternate = MenuWidget_FindByModeAndSelectedBase(1, 0x28) != 0;
    mode = 0x28;
    if (alternate) {
        mode = 0x3D;
    }
    parent = MenuWidget_CreateSimpleNode(mode, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(mode, parent, parent);
    parent->draw = Menu_DrawNotificationText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->draw = Menu_DrawNotificationDialogContent;
    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);
    messageText = Str_LookupTable4(message);
    text = D_800A1A20[alternate];
    Util_CopyFFTerminatedBytes(text, messageText);
    if (suffix) {
        Util_AppendFFTerminatedBytes(text, Str_LookupTable4(suffix));
    }
    width = Draw_MeasureTextWidth(text) < 100 ? 100 : Draw_MeasureTextWidth(text);
    parent->grid_width = width + 20;
    parent->x = (300 - width) >> 1;
    child->x = parent->grid_width - 68;
    Queue_Init();
}

void Menu_CloseNotificationDialogs(void) {
    MenuWidget_NavScrollTo(0x28);
    MenuWidget_NavScrollTo(0x3D);
}

void Menu_DrawNotificationText(MenuWidgetNode *node) {
    u8 *text;

    Draw_OffsetCursor(0, 0xA);
    text = g_MenuTwoLineDialogText;
    if (node->selected_base == 0x3D) {
        text += 0x40;
    }
    Draw_PrintCenteredText(text);
}

void Menu_CreateTwoLineDialog(int line0_id, int line1_id) {
    MenuWidgetNode *parent;
    MenuWidgetNode *child;
    u8 *line0;
    u8 *line1;
    u8 *text;
    int width;
    parent = MenuWidget_CreateSimpleNode(0x28, MenuWidget_GetCurrentNode(), 0, 1);
    child = MenuWidget_CreateNode(0x28, parent, parent);

    parent->draw = Menu_DrawTwoLineDialogText;
    parent->update = Menu_HandleDeferredCallbackInput;
    child->draw = Menu_DrawNotificationDialogContent;

    MenuWidget_SetCurrentNode(child);
    Menu_SetDeferredCallback(0);

    text = Str_LookupTable4(line0_id);
    line0 = D_800A1A20[0];
    Util_CopyFFTerminatedBytes(line0, text);

    text = Str_LookupTable4(line1_id);
    line1 = line0 + 0x40;
    Util_CopyFFTerminatedBytes(line1, text);

    if (Draw_MeasureTextWidth(line0) > Draw_MeasureTextWidth(line1)) {
        if (Draw_MeasureTextWidth(line0) >= 100)
            goto measure_wide;
        width = 100;
        /* Keep the short-width branch distinct from the second-line test. */
        asm("");
        goto width_ready;
    }
    if (Draw_MeasureTextWidth(line1) < 100) {
        width = 100;
        goto width_ready;
    }
measure_wide:
    {
        u8 *first = D_800A1A20[0];
        u8 *second = first + 0x40;
        width = Draw_MeasureTextWidth(first) > Draw_MeasureTextWidth(second)
            ? Draw_MeasureTextWidth(first) : Draw_MeasureTextWidth(second);
    }
width_ready:
    /* Do not pull layout arithmetic into the preceding jump delay slot. */
    asm("");
    parent->grid_width = width + 0x14;
    parent->x = (0x12C - width) >> 1;
    parent->visible_rows += 0xE;
    child->x = parent->grid_width - 0x44;
    child->y += 0xE;

    Queue_Init();
}

void Menu_DrawTwoLineDialogText(void) {
    Draw_OffsetCursor(0, 0xA);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText);
    Draw_OffsetCursor(0, 0xE);
    Draw_PrintCenteredText(g_MenuTwoLineDialogText + 0x40);
}

void Menu_SetDeferredCallback(void (*callback)(void)) {
    g_MenuDeferredCallback = callback;
}

int Menu_HandleDeferredCallbackInput(MenuWidgetNode *node, int arg1) {
    if ((arg1 & 0x10040) != 0) {
        MenuWidget_DestroyNode(node);
        if (g_MenuDeferredCallback != 0) {
            g_MenuDeferredCallback();
            g_MenuDeferredCallback = 0;
        }
        Menu_PlayConfirmSound();
    }
    return 1;
}

void Menu_OpenMemCardSelectDialog(s32 arg0) {
    MenuWidgetNode *temp_a0;
    MenuWidgetNode *temp_v0;

    temp_v0 = MenuWidget_CreateSimpleNode(0x24, MenuWidget_GetCurrentNode(), 0, 0);
    temp_a0 = MenuWidget_CreateNode(0x24, temp_v0, temp_v0);
    temp_v0->update = (void (*)(void))Menu_MemCardPortSelectHandler;
    temp_a0->draw = (void (*)(void))Menu_DrawMemCardPortList;
    temp_a0->selectionAvailable = Menu_IsMemCardSlotSelectable;
    if (temp_a0->cursor_y == 2) {
        temp_a0->cursor_y = 0;
    }
    MenuWidget_SetCurrentNode(temp_a0);
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
        MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = (void (*)(void))Menu_DrawContextHelpText;
    }
    MenuWidget_FindByModeAndSelectedBase(1, 0x13)->visible_rows = 0x24;
    g_McDialogMode = arg0;
    if (arg0 != 0) {
        MemCard_SetDialogActive(1);
        MemCard_InitState();
    }
    Queue_Init();
}

void Menu_OpenStartupMemCardDialog(void) {
    s32 one;
    void *callback_2c;
    void *callback_30;
    void *callback_8c;
    MenuWidgetNode *child;
    MenuWidgetNode *parent;

    parent = MenuWidget_CreateSimpleNode(0x24, MenuWidget_GetCurrentNode(), 0, 0);
    child = MenuWidget_CreateNode(0x24, parent, parent);
    callback_2c = Menu_MemCardPortSelectHandler;
    callback_30 = Menu_DrawMemCardPortList;
    parent->update = (void (*)(void))callback_2c;
    child->draw = (void (*)(void))callback_30;
    callback_8c = Menu_IsMemCardSlotSelectable;
    child->selectionAvailable = (int (*)(int))callback_8c;
    one = 1;
    if (child->cursor_y == 2) {
        child->cursor_y = 0;
    }
    MenuWidget_SetCurrentNode(child);
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x13) == NULL) {
        MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = (void (*)(void))Menu_DrawContextHelpText;
    }
    MenuWidget_FindByModeAndSelectedBase(1, 0x13)->visible_rows = 0x24;
    g_McDialogMode = one;
    MemCard_SetDialogActive(1);
    MemCard_InitState();
    Queue_Init();
}

unsigned int MemCard_GetDialogMode(void) {
    return g_McDialogMode;
}

void Menu_SetMemCardConfirmPending(void) {
    g_MenuMemCardConfirmPending = 1;
}

void Menu_ShowMemCardErrorDialog(int arg0) {
    if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
        Menu_CreateTwoLineDialog(arg0 + 0x47, g_McDialogMode + 0x42);
    }
}

s32 Menu_MemCardPortSelectHandler(void *node, s32 flags) {
    s32 cellIndex;

    cellIndex = MenuWidget_GetCellIndex(MenuWidget_GetChild(node, 0));
    if (flags & 0x10000) {
        if (cellIndex >= 0) {
            if (cellIndex >= 2) {
            if (cellIndex == 2) {
                goto close_dialog;
            }
            goto error_sound;
            }
read_card:
            if ((MemCard_CheckPresent(cellIndex) != 0) && (MemCard_IsPortPresent(cellIndex) != 0) && (MemCard_IsOperationPending() == 0)) {
                MemCard_CloseAllAndResetState();
                MemCard_StartRead(cellIndex, 1);
                Menu_PlayConfirmSound();
                return 1;
            }
            return 1;
close_dialog:
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x25));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x26));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x24));
            MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
            MemCard_SetDialogActive(0);
            Inv_SetActiveList(9, 0);
            g_MenuMemCardConfirmPending = 0;
            Menu_PlayConfirmSound();
            return 1;
        }
error_sound:
        Menu_PlayErrorSound();
        return 1;
    }
    if (flags & 0x40) {
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x25));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x26));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x24));
        MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
        MemCard_SetDialogActive(0);
        Inv_SetActiveList(9, 0);
        g_MenuMemCardConfirmPending = 0;
        Menu_PlayCancelSound();
    }
    return 1;
}

int Menu_IsMemCardDialogOpen(void) {
    return MenuWidget_FindByModeAndSelectedBase(1, 0x24) != 0;
}
