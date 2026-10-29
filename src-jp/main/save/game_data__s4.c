/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/aya.h"
#include "pe1/draw_state.h"
#include "pe1/draw_wipe_rect.h"
#include "pe1/game_state.h"
#include "pe1/game_timers.h"
#include "pe1/inventory_category.h"
#include "pe1/inventory_slots.h"
#include "pe1/inventory_state.h"
#include "pe1/memcard.h"
#include "pe1/menu_background.h"
#include "pe1/menu_inventory.h"
#include "pe1/menu_queue.h"
#include "pe1/menu_sound.h"
#include "pe1/menu_state.h"
#include "pe1/menu_widget.h"
#include "pe1/psyq_bios.h"
#include "pe1/save.h"
#include "pe1/scene_dispatch.h"
#include "pe1/scene_room.h"
#include "pe1/text.h"
#include "include_asm.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
int g_InvCategoryBaseItemId;
s32 g_InvPackedListCount;
s16 *g_InvItemPtr;
s32 g_InvSlotLimit;
u32 *g_InvSelectionBits;
s32 g_InvSelectionBitWords;
s32 g_InvPackedListCursor;
s32 g_InvRebuildFilterMode;
s32 g_InvRebuildMinFirstStat;
int g_MenuBattleEquipMode;
u8 *g_SaveMetadataCursor;
ItemDataRecord *g_SaveMetadataSourceRecord;
int g_SaveMetadataWindowIndex;
int g_SaveMetadataWindowLength;
int g_MenuScrollDirectionValueUnwritten;
int g_MemCardDialogState;
int g_MenuScrollStep;

int g_SaveMetadataWindowIndex;

void Save_AppendMetadataChar(unsigned char ch) {
    unsigned char *cursor;
    int used;
    int length;
    unsigned int value;
    int has_room;
    int terminator;
    /* Never used: an unused local array is what gives this leaf function
     * its 8-byte stack frame, in retail as here. */
    unsigned char spare[4];

    length = g_SaveMetadataWindowLength;
    cursor = g_SaveMetadataCursor;
    used = 0;
    if (length > 0) {
        terminator = 0xFF;

        do {
            value = *cursor;
            if (value != terminator) {
                used += value < 0xFA;
                cursor++;
            } else {
                break;
            }
        } while (used < length);

        has_room = used < g_SaveMetadataWindowLength;
        if (has_room != 0) {
            cursor[0] = ch;
            cursor[1] = 0xFF;
            return;
        }
    }

    if (cursor[-2] >= 0xFA) {
        cursor--;
        cursor[0] = 0xFF;
    }

    cursor[-1] = ch;
}

int Save_DeleteMetadataChar(void) {
    unsigned char *cursor = g_SaveMetadataCursor;
    int used = 0;
    int removed;
    int length = g_SaveMetadataWindowLength;

    while (used < length && *cursor != 0xFF) {
        used += *cursor < 0xFA;
        cursor++;
    }

    removed = 0 < used;
    if (removed != 0) {
        cursor--;
        cursor[0] = 0xFF;
        cursor--;
        if (cursor >= g_SaveMetadataCursor && *cursor >= 0xFA) {
            cursor[0] = 0xFF;
        }
    }

    return removed;
}

void Save_LoadMetadataWindowText(void) {
    unsigned char *cursor;
    unsigned char *text;
    unsigned char *lookup;
    unsigned char *dst;
    unsigned char *out;
    unsigned char value;
    unsigned char loop_value;
    int loop_terminator;

    cursor = g_SaveMetadataCursor;
    if (cursor < g_SaveMetadataCursor + g_SaveMetadataWindowLength) {
        do {
            *cursor = 0xFF;
            cursor++;
        } while (cursor < g_SaveMetadataCursor + g_SaveMetadataWindowLength);
    }

    dst = g_SaveMetadataCursor;
    if (g_SaveMetadataSourceRecord != 0) {
        lookup = Text_LookupTable8(g_SaveMetadataSourceRecord->itemId - 1);
        out = dst;
    } else {
        lookup = Text_LookupTable4(0x1E);
        out = dst;
    }

    text = lookup;
    value = *text++;
    *out++ = value;
    if (value != 0xFF) {
        loop_terminator = 0xFF;
        do {
            loop_value = *text++;
            *out++ = loop_value;
        } while (loop_value != loop_terminator);
    }
}

