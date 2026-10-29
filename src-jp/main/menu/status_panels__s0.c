/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/battle_cmd.h"
#include "pe1/battle_runtime.h"
#include "pe1/draw_level_bar.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/inventory.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/memcard.h"
#include "pe1/menu_background.h"
#include "pe1/menu_bonus_stats.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_equipment.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_inventory_root.h"
#include "pe1/menu_item_list_input.h"
#include "pe1/menu_item_rows.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_gpu.h"
#include "pe1/render_buffers.h"
#include "pe1/resident_tables.h"
#include "pe1/save.h"
#include "pe1/stat_modifiers.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_BonusPointDisplayValue;
int g_BonusPointBarAnimStep;
int g_BonusPointBarAnimProgress;
int g_InvItemUsableFlag;
MenuWidgetNode *g_MenuActiveWidget;
int g_MenuBattleStatusOverlayActive;
int g_MenuEquipMode;
int g_MenuItemUseMode;
int g_MenuItemRenameMode;
int g_MenuItemDiscardMode;
int g_InvSelectedItemIndex;
int g_MenuSelectionLocked;
int g_MenuLayoutLocked;
int g_MenuSaveBgFadeIndex;
int g_MenuSaveBgFadeLutLen;
int g_MenuSaveBgFadeTint;

/* Save-screen background fade: gamma curve, per-frame darkening of the
 * captured frame and the fade state machine setters. Contiguous at
 * 0x80042C78 and joined by the fade state words. MemCard_InitSlotState
 * (historical name) arms the fade-in. */

int g_MenuSaveBgFadeState;
int g_MenuSaveBgFadeStep;
int g_MenuSaveBgFadeHeight;

void Menu_SaveBgInitFade(void) {
    g_MenuSaveBgFadeState = 0;
    g_MenuSaveBgFadeLutLen = 0;
    g_MenuSaveBgFadeStep = 0;
    g_MenuSaveBgFadeHeight = 0x20;
    Menu_ComputeGammaLut(0x90, 0xFF);
    g_MenuSaveBgFadeTint = 0x48;
}

void Menu_SaveBgSetFadeTarget(int value) {
    g_MenuSaveBgFadeTint = value;
}

void Menu_ComputeGammaLut(int arg0, int arg1) {
    u8 *base;
    u8 *ptr;
    int scale;
    int limit;
    u8 *iter_end;
    u8 *check_end;

    base = g_MenuSaveBgFadeLut;
    base[0] = 0;
    ptr = base;
    limit = 0x100;
    scale = limit - arg0;
    check_end = ptr + 0xF;
    arg0 <<= 8;
    if (ptr < check_end) {
        iter_end = check_end;
        do {
            if (*ptr >= arg1) {
                base = g_MenuSaveBgFadeLut;
                limit = (ptr - base) + 1;
                g_MenuSaveBgFadeLutLen = limit;
                return;
            }
            ptr[1] = (arg0 + (scale * *ptr)) >> 8;
            ptr++;
        } while (ptr < iter_end);
    }

out:
    base = g_MenuSaveBgFadeLut;
    limit = (ptr - base) + 1;
    g_MenuSaveBgFadeLutLen = limit;
}

