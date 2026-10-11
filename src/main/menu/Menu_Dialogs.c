/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */
#include "common.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/memcard_save_state.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_widget.h"
#include "pe1/text.h"
#include "pe1/menu_flags.h"

/* Parasite-spell info and context help panels, notification and two-line
 * dialogs, the deferred confirm callback they run, and the memory-card port
 * dialog with its error prompt. */

#define NULL ((void *)0)

void MemCard_InitState(void);
s32 MemCard_IsPortPresent(s32 port);
s32 MemCard_CheckPresent(s32 port);
void MemCard_StartRead(s32 port, s32 slot);
s32 MemCard_IsOperationPending(void);
s32 Menu_MemCardPortSelectHandler(void *node, s32 flags);
int Menu_IsMemCardSlotSelectable(int arg0);
void Menu_DrawMemCardPortList(int arg0);

int g_McDialogMode;
int g_MenuMemCardConfirmPending;
int D_8009CFF4;

void Menu_DrawParasiteSpellInfoPanel(void);

void Menu_CreateParasiteSpellInfoPanel(int arg0) {
    MenuWidgetNode *widget;

    if (MenuWidget_FindByModeAndSelectedBase(1, 0x1E) == 0) {
        widget = MenuWidget_CreateSimpleNode(0x1E, 0, 0, 0);
        widget->draw = Menu_DrawParasiteSpellInfoPanel;
        MenuWidget_CreateSimpleNode(0x1C, 0, 0, 0);
    }

    D_8009CFF4 = arg0;
}

void Menu_DrawParasiteSpellInfoPanel(void) {
    Draw_OffsetCursor(2, 4);
    Draw_PrintCenteredText(Str_LookupTable4(0x26));
    Draw_OffsetCursor(0, 0x10);
    Draw_PrintCenteredText(Str_LookupTable8(D_8009CFF4 + 0xEB));
    Draw_OffsetCursor(2, 0x14);
    Draw_PrintRawText(Str_LookupTableC(D_8009CFF4 + 0xEB));
}

void Menu_CreateContextHelpPanel(void) {
    void *node = MenuWidget_FindByModeAndSelectedBase(1, 0x13);

    if (node == 0) {
        MenuWidget_CreateSimpleNode(0x13, 0, 0, 0)->draw = Menu_DrawContextHelpText;
    }
}

void Menu_CloseContextHelpPanel(void) {
    MenuWidget_DestroyNode(MenuWidget_FindByModeAndSelectedBase(1, 0x13));
}

/* Draws the play-time clock and the help line for the focused menu widget:
 * the item description, ability or option hint selected by its grid cell. */