unsigned char *Save_GetMetadataCursor(void) {
    return g_SaveMetadataCursor;
}

char *Save_GetActiveMetadataBuffer(void) {
    char *buffer;

    buffer = g_SaveRuntimeState.windows.metadata[0].text;

    if (g_SaveMetadataWindowIndex != 0) {
        buffer = g_SaveRuntimeState.windows.metadata[1].text;
    }

    return buffer;
}

unsigned int Save_GetMetadataRemainingChars(void) {
    unsigned int remaining = g_SaveMetadataWindowLength;
    unsigned char *cursor = g_SaveMetadataCursor;

    while (remaining != 0 && *cursor != 0xFF) {
        remaining -= *cursor < 0xFA;
        cursor++;
    }

    return remaining;
}

SavedMenuMode g_SavedMenuMode;

/* Equips the inventory item whose equipment record has this ID: weapons
 * (kinds 1..8) and armor (kind 9). Returns nonzero when nothing was equipped. */
int Inv_EquipItemById(unsigned int itemId) {
    int result = itemId;
    ItemDataRecord *record = g_InventoryRuntime.equipment;

    if (record < g_InventoryRuntime.equipment + 128) {
        while (1) {
            if (record->itemId == result) {
                break;
            }
            record++;
            if (record >= g_InventoryRuntime.equipment + 128) {
                break;
            }
        }
        if (record < g_InventoryRuntime.equipment + 128) {
            int value;
            int index;
            int count;
            s16 *item;
            s16 *end;
            g_InvItemPtr = g_InventoryRuntime.slots;
            count = Inv_GetAyaSlotLimit();
            g_InvSelectionBits = g_InvSelectionBitStorage;
            g_InvSelectionBitWords = 2;
            value = record - g_InventoryRuntime.equipment + 0x100;
            item = g_InvItemPtr;
            g_InvSlotLimit = count;
            end = item + count;
            if (item < end) {
                while (*item != value) {
                    item++;
                    if (item >= end) {
                        break;
                    }
                }
                if (item < g_InvItemPtr + g_InvSlotLimit) {
                    index = item - g_InvItemPtr;
                } else {
                    index = -1;
                }
            } else {
                index = -1;
            }
            result = index;
            if (result >= 0) {
                ItemDataRecord *entry;
                int kind;
                if (result < g_InvSlotLimit) {
                    entry = Inv_LookupItemRecordScaled(g_InvItemPtr[result]);
                } else {
                    entry = 0;
                }
                kind = 0;
                if (entry != 0) {
                    kind = entry->kind;
                }
                if (kind != 0) {
                    /* Retail tests kind < 9 first, then 9; the inner test is
                     * threaded into those two branches. */
                    if (kind < 9 || kind == 9) {
                        if (kind < 9) {
                            g_AyaEquippedWeaponSlot[0] = result;
                        } else {
                            g_AyaEquippedArmorSlot[0] = result;
                        }
                        return (unsigned int)result >> 31;
                    }
                    result = -1;
                } else {
                    result = -1;
                }
            }
        }
    }
    return (unsigned int)result >> 31;
}

void Menu_ResetInputState(void) {
    g_SavedMenuMode.value = Menu_GetActiveMode() & 0xFF;
    Battle_SetWindowBoundsByMode(0);
}

/* Open the player inventory using the same saved menu mode. */

void BattleCmd_SyncActiveAmmo(void);
void Menu_CreateInventoryTabView(void);

void Inv_OpenAyaItemList(unsigned int arg) {
    g_MenuBattleEquipMode = arg;
    g_SavedMenuMode.value = Menu_GetActiveMode() & 0xFF;
    Battle_SetWindowBoundsByMode(0);
    BattleCmd_SyncActiveAmmo();
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    Menu_CreateInventoryTabView();
    Menu_PlayConfirmSound();
}

/* Memory-card dialog gating from the inventory frontend. */

void MemCard_SetDialogActive(int active) {
    if (active == 0) {
        g_MemCardDialogState = 0;
        MemCard_CloseAll();
        g_GameState.flags &= 0xFFFF3FFF;
    } else if (g_MemCardDialogState == 0) {
        g_MemCardDialogState = 1;
        g_GameState.flags |= 0xC000;
    }
}

