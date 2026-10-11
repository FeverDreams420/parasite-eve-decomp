/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "common.h"
#include "pe1/draw_state.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_dialog.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/text.h"

/* Equip item selection view, its stat delta preview, grid scroll and slot
 * step, and the skill screen input handler it installs. */

#define NULL ((void *)0)

#include "Menu_EquipItemSelectionFlow_externs.h"
int MenuWidget_IsCursorYClear(MenuWidgetNode *ptr);
void Menu_InitCategoryTabs(void);
void Menu_OpenItemList(void);
int Battle_IsInputAllowedWrapped(void);

void Menu_CreateEquipItemSelectionView(MenuWidgetNode *parent)
{
    MenuWidgetNode *root;
    MenuWidgetNode *node;

    root = MenuWidget_CreateSimpleNode(5, parent, 0, 0);
    node = MenuWidget_CreateNode(5, root, root);
    root->draw = Menu_StepEquipSlotSelect;
    root->update = (void (*)())Menu_StepSkillScreen;
    node->draw = Menu_DrawSoundTestList;
    if (g_MenuLayoutLocked != 0) {
        MenuWidget_ClearColumnLayout(node);
    } else if (g_InvItemUsableFlag == 0) {
        MenuWidget_SetColumnLayout(node, 0x12);
    }
    if (node->cursor_x >= 0) {
        MenuWidget_SetCurrentNode(node);
    }
    root->disabled = 1;
    node = MenuWidget_CreateNode(0x1B, root, root);
    node->draw = (void (*)())Menu_DrawItemListInvPanel;
    MenuWidget_ClearColumnLayout(node);
}

void Menu_DrawEquipStatsDelta(ItemDataRecord *preview) {
    int sprite_base;
    int value;
    MenuWidgetNode *node;

    if (preview == 0) {
        return;
    }

    sprite_base = 0x7F;
    if (D_8009CF18 != 0) {
        sprite_base = 0x7C;
    }

    Draw_OffsetCursor(4, 0x1C);
    Draw_AllocSprite(sprite_base);
    Draw_OffsetCursor(0, 0xE);
    Draw_AllocSprite(sprite_base + 1);
    Draw_OffsetCursor(0, 0xE);
    Draw_AllocSprite(sprite_base + 2);

    if (D_8009CF1C != 0) {
        node = MenuWidget_GetCurrentNode();
        if (node->selected_base == 7) {
            Draw_OffsetCursor(0x18, 0xE);
            return;
        }
    }

    Draw_OffsetCursor(0x1E, -0x1C);
    value = preview->baseStats[0] + preview->bonusStats[0];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
    value = preview->baseStats[1] + preview->bonusStats[1];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
    value = preview->baseStats[2] + preview->bonusStats[2];
    if (value >= 0x3E8) {
        value = 0x3E7;
    }
    Draw_PrintNumberWidth4(value);

    Draw_OffsetCursor(-0x24, 0xE);
}

/* The retail display clamps only the upper bound; negative totals survive. */
static inline int clampedStatTotal(ItemDataRecord *item, int index)
{
    int value = item->baseStats[index] + item->bonusStats[index];
    if (value > 999)
        value = 999;
    return value;
}

void Menu_StepItemGridScroll(ItemDataRecord *item, ItemDataRecord *other)
{
    int sprite;
    if (!other)
        return;
    if (item) {
        if (D_8009CF1C) {
            Draw_OffsetCursor(40, -50);
            Draw_AllocSprite(0x88);
            Draw_OffsetCursor(0, 10);
            Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
            Draw_OffsetCursor(-20, 14);
            Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
            Draw_OffsetCursor(-20, 14);
            Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
            Draw_OffsetCursor(-38, 12);
        }
        Draw_OffsetCursor(38, -12);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -14);
        Draw_AllocSprite(0x22);
        Draw_OffsetCursor(0, -14);
        Draw_AllocSprite(0x22);
        if (D_8009CF1C) {
            Draw_OffsetCursor(12, -10);
            Draw_AllocSprite(0x88);
            Draw_SetStatCompareColor(item->bonusStats[0], other->bonusStats[0]);
            Draw_OffsetCursor(0, 10);
            Draw_PrintSignedNumberWidth4(other->bonusStats[0]);
            Draw_OffsetCursor(-20, 14);
            Draw_SetStatCompareColor(item->bonusStats[1], other->bonusStats[1]);
            Draw_PrintSignedNumberWidth4(other->bonusStats[1]);
            Draw_OffsetCursor(-20, 14);
            Draw_SetStatCompareColor(item->bonusStats[2], other->bonusStats[2]);
            Draw_PrintSignedNumberWidth4(other->bonusStats[2]);
        } else {
            Draw_SetStatCompareColor(clampedStatTotal(item, 0), clampedStatTotal(other, 0));
            Draw_OffsetCursor(12, -2);
            Draw_PrintNumberWidth4(clampedStatTotal(other, 0));
            Draw_OffsetCursor(-36, 14);
            Draw_SetStatCompareColor(clampedStatTotal(item, 1), clampedStatTotal(other, 1));
            Draw_PrintNumberWidth4(clampedStatTotal(other, 1));
            Draw_OffsetCursor(-36, 14);
            Draw_SetStatCompareColor(clampedStatTotal(item, 2), clampedStatTotal(other, 2));
            Draw_PrintNumberWidth4(clampedStatTotal(other, 2));
        }
    } else {
        sprite = D_8009CF18 ? 0x7C : 0x7F;
        Draw_OffsetCursor(4, 23);
        Draw_AllocSprite(sprite);
        Draw_OffsetCursor(0, 14);
        Draw_AllocSprite(sprite + 1);
        Draw_OffsetCursor(0, 14);
        Draw_AllocSprite(sprite + 2);
        Draw_SetColor(0x8080);
        Draw_OffsetCursor(40, -28);
        Draw_SetStatCompareColor(0, other->baseStats[0] + other->bonusStats[0]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 0));
        Draw_OffsetCursor(-36, 14);
        Draw_SetStatCompareColor(0, other->baseStats[1] + other->bonusStats[1]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 1));
        Draw_OffsetCursor(-36, 14);
        Draw_SetStatCompareColor(0, other->baseStats[2] + other->bonusStats[2]);
        Draw_PrintNumberWidth4(clampedStatTotal(other, 2));
    }
    Draw_SetColor(0x808080);
}

void Menu_StepEquipSlotSelect(void)
{
    MenuWidgetNode *node, *current;
    ItemDataRecord *item, *other;
    int selection;
    node = MenuWidget_FindByModeAndSelectedBase(2, 13);
    if (node) {
        if (node->cursor_x >= 0)
            Inv_RememberSelection(0, Inv_GetPackedListItem(MenuWidget_GridCellIndex(node)));
    } else if (g_MenuItemUseMode) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 16);
        if (node && node->cursor_x >= 0)
            Inv_RememberSelection(0, Inv_GetPackedListItem(MenuWidget_GridCellIndex(node)));
    } else if (!D_8009CF1C) {
        Inv_RememberSelection(0, D_8009CF18 ? D_800C0E20.tracked[0] : D_800C0E20.tracked[2]);
    }
    if (D_8009CF1C)
        selection = Inv_RestoreSelection(0);
    else
        selection = D_8009CF18 ? D_800C0E20.tracked[0] : D_800C0E20.tracked[2];
    item = Inv_LookupActiveListData(selection);
    current = MenuWidget_FindByModeAndSelectedBase(2, 7);
    if (current == MenuWidget_GetCurrentNode()) {
        other = Inv_LookupActiveListData(Inv_RestoreSelection(1));
        Menu_DrawEquipStatsDelta(D_8009CF1C ? other : item);
        Menu_StepItemGridScroll(item, other);
    } else {
        Menu_DrawEquipStatsDelta(item);
        if (item) {
            Draw_OffsetCursor(42, -12);
            Draw_PrintNumberWidth4Unk(item->baseStats[2]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[2]);
            Draw_OffsetCursor(-45, -14);
            Draw_PrintNumberWidth4Unk(item->baseStats[1]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[1]);
            Draw_OffsetCursor(-45, -14);
            Draw_PrintNumberWidth4Unk(item->baseStats[0]);
            Draw_OffsetCursor(5, 0);
            Draw_PrintSignedNumberWidth4(item->bonusStats[0]);
            Draw_OffsetCursor(-45, -10);
            Draw_AllocSprite(0x87);
            Draw_OffsetCursor(25, 0);
            Draw_AllocSprite(0x88);
        }
    }
}

s32 Menu_StepSkillScreen(MenuWidgetNode *arg0, s32 arg1) {
    MenuWidgetNode *parg0;
    s32 parg1;
    s32 tv4k;
    MenuWidgetNode *temp_v0;
    register MenuWidgetNode *node asm("$16");
    register MenuWidgetNode *temp_v0_3 asm("$18");
    register s32 cm1 asm("$17");

    parg0 = arg0;
    parg1 = arg1;
    node = MenuWidget_GetChild(arg0, NULL);
    if (parg1 & 0x10000) {
        if (Inv_GetPackedListCount() != 0) {
            if (g_MenuLayoutLocked != 0) {
                Menu_OpenItemList();
                return 1;
            }
            if (Battle_IsInputAllowedWrapped() != 0) {
                temp_v0 = MenuWidget_CreateSimpleNode(0x35, 0, 0, 0);
                (MenuWidget_CreateNode(0x35, temp_v0, temp_v0))->draw = Menu_SetupSkillSubmenu;
                node = MenuWidget_FindByModeAndSelectedBase(2, 7);
                node->cursor_x = 0;
                MenuWidget_SetCurrentNode(node);
                Menu_PlayConfirmSound();
                return 1;
            }
        }
        Menu_PlayErrorSound();
        return 1;
    }
    if (parg1 & 0x40) {
        if (g_MenuLayoutLocked != 0) {
            Menu_InitCategoryTabs();
            return 1;
        }
        MenuWidget_NavScrollTo(7);
        MenuWidget_DestroyNode(parg0);
        temp_v0_3 = MenuWidget_FindByModeAndSelectedBase(1, 6);
        if (temp_v0_3 != NULL) {
            MenuWidget_DestroyNode(temp_v0_3);
            Menu_CreateBonusPointAllocationView();
            Menu_PlayCancelSound();
            return 1;
        }
        /* Duplicate return node #20. Try simplifying control flow for better match */
        return 1;
    }
    tv4k = parg1 & 0x4000;
    if (tv4k && (MenuWidget_IsCursorYClear(MenuWidget_FindByModeAndSelectedBase(1, 6)) != 0)) {
        cm1 = -1;
        node->cursor_x = cm1;
        node = MenuWidget_GetChild(parg0, 1);
        if (node != NULL) {
            node->cursor_x = cm1;
        }
        node = MenuWidget_FindByModeAndSelectedBase(2, 6);
        if (node != NULL) {
            node->cursor_y = 0;
            node->cursor_x = 0;
            MenuWidget_SetCurrentNode(node);
        }
        Menu_PlayMoveSound();
    }
    return 1;
}
