/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/battle_cmd.h"

#define CLOSE_SELECT_VIEWS() do { \
    MenuWidget_NavScrollTo(0x2d); \
    MenuWidget_NavScrollTo(0x18); \
    MenuWidget_NavScrollTo(0x12); \
} while (0)

/* Matching debt: the three inventory-root arguments retain retail registers.
 * Empty barriers preserve the return paths and call delay slots. */
int Menu_StepItemSelectScreen(MenuWidgetNode *node, unsigned int buttons)
{
    int index = 0;
    int mask;
    int action;
    MenuWidgetNode *child;
    int confirm = buttons & 0x10000;
    if (confirm) {
        child = MenuWidget_GetChild(node, 0);
        index = MenuWidget_GridCellIndex(child);
        if (Menu_GetBattleEquipMode()) {
            mask = D_8009CEF0 & 0x1f;
        } else {
            mask = D_8009CEF0 & 0x1ef;
        }
        action = -1;
        goto check;
        while (1) {
            if (index < 0) break;
            index -= mask & 1;
            mask >>= 1;
            ++action;
        check:
            if (action >= 9) break;
        }
        switch (action) {
        case 0:
            CLOSE_SELECT_VIEWS();
            Menu_CreateEquipScreen(child);
            break;
        case 1:
            CLOSE_SELECT_VIEWS();
            Menu_CreateSkillActionScreen(child);
            break;
        case 2:
            CLOSE_SELECT_VIEWS();
            D_8009CF18 = 1;
            Menu_CreateInventoryListView(child);
            Menu_CreateEquipItemSelectionView(child);
            Menu_StepSkillList(MenuWidget_FindByModeAndSelectedBase(2, 5), 0);
            break;
        case 3:
            CLOSE_SELECT_VIEWS();
            D_8009CF18 = 0;
            Menu_CreateInventoryListView(child);
            Menu_CreateEquipItemSelectionView(child);
            /* An int-returning call keeps cross-jumping from merging this
             * case into case 2, which retail keeps separate. */
            ((int (*)(MenuWidgetNode *, int))Menu_StepSkillList)(MenuWidget_FindByModeAndSelectedBase(2, 5), 0);
            break;
        case 4:
            if (Battle_IsActiveWrapped()) {
                CLOSE_SELECT_VIEWS();
                Inv_SetActiveList(8, 0);
            } else {
                Menu_PlayErrorSound();
                index = 1;
                asm volatile("" : "=r"(index) : "0"(index));
                return index;
            }
            break;
        case 5:
            CLOSE_SELECT_VIEWS();
            Menu_CreateMainMenuView(child);
            break;
        case 6:
            CLOSE_SELECT_VIEWS();
            Menu_CreateInvSwapView(child, 0);
            break;
        case 7:
            CLOSE_SELECT_VIEWS();
            D_8009CF1C = 1;
            Inv_SelectActiveList(0);
            g_StatBaseTable[0] = Inv_CountByCategory(0xe) ? 999 : Inv_CountByCategory(0xc);
            g_StatBaseTable[1] = Inv_CountByCategory(0xf) ? 999 : Inv_CountByCategory(0xd);
            if (D_8009CF0C) {
                g_StatBaseTable[2] = WayneStorage_CountItemType(0xe) ? 999 : WayneStorage_CountItemType(0xc);
                g_StatBaseTable[3] = WayneStorage_CountItemType(0xf) ? 999 : WayneStorage_CountItemType(0xd);
            } else {
                g_StatBaseTable[3] = 0;
                g_StatBaseTable[2] = 0;
            }
            Menu_StepInventoryRoot(0x33e, -1, -1);
            D_8009CEF8 = 0;
            break;
        case 8: {
            int command;
            register int first asm("$4");
            register int second asm("$5");
            register int third asm("$6");
            CLOSE_SELECT_VIEWS();
            first = 0x37e;
            second = -1;
            third = -1;
            command = 1;
            asm volatile("" : "=r"(command) : "0"(command), "r"(first), "r"(second), "r"(third));
            D_8009CF30 = command;
            Menu_StepInventoryRoot(first, second, third);
            D_8009CEF8 = 0;
            break;
        }
        default:
            goto return_one;
        }
        Menu_PlayConfirmSound();
        index = 1;
        asm volatile("" : "=r"(index) : "0"(index));
        return index;
    }
    if (buttons & 0x40) {
        MenuWidget_InitPool();
        Inv_SetActiveList(9, 0);
        if (!Menu_GetBattleEquipMode()) BattleCmd_ResetTableCursor();
        Menu_PlayCancelSound();
    return_one:
        index = 1;
        asm volatile("" : "=r"(index) : "0"(index));
        return index;
    }
    {
        int down = buttons & 2;
        asm volatile("" : "=r"(index) : "0"(index));
        if (down) Menu_InitScrollDirection();
    }
    return index;
}