int Menu_GetItemContextFlag(void);
int Save_ClassifyPlaytime(int arg0);

void Save_BuildHeader(void) {
    u16 *dst;
    int i;
    int category;
    unsigned int value;

    i = 0;
    dst = g_SaveRuntimeState.categoryCounts;
    category = 0;
    while (i < 6) {
        *dst = g_InvCategoryItemTable[category].count;
        category++;
        i++;
        dst++;
    }

    g_SaveRuntimeState.drawBlendColor = Draw_GetBlendColor();
    g_SaveRuntimeState.menuClampValue = Menu_GetItemContextFlag();
    g_SaveRuntimeState.windows.header.playTimeMinutes =
        (unsigned int)GAME_TIME_COUNTER(2) / 60U;
    g_SaveRuntimeState.windows.header.gameTimeMinutes =
        (unsigned int)GAME_TIME_COUNTER(0) / 60U;
    g_SaveRuntimeState.windows.header.drawBaseY = Draw_GetBaseY();
    value = Menu_GetActionSubmenuSelection();
    g_SaveRuntimeState.windows.header.menuLayout =
        value | (MenuWidget_GetColumnLayoutMode() << 2);
    g_SaveRuntimeState.windows.header.reserved1E = 0;
    g_SaveRuntimeState.playTimeClass = Save_ClassifyPlaytime(g_PlayTimeFrameCounter);
    g_SaveRuntimeState.currentMapNumber = Save_GetCurrentMapNumber();
}

void Menu_CopyPromptCodes(u8 *src);

void Save_RestoreHeader(void) {
    u16 *src;
    int i;
    int category;
    u8 *prompt;

    i = 0;
    src = g_SaveRuntimeState.categoryCounts;
    category = 0;
    while (i < 6) {
        g_InvCategoryItemTable[category].count = *src++;
        i++;
        category++;
    }

    Draw_BlendColor(g_SaveRuntimeState.drawBlendColor);
    Menu_ClampRange(g_SaveRuntimeState.menuClampValue);

    GAME_TIME_COUNTER(2) = (g_SaveRuntimeState.windows.header.playTimeMinutes * 15) * 4;
    GAME_TIME_COUNTER(0) = (g_SaveRuntimeState.windows.header.gameTimeMinutes * 15) * 4;
    Draw_SetBaseOffsetPosition(0, g_SaveRuntimeState.windows.header.drawBaseY - Draw_GetBaseY());
    Menu_SetActionSubmenuSelection(g_SaveRuntimeState.windows.header.menuLayout & 3);
    MenuWidget_SetColumnLayoutMode((g_SaveRuntimeState.windows.header.menuLayout >> 2) & 1);

    prompt = g_SaveRuntimeState.windows.metadata[0].text;
    if (g_SaveMetadataWindowIndex != 0) {
        prompt = g_SaveRuntimeState.windows.metadata[1].text;
    }
    Menu_CopyPromptCodes(prompt);
}

u32 g_MenuErrorSoundPending;

/* Queued by frontend commands, consumed by either frame entry point. */
void Menu_RequestErrorSound(void) {
    g_MenuErrorSoundPending = 1;
}

