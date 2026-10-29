/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/akao.h"
#include "pe1/akao/commands.h"
#include "pe1/akao_counters.h"
#include "pe1/akao_send.h"
#include "pe1/aya.h"
#include "pe1/battle_modifiers.h"
#include "pe1/draw_state.h"
#include "pe1/game_state.h"
#include "pe1/game_timers.h"
#include "pe1/inventory.h"
#include "pe1/inventory_category.h"
#include "pe1/inventory_queries.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_sort.h"
#include "pe1/inventory_state.h"
#include "pe1/menu_context_help.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_state.h"
#include "pe1/random.h"
#include "pe1/save.h"
#include "pe1/script_queue.h"
#include "pe1/text.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
s32 g_ItemDropRngIndex;
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPackedListCursor;
s32 g_InvPendingCompactCount;
int g_SaveMetadataWindowIndex;
s16 *g_InvActiveListOverride;
s32 g_InvOverrideSlotLimit;
int g_AkaoPendingBgmVolumeFade;
int g_MemCardDialogState;
unsigned int g_MenuActionSubmenuSel;
unsigned int g_MenuErrorSoundPending;
SavedMenuMode g_SavedMenuMode;

/* Retail passes the window index although the reset ignores it. */
void Save_ResetMetadataWindow(int);
void Menu_ResetStateFlags(void);
void Inv_SetDefaultItemId(void);
void MenuWidget_InitPoolUnk(void);
void Menu_InitStateBuffer(void);
void MemCard_ClearDelayedCallback(void);
void BattleCmd_InitTableCursor(void);
void Inv_BuildItemGridFromCategory(void);
void Menu_SetEquipSlotIndex(int index);

/* Item-drop random table: seeding from the play clock and the per-draw
 * lagged-xor refresh of the 0x209-byte table. */

int g_ItemDropRngIndex;

#define SCRAMBLE_RANDOM_BYTES()                    \
    {                                           \
        int scramble_i_;                           \
        for (scramble_i_ = 0; scramble_i_ < 0x20; scramble_i_++) { \
            g_ItemDropRngTable[scramble_i_] ^= g_ItemDropRngXorKeyLow[scramble_i_]; \
        }                                          \
        for (scramble_i_ = 0x20; scramble_i_ < 0x209; scramble_i_++) { \
            g_ItemDropRngTable[scramble_i_] ^= g_ItemDropRngXorKeyHigh[scramble_i_]; \
        }                                          \
    }


/* Text byte-string helpers, the item grid rebuild and the active inventory
 * list selection (Aya's list or a storage override) with its slot limit. */

s16 *g_InvItemPtr;
s16 *g_InvActiveListOverride;
int g_InvSlotLimit;
int g_InvOverrideSlotLimit;
u32 *g_InvSelectionBits;
int g_InvSelectionBitWords;

#define NULL ((void *)0)

void *Inv_LookupActiveListDisplayData(s32 index);
#undef NULL


int g_InvSlotLimit;
s16 *g_InvItemPtr;

#define NULL ((void *)0)

#undef NULL


/* Insert an item into a free active slot. Non-zero means the insert failed. */
#define NULL ((void *)0)


int g_InvCategoryBaseItemId;
s16 *g_InvItemPtr;
int g_InvSlotLimit;

/* Contiguous inventory counts, removal-by-ID, active-item kind lookup, base
 * item kinds, the equipped-slot reservation test and the packed-list
 * accessors. */
s16 *g_InvItemPtr;
int g_InvSlotLimit;
u32 *g_InvSelectionBits;
int g_InvSelectionBitWords;

/* The equipped-slot bytes live at 0x800C0E20/0x800C0E22, outside the gp
 * window; incomplete array types keep -G8 from making them small data. */

int g_InvPackedListCount;

int g_InvPackedListCursor;
void Inv_SetDefaultItemId(void);
void Inv_BuildItemGridFromCategory(void);



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
    Menu_InitStateBuffer();
    MenuInput_InitQueue();
    MemCard_ClearDelayedCallback();
    BattleCmd_InitTableCursor();
    Inv_BuildItemGridFromCategory();
    Save_SelectMetadataWindow(0);
    Save_InitMetadataState();
    BattleCmd_LoadWeaponModifiers();
    Menu_SaveBgInitFade();
}