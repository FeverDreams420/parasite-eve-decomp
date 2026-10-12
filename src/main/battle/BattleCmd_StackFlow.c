/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: --use-comm-section -G8 */

#include "common.h"
#include "pe1/battle_cmd.h"
#include "pe1/inventory.h"
#include "pe1/inventory_slots.h"
#include "pe1/battle_modifiers.h"

BattleCmdEntry *BattleCmd_AllocSlot(void) {
    BattleCmdEntry *top;
    BattleCmdEntry *end;
    BattleCmdEntry *limit;
    BattleCmdEntry *next;

    top = g_BattleCmdStackTop;
    end = (BattleCmdEntry *)D_800A1B30;
    if (top < end) {
        next = top + 1;
        g_BattleCmdStackTop = next;
    } else {
        top = end - 4;
        end = end - 1;
        if (top < end) {
            limit = end;
            do {
                BattleCmdEntry *src = top + 1;
                *top = *src;
                top += 1;
            } while (top < limit);
        }
    }
    return top;
}

void BattleCmd_UndoPending(void) {
    BattleCmdEntry *entry;
    BattleCmdEntry *entry_tmp;
    BattleCmdEntry *top;
    int opcode;
    s16 *slot;
    ItemDataRecord *data;
    int index;

    top = g_BattleCmdStackTop;
    if (g_BattleCmdStack < top) {
        entry_tmp = top - 1;
        opcode = (top - 1)->header.word;
        g_BattleCmdStackTop = entry_tmp;
        entry = entry_tmp;

        switch (opcode) {
        case 0:
            slot = &g_AyaInventoryItems[entry->payload.inventory_restore.slot_index];
            if (*slot == 0) {
                *slot = entry->payload.inventory_restore.item_id;
            } else {
                Inv_AddItem(entry->payload.inventory_restore.item_id);
            }
            break;

        case 1:
            break;

        case 2:
            g_InvTrackedSlots[0] =
                Inv_FindIndexByData(entry->payload.equip_restore.item_data);
            break;

        case 3:
            data = entry->payload.equip_restore.item_data;
            index = -1;
            if (data != 0) {
                index = Inv_FindIndexByData(data);
            }
            D_800C0E22[0] = index;
            break;

        case 4:
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data0)->ammo =
                entry->payload.ammo_restore.ammo0;
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data1)->ammo =
                entry->payload.ammo_restore.ammo1;
            ((ItemDataRecord *)entry->payload.ammo_restore.item_data2)->ammo =
                entry->payload.ammo_restore.ammo2;
            break;
        }
    }
}

void BattleCmd_ResetTableCursor(void) {
    g_BattleCmdStackTop = g_BattleCmdStack;
}

void BattleCmd_UndoAllPending(void) {
    if (g_BattleCmdStack < g_BattleCmdStackTop) {
        BattleCmdEntry *base = g_BattleCmdStack;

        do {
            BattleCmd_UndoPending();
        } while (base < g_BattleCmdStackTop);
    }
}