void Menu_DrawContextHelpText(MenuWidgetNode *panel)
{
    MenuWidgetNode *current;
    ItemDataRecord *data;
    u8 *text;
    int index;
    int mask;
    int value;
    int hint;
    int i;

    text = 0;
    current = MenuWidget_GetCurrentNode();
    Draw_StatePush();
    Draw_OffsetCursor(panel->grid_width - 0x27, 1);
    Draw_PrintTimeValue(GameTime_GetCounterSeconds(2), 1);
    Draw_OffsetCursor(-0x21, 0);
    Draw_AllocSprite(0x8C);
    Draw_StatePop();
    if (current != 0) {
        index = MenuWidget_GridCellIndex(
            MenuWidget_FindByModeAndSelectedBase(2, current->selected_base));
        switch (current->selected_base) {
        case 0:
            if (Menu_GetBattleEquipMode())
                mask = D_8009CEF0 & 0x1F;
            else
                mask = D_8009CEF0 & 0x1EF;
            i = -1;
            do {
                int bit;

                if (index < 0)
                    break;
                bit = mask & 1;
                index -= bit;
                i++;
                mask >>= 1;
            } while (i < 9);
            text = Str_LookupTable10(i + 0x28);
            break;
        case 5:
            if (index < 0)
                break;
            data = Inv_LookupActiveListData(D_8009CF18 ? D_800C0E20.tracked[0]
                                                       : D_800C0E20.tracked[2]);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 1:
            if (D_8009CF8C >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(D_8009CF8C) < 0x13
                    || Inv_GetActiveListItemType(D_8009CF8C) >= 0x16)
                    hint = 1;
                text = Str_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 51:
            if (D_8009CF8C >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(D_8009CF8C) < 0x13
                    || Inv_GetActiveListItemType(D_8009CF8C) >= 0x16)
                    hint = 1;
                text = Str_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupActiveListData(Inv_GetWayneListItemByIndex(index));
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 52:
            if (D_8009CF8C >= 0) {
                hint = 0;
                if (Inv_GetActiveListItemType(D_8009CF8C) < 0x13
                    || Inv_GetActiveListItemType(D_8009CF8C) >= 0x16)
                    hint = 1;
                text = Str_LookupTable4(hint | 0xE);
                Draw_SetColor(0x408040);
                break;
            }
            data = Inv_LookupItemData(index);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 7:
            if (D_8009CF1C)
                index = Inv_RestoreSelection(1);
            else
                index = Inv_GetPackedListItem(index);
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 8:
            if (index < Inv_GetPackedListCount())
                text = Str_LookupTableC(Inv_GetPackedListItem(index) + 0xEB);
            break;
        case 12:
            text = Str_LookupTable10(index + 0x4F);
            break;
        case 13:
        case 16:
            if (D_8009CEFC) {
                text = Str_LookupTableC(Menu_GetBattleCountEntry(index) - 1);
                break;
            }
            data = Inv_LookupActiveListData(Inv_GetPackedListItem(index));
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 14:
            Inv_SelectActiveList(0);
            data = Inv_LookupActiveListData(index);
            if (data != 0)
                text = Str_LookupTableC(data->itemId - 1);
            break;
        case 6:
            value = D_8009CF20->tailData[index] & 0x1F;
            if (value != 0)
                text = Str_LookupTable10(value + (1 - D_8009CF18) * 20 - 1);
            break;
        case 11:
            data = Inv_LookupActiveListData(Inv_RestoreSelection(1));
            value = data->tailData[index] & 0x1F;
            if (value != 0)
                text = Str_LookupTable10(value + (1 - D_8009CF18) * 20 - 1);
            break;
        case 17:
            text = Str_LookupTable4(0x27);
            break;
        case 27:
        case 28:
            text = Str_LookupTable10(0x52);
            break;
        case 29:
            {
                int base = D_8009CF18 * 3 - 0x56;

                text = Str_LookupTable10(index - base);
            }
            break;
        case 32:
            if (index < 4)
                text = Str_LookupTable10(index + 0x31);
            else
                text = 0;
            break;
        case 33:
            text = Str_LookupTable10(index + 0x3A);
            break;
        case 35:
            text = Str_LookupTable10(index + 0x3D);
            break;
        case 36:
            if (MemCard_PollTransferDelay()) {
                Draw_OffsetCursor(6, 5);
                Draw_PrintTextById(0x4C);
                Draw_OffsetCursor(0, 0x10);
                Draw_PrintTextById(0x4D);
                break;
            }
            hint = 0;
            if (MemCard_IsPortPresent(0) || MemCard_IsPortPresent(1))
                hint = 1;
            text = Str_LookupTable4(hint | 0x4E);
            break;
        case 37:
        case 38:
            if (D_8009CF50) {
                if (D_8009CFF8) {
                    Draw_SetTextDimmed(0);
                    Draw_OffsetCursor(6, 5);
                    Draw_PrintTextById(0x58);
                    Draw_OffsetCursor(0, 0x10);
                    Draw_PrintTextById(0x59);
                    break;
                }
                text = Str_LookupTable4(0x5C);
                break;
            }
            text = Str_LookupTable4(Save_GetTitleStyleFlag() ? 0x57 : 0x5B);
            break;
        case 39:
            Draw_SetTextDimmed(0);
            Draw_OffsetCursor(6, 5);
            Draw_PrintTextById(0x50);
            Draw_OffsetCursor(0, 0x10);
            Draw_PrintTextById(0x51);
            break;
        case 46:
            text = Str_LookupTable10(index + 0x3F);
            break;
        case 48:
            text = Str_LookupTable10(index + 0x38);
            break;
        case 49:
            text = Str_LookupTable10(index + 0x42);
            break;
        case 50:
            /* Retail keeps the table base in a register for the row-4
             * override and recomputes the row address on both paths. */
            {
                u8 *hints = D_8009234C;
                u8 *hint;

                if (D_8009CF0C == 1) {
                    if (index == 2)
                        hint = &hints[4];
                    else
                        hint = &D_8009234C[index];
                } else {
                    hint = &D_8009234C[index];
                }
                text = Str_LookupTable10(*hint);
            }
            break;
        case 56:
            Draw_SetTextDimmed(0);
            text = Str_LookupTable10(0x44);
            break;
        case 58:
            text = Str_LookupTable10(index + 0x35);
            break;
        case 59:
            if (D_8009CF0C == 2)
                text = Str_LookupTable10(index + 0x4D);
            else
                text = Str_LookupTable10(index + 0x4B);
            break;
        case 60:
            {
                int base = D_8009CF18 * 3 - 0x48;

                text = Str_LookupTable10(index - base);
            }
            break;
        }
    }
    if (text != 0) {
        Draw_OffsetCursor(6, 5);
        Draw_PrintRawText(text);
        Draw_SetColor(0x808080);
    }
}

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
    }
    else
    {
    if (Draw_MeasureTextWidth(line1) < 100) {
        width = 100;
    }
    else
    {
measure_wide:
    {
        u8 *first = D_800A1A20[0];
        u8 *second = first + 0x40;
        width = Draw_MeasureTextWidth(first) > Draw_MeasureTextWidth(second)
            ? Draw_MeasureTextWidth(first) : Draw_MeasureTextWidth(second);
    }
    }
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
            }
            else
            {
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