void Menu_SaveBgApplyFadeStep(void) {
    int pos;
    int level;
    int bias;
    int attenuation;
    u16 *src;
    u16 *dst;
    int i;

    pos = g_MenuSaveBgFadeIndex + g_MenuSaveBgFadeStep;
    g_MenuSaveBgFadeIndex = pos;
    if (pos < 0) {
        g_MenuSaveBgFadeIndex = 0;
        g_MenuSaveBgFadeStep = 0;
        g_MenuSaveBgFadeState = 0;
    } else if (pos >= g_MenuSaveBgFadeLutLen) {
        g_MenuSaveBgFadeIndex = g_MenuSaveBgFadeLutLen - 1;
        g_MenuSaveBgFadeStep = 0;
        g_MenuSaveBgFadeState = 7;
    }

    level = g_MenuSaveBgFadeLut[g_MenuSaveBgFadeIndex];
    {
        int product = level * g_MenuSaveBgFadeTint;
        bias = product << 5;
    }
    attenuation = level * (g_MenuSaveBgFadeTint + 0x100);
    src = dst = g_GameState.save_background_source;
    dst = g_GameState.save_background_destination;

    for (i = 0; i < (g_MenuSaveBgFadeHeight << 8); i++) {
        int pixel = *src++;
        int color;

        if (pixel != 0) {
            int r = pixel & 0x1F;
            int g = (pixel >> 5) & 0x1F;
            int b = (pixel >> 10) & 0x1F;
            color = pixel & 0x8000;

            {
                int mixed = r << 16;
                mixed = bias + mixed - attenuation * r;
                r = mixed >> 16;
            }
            g = (bias + (g << 16) - (attenuation * g)) >> 16;
            b = (bias + (b << 16) - (attenuation * b)) >> 16;
            color |= r;
            *dst = color | (g << 5) | (b << 10);
        } else {
            *dst = 0;
        }
        dst++;
    }

    {
        unsigned shade = (unsigned)bias >> 13;
        g_RenderDrawEnvArray[1].b0 = shade;
        g_RenderDrawEnvArray[1].g0 = shade;
        g_RenderDrawEnvArray[1].r0 = shade;
        g_RenderDrawEnvArray[0].b0 = shade;
        g_RenderDrawEnvArray[0].g0 = shade;
        g_RenderDrawEnvArray[0].r0 = shade;
    }
}

int Menu_SaveBgIsFadeActive(void) {
    return g_MenuSaveBgFadeState != 0;
}

void MemCard_InitSlotState(void) {
    int value = g_GeomOtZ[0];

    g_MenuSaveBgFadeState = 1;
    g_MenuSaveBgFadeStep = 1;
    g_MenuSaveBgFadeIndex = 0;
    g_MenuSaveBgFadeHeight = value;

    /* value is a zero-extended byte, so the < 0 arm is dead at runtime,
     * but it is what retail compiled (bgez). */
    if (value < 0) {
        g_MenuSaveBgFadeHeight = 1;
    } else if (value >= 0x21) {
        g_MenuSaveBgFadeHeight = 0x20;
    }
}

void Menu_SaveBgStartFadeOut(void) {
    g_MenuSaveBgFadeState = 5;
    g_MenuSaveBgFadeStep = -1;
}

void Menu_SaveBgClearFade(void) {
    g_MenuSaveBgFadeState = 0;
}

void Menu_SaveBgAdvanceFade(void) {
    RECT rect;
    int next;

    switch (g_MenuSaveBgFadeState) {
    case 0:
        return;

    case 1:
        next = 2;
        break;

    case 2:
        next = 3;
        break;

    case 3:
        rect.y = 0x1E0;
        rect.x = 0;
        rect.w = 0x100;
        rect.h = g_MenuSaveBgFadeHeight;
        StoreImage(&rect, (void *)g_RenderFrontBufferBase[0]);
        next = 4;
        break;

    case 5:
        g_MenuSaveBgFadeState = 6;
        Menu_SaveBgApplyFadeStep();
        return;

    case 4:
    case 6:
        next = 5;
        break;

    case 7:
        return;

    default:
        return;
    }

    g_MenuSaveBgFadeState = next;
}

void Menu_DrawSaveBg(void) {
    RECT rect;

    if (g_MenuSaveBgFadeState == 6) {
        rect.y = 0x1E0;
        rect.h = g_MenuSaveBgFadeHeight;
        rect.x = 0;
        rect.w = 0x100;
        LoadImage(&rect, (u_long *)g_RenderBackBufferBase[0]);
    }
}

int g_MenuSaveBgFadeState;

int Menu_SaveBgIsFadeComplete(void) {
    return g_MenuSaveBgFadeState == 7;
}

void Inv_FetchSelectedInventoryItem(void) {
    int node;

    node = MenuWidget_FindByModeAndSelectedBase(2, 7);
    if (node == 0) {
        node = MenuWidget_FindByModeAndSelectedBase(2, 0xD);
        if (node == 0) {
            node = MenuWidget_FindByModeAndSelectedBase(2, 0x10);
        }
    }

    Inv_GetPackedListItem(MenuWidget_GridCellIndex(node));
}

void Menu_HandleInput(int arg0) {
    register int value;
    int flag;
    int blocked;

    if (arg0 >= Inv_GetPackedListCount()) {
        return;
    }

    if (g_MenuItemRenameMode != 0 || g_MenuItemDiscardMode != 0) {
        if (Inv_TestSelectionBit(arg0) == 0) {
            Draw_SetTextDimmed(1);
        }
    }

    value = Inv_GetPackedListItem(arg0);
    blocked = Inv_IsAyaInventorySlotReserved(value);

    if (blocked != 0) {
        if (g_MenuActiveWidget->selected_base != 0xD) {
            if (g_MenuLayoutLocked == 0 && g_MenuItemUseMode == 0) {
                Draw_SetTextDimmed(1);
            }
        }
    }

    if (g_MenuLayoutLocked != 0) {
    flag = 0;
    if (g_StatBaseTable[0] + g_StatBaseTable[1] + g_StatBaseTable[2] + g_StatBaseTable[3] != 0) {
        if ((Inv_LookupActiveListData(value)->flags & 0x40) == 0) {
            int mask;

            if (Inv_GetActiveListItemType(value) >= 6) {
                mask = 1 << Inv_GetActiveListItemType(value);
            } else {
                mask = 0x3E;
            }

            if (Inv_SetupSlotDisplay(mask) != 0) {
                flag = 1;
            }
        }
    }

    if (flag == 0) {
        Draw_SetTextDimmed(1);
    }
    }

    Menu_DrawActiveListSlot(value);
    if (blocked != 0) {
        Draw_DropShadow();
    }
}

int g_MenuItemSelectFlag;

void Menu_SetItemContext(int arg0) {
    g_MenuItemSelectFlag = arg0;
}

int Menu_CheckItemAffordable(int actionId)
{
    int total, remaining, enabled, allowed, category;
    int action = actionId;
    int bits;
    ItemDataRecord *item;
    remaining = BattleCmd_GetRemainingAmmo(&total);
    item = Item_LookupBaseData(action + 235);
    bits = item->flags >> Menu_GetBattleEquipMode();
    if (action == 5) {
        if (remaining >= total / 3) enabled = bits & 1;
        else enabled = 0;
    } else if (action != 6) {
        if (action != 19) {
            if (remaining >= Inv_GetSlotHighlightState(action, 0)) enabled = bits & 1;
            else enabled = 0;
        } else {
            if (remaining >= total) enabled = bits & 1;
            else enabled = 0;
        }
    } else {
        if (remaining > 0) enabled = bits & 1;
        else enabled = 0;
    }
    /* These actions are unavailable while the global restriction is active. */
    allowed = 0;
    if (!g_MenuItemSelectFlag || ((unsigned)(action - 6) >= 3 && action != 10 && action != 19)) {
        allowed = 1;
    }
    enabled &= allowed;
    if (action == 5 && Battle_GetEnemyEscapeFlag()) enabled = 0;
    if (action == 6) {
        ItemDataRecord *weapon;
        weapon = Inv_LookupActiveListData(g_InvTrackedSlots[0]);
        category = weapon->kind;
        if (category == 8) enabled = 0;
        else {
            if (category && (unsigned)category < 8) {
                category -= 4;
                if (category <= 0) category = 1;
            } else {
                category = weapon->kind < 19 ? 0 : weapon->kind - 18;
            }
            if (category == 1 || category == 3) {
                int ammo = Inv_GetWeaponCategoryAmmoBase(category - 1);
                enabled &= weapon->ammo + ammo > 0 ? 1 : 0;
            } else enabled = 0;
        }
    }
    if (action < 3 || action == 18) {
        if (g_AyaHpCurrent[0] >= g_AyaHpMax[0]) enabled = 0;
    }
    return enabled;
}

int Save_ClassifyPlaytime(int playTime) {
    int result;

    if (playTime >= 0x80) {
        if (playTime >= 0x138) {
            if (playTime >= 0x1B0) {
                if (playTime >= 0x218) {
                    if (playTime < 0x2D3) {
                        result = 5;
                    } else {
                        result = 6;
                    }
                } else {
                    result = 4;
                }
            } else {
                result = 3;
            }
        } else {
            result = 2;
        }
    } else {
        result = 1;
    }

    return result;
}

#define NULL ((void *)0)


int g_MenuItemContextFlag;

void Menu_DrawStatusPanel(void);
void Menu_DrawStatsList(void);

#define CLOSE_SELECT_VIEWS() { \
    MenuWidget_NavScrollTo(0x2d); \
    MenuWidget_NavScrollTo(0x18); \
    MenuWidget_NavScrollTo(0x12); \
}
