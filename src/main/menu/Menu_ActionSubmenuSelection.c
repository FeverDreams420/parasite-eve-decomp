/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */
#include "pe1/menu_state.h"
#include "pe1/game_state.h"
#include "pe1/inventory_slots.h"
#include "pe1/menu_queue.h"
#include "pe1/akao/commands.h"
#include "pe1/akao_counters.h"
#include "pe1/save.h"
#include "pe1/menu_inventory.h"
#include "pe1/battle_modifiers.h"

/* Retail passes the window index although the reset ignores it. */
void Save_ResetMetadataWindow(int);
void Menu_ResetStateFlags(void);
void Inv_SetDefaultItemId(void);
void Draw_InitBuffers(void);
void MenuWidget_InitPoolUnk(void);
void Sys_InitStateBuffer(void);
void MemCard_ClearDelayedCallback(void);
void BattleCmd_InitTableCursor(void);
void Inv_BuildItemGridFromCategory(void);
void Menu_SetEquipSlotIndex(int index);

void Akao_FlushBgmVolumeFade(void) {
    if (g_AkaoPendingBgmVolumeFade != 0) {
        Akao_Cmd_21(g_AkaoPendingBgmVolumeFade, 0);
        g_AkaoPendingBgmVolumeFade = 0;
    }
}

void Menu_SetActionSubmenuSelection(int value) {
    g_MenuActionSubmenuSel = value;
    Akao_Cmd_80_81_82(value == 0);
}

unsigned int Menu_GetActionSubmenuSelection(void) {
    return g_MenuActionSubmenuSel;
}

void BoundsCheck_AssertStub(int code)
{
}

void Menu_FullInit(void) {
    Menu_SetBattleEquipMode(0);
    Save_ResetMetadataWindow(0);
    g_SavedMenuMode.value = 0;
    g_MemCardDialogState = 0;
    g_MenuErrorSoundPending = 0;
    Menu_ResetStateFlags();
    Inv_SetDefaultItemId();
    Draw_InitBuffers();
    MenuWidget_InitPoolUnk();
    Sys_InitStateBuffer();
    Queue_Init();
    MemCard_ClearDelayedCallback();
    BattleCmd_InitTableCursor();
    Inv_BuildItemGridFromCategory();
    Save_SelectMetadataWindow(0);
    Save_InitMetadataState();
    BattleCmd_LoadWeaponModifiers();
    Menu_SaveBgInitFade();
    Save_ResetMetadataWindow(1);
    g_GameState.flags |= 0x40000000;
    Menu_SetEquipSlotIndex(1);
}
