#include "pe1/menu_widget.h"
#include "pe1/menu_confirm_callback.h"
#include "common.h"
#include "pe1/memcard.h"
#include "pe1/inventory_queries.h"
#include "pe1/menu_notification_dialog.h"
#include "pe1/menu_sound.h"
#include "pe1/text.h"
#include "pe1/menu_flags.h"
#include "pe1/menu_dialog.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --dont-expand-li */
/* ASPSX_VERSION: 2.21 */

#define NULL ((void *)0)
#include "../../../tools/m2c/m2c_macros.h"
M2C_UNK Save_SetTitleStyleFlag();
s32 func_80042964();
M2C_UNK func_80042B50();
#include "Menu_StepSaveSelect_externs.h"
#define g_MenuItemLabelBuffer (*(u8 *)&D_800A1980_o)
#define Menu_DrawItemLabel (*(M2C_UNK *)&Menu_DrawItemLabel_o)
#define Menu_ConfirmDialogHandler (*(M2C_UNK *)&func_80044E98_o)
#define Menu_DrawActionOptionList (*(M2C_UNK *)&func_8004F950_o)
#define func_800504F4 (*(M2C_UNK *)&func_800504F4_o)
#define func_8005051C (*(M2C_UNK *)&func_8005051C_o)

s32 Menu_StepSaveSelect(MenuWidgetNode *arg0, s32 arg1) {
    M2C_UNK *var_a0;
    s32 temp_s0;
    MenuWidgetNode *temp_s2;
    s32 temp_v0;
    register s32 temp_cf50 asm("$5");
    s32 temp_height;
    s32 var_v1;
    s32 temp_a0;
    MemCardSaveSlot *temp_v0_2;
    MenuWidgetNode *temp_s2_2;
    MenuWidgetNode *temp_v0_3;

    temp_s2 = MenuWidget_GetChild(arg0, 0);
    if (arg1 & 0x40) {
        MenuWidget_NavScrollTo(0x3F);
        MenuWidget_DestroyNode(arg0);
        MemCard_CloseAllAndResetState();
        Menu_PlayCancelSound();
        return 1;
    }
    if (arg1 & 0x10000) {
        temp_v0 = MenuWidget_GetCellIndex(temp_s2);
        g_SaveSlotCellIndex = temp_v0;
        if (temp_v0 >= 0) {
            temp_v0_2 = MemCard_GetSlot(g_SaveSelectedSlot, temp_v0);
            if (temp_v0_2 != NULL) {
                temp_cf50 = g_McDialogMode;
                if ((temp_cf50 != 0) || (temp_v0_2->state != MEMCARD_SLOT_EMPTY)) {
                    temp_v0 = temp_v0_2->state;
                    D_8009CF4C = temp_v0;
                    if (temp_cf50 != 0) {
                        Save_SetTitleStyleFlag(g_MenuMemCardConfirmPending, temp_cf50);
                        if (D_8009CF4C != MEMCARD_SLOT_EMPTY) {
                            temp_v0_3 = MenuWidget_CreateSimpleNode(0x29, temp_s2, 0, 1);
                            temp_s2_2 = MenuWidget_CreateNode(0x29, temp_v0_3, temp_v0_3);
                            temp_v0_3->draw = (void (*)())&Menu_DrawItemLabel;
                            temp_v0_3->update = (void (*)())&Menu_ConfirmDialogHandler;
                            temp_s2_2->draw = (void (*)())&Menu_DrawActionOptionList;
                            g_MenuActionTextBase = 0x6C;
                            MenuWidget_SetCurrentNode(temp_s2_2);
                            temp_a0 = g_MenuActiveItemList;
                            temp_s2_2->cursor_x = 1;
                            Inv_SelectActiveList(temp_a0);
                            g_MenuItemLabelBuffer = 0xFF;
                            Util_AppendFFTerminatedBytes(&g_MenuItemLabelBuffer, Str_LookupTable4(0x44));
                            g_MenuItemPrimListHandles[0] = 0;
                            {
                                MenuConfirmCallback callback = Menu_TriggerSaveWrite;
                                if (Draw_MeasureTextWidth(&g_MenuItemLabelBuffer) < 0x78) {
                                    var_v1 = 0x78;
                                } else {
                                    var_v1 = Draw_MeasureTextWidth(&g_MenuItemLabelBuffer);
                                }
                                temp_v0_3->grid_width = (var_v1 + 0x14);
                                temp_v0_3->visible_rows = 0x32;
                                temp_v0_3->x = ((0x12C - var_v1) >> 1);
                                temp_s2_2->x = ((temp_v0_3->grid_width - 0x80) >> 1);
                                temp_height = temp_v0_3->visible_rows;
                                g_MenuConfirmCallback = callback;
                                temp_s2_2->y = (temp_height - 0x14);
                            }
                        } else {
                            if (func_80042964(g_SaveSelectedSlot) < 0xF) {
                                MenuWidget_NavScrollTo(0x1F);
                                Menu_CreateMemCardProgressWidget(0x45);
                                var_a0 = &func_800504F4;
                                func_80042B50(var_a0);
                            } else {
                                temp_s0 = g_SaveSelectedSlot;
                                if (MenuWidget_FindByModeAndSelectedBase(1, 0x28) == 0) {
                                    Menu_CreateTwoLineDialog(temp_s0 + 0x47, g_McDialogMode + 0x42);
                                }
                            }
                        }
                    } else {
                        MenuWidget_NavScrollTo(0x1F);
                        Menu_CreateMemCardProgressWidget(0x46);
                        var_a0 = &func_8005051C;
                        func_80042B50(var_a0);
                    }
                    Menu_PlayConfirmSound();
                    return 1;
                }
            }
        }
        Menu_PlayErrorSound();
        /* Duplicate return node #20. Try simplifying control flow for better match */
        return 1;
    }
    return 1;
}