s32 Menu_RunFrameWithArg(s32 arg0) {
    if (Menu_SaveBgIsFadeActive() == 0) {
        g_MenuActiveListTarget = (u8 *)arg0;
        Menu_ClearCommandResult();
        Draw_SelectBuffer();
        Menu_DeferredExec();
        MenuInput_DispatchQueuedEvents();
        Menu_ProcessSwapReturnIfPending();
        MemCard_DelayedCallback();
        MenuWidget_UpdateAndDraw();
        Draw_PresentFrame(1);
        if (g_MemCardDialogState >= 2) {
            MemCard_UpdateSavePolling();
        } else if (g_MemCardDialogState > 0) {
            g_MemCardDialogState += 1;
        }
        if (g_MenuErrorSoundPending != 0) {
            g_MenuErrorSoundPending = 0;
            Inv_SetActiveList(9, 0);
        }
        if (Menu_GetCommandResult() != 0) {
            Battle_SetWindowBoundsByMode(g_SavedMenuMode.bytes.mode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}

s32 Menu_RunFrame(void) {
    if (Menu_SaveBgIsFadeActive() == 0) {
        g_MenuActiveListTarget = 0;
        Menu_ClearCommandResult();
        Draw_SelectBuffer();
        Menu_DeferredExec();
        MenuInput_DispatchQueuedEvents();
        Menu_ProcessSwapReturnIfPending();
        MemCard_DelayedCallback();
        MenuWidget_UpdateAndDraw();
        Draw_PresentFrame(1);
        if (g_MemCardDialogState >= 2) {
            MemCard_UpdateSavePolling();
        } else if (g_MemCardDialogState > 0) {
            g_MemCardDialogState += 1;
        }
        if (g_MenuErrorSoundPending != 0) {
            g_MenuErrorSoundPending = 0;
            Inv_SetActiveList(9, 0);
        }
        if (Menu_GetCommandResult() != 0) {
            Battle_SetWindowBoundsByMode(g_SavedMenuMode.bytes.mode);
        }
        return Menu_GetCommandResult();
    }
    Menu_SaveBgAdvanceFade();
    return 0;
}

static inline ItemDataRecord *LookupActiveItem(int index) {
    if (index >= 0 && index < g_InvSlotLimit) return Inv_LookupItemRecordScaled(g_InvItemPtr[index]);
    return 0;
}

static inline void ClearBits(void) {
    int i;
    for (i = 0; i < g_InvSelectionBitWords; i++) g_InvSelectionBits[i] = 0;
}

/* Build the threshold-qualified set; negate its count if a tagged record exists. */
int Inv_RebuildWithBonusSlots(int minSecond, int minThird) {
    int count, i, index;
    ItemDataRecord *item;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    Inv_BuildFilteredPackedList(g_InvRebuildFilterMode ? 0x1FE : 0x200);
    ClearBits();
    count = 0;
    i = 0;
    while (1) {
        if (i >= g_InvPackedListCount) break;
        index = i >= 0 && i < g_InvPackedListCount ? g_InvPackedList[i] : 0;
        item = LookupActiveItem(index);
        if (item->baseStats[0] + item->bonusStats[0] >= g_InvRebuildMinFirstStat &&
            item->baseStats[1] + item->bonusStats[1] >= minSecond &&
            item->baseStats[2] + item->bonusStats[2] >= minThird &&
            item->itemId != 0x93 && item->itemId != 0x61) {
            count++;
            g_InvSelectionBits[i >> 5] |= 1u << (i & 31);
        }
        i++;
    }
    if (count) {
        ItemDataRecord *scan;
        if (g_InvRebuildFilterMode) {
            scan = g_InventoryRuntime.equipment;
            for (; scan < g_InventoryRuntime.equipment + 128 &&
                   !(scan->iconId && scan->kind < 9 && (scan->flags & 0x10));
                 scan++)
                ;
        } else {
            scan = g_InventoryRuntime.equipment;
            for (; scan < g_InventoryRuntime.equipment + 128 &&
                   !(scan->iconId && scan->kind == 9 && (scan->flags & 0x10));
                 scan++)
                ;
        }
        if (scan < g_InventoryRuntime.equipment + 128) count = -count;
    }
    g_InvPackedListCursor = count != 0;
    return count;
}

/* Historical name: mark filtered records whose tail count is below byte 1. */
int Inv_TransferToStorage(void) {
    int count, i, index;
    ItemDataRecord *item;
    g_InvItemPtr = g_AyaInventoryItems;
    g_InvSlotLimit = Inv_GetAyaSlotLimit();
    g_InvSelectionBits = g_InvSelectionBitStorage;
    g_InvSelectionBitWords = 2;
    Inv_BuildFilteredPackedList(0x3FE);
    ClearBits();
    count = 0;
    i = 0;
    while (1) {
        if (i >= g_InvPackedListCount) break;
        index = i >= 0 && i < g_InvPackedListCount ? g_InvPackedList[i] : 0;
        item = LookupActiveItem(index);
        if (item->tailCount < item->tailCapacity) {
            count++;
            g_InvSelectionBits[i >> 5] |= 1u << (i & 31);
        }
        i++;
    }
    return count;
}

void Battle_SyncEquipSlots(void);