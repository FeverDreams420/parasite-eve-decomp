#include "common.h"
#include "pe1/inventory_slots.h"
/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 */


void Inv_BuildFilteredPackedList(int mask) {
    int limit;
    int index;
    int item_id;
    int offset;
    ItemDataRecord *data;
    int type;
    s16 *out;
    int saved_item_id;
    int stack_pad[2];

    limit = g_InvSlotLimit;
    out = g_InvPackedList;
    index = 0;
    if (limit > 0) {
        do {
            if (index < 0 || index >= limit) {
                data = 0;
            } else {
                item_id = g_InvItemPtr[index];
                saved_item_id = (s16)item_id;
                if (item_id - 0x100U < 0x80U) {
                    data = (ItemDataRecord *)(g_EquipItemDataTable +
                                              (item_id << 5));
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        data = Item_LookupBaseData(offset);
                    } else if (saved_item_id - 0x200U < 9U) {
                        int shifted;

                        shifted = saved_item_id << 5;
                        data = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
                    } else {
                        data = 0;
                    }
                }
            }

            if (data != 0) {
                type = data->kind;
            } else {
                type = 0;
            }
            if (((mask >> type) & 1) != 0) {
                *out++ = index;
            }

            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    g_InvPackedListCursor = 0;
    g_InvPackedListCount = (out - g_InvPackedList);

}

void Inv_BuildFilteredPackedListExcluding(int mask, int excluded) {
    int limit;
    int index;
    int item_id;
    int offset;
    ItemDataRecord *lookup;
    ItemDataRecord *data;
    int type;
    s16 *out;
    int saved_item_id;
    int stack_pad[2];

    limit = g_InvSlotLimit;
    out = g_InvPackedList;
    index = 0;
    if (limit > 0) {
        do {
            if (index >= 0 && index < limit) {
                item_id = g_InvItemPtr[index];
                saved_item_id = item_id;
                if (item_id - 0x100U < 0x80U) {
                    lookup = (ItemDataRecord *)(g_EquipItemDataTable +
                                               (item_id << 5));
                } else {
                    offset = item_id - 1;
                    if ((unsigned int)offset < 0xFF) {
                        lookup = Item_LookupBaseData(offset);
                    } else if (saved_item_id - 0x200U < 9U) {
                        int shifted;
                        shifted = saved_item_id << 5;
                        lookup = (ItemDataRecord *)(g_KeyItemDataTable + shifted);
                    } else {
                        lookup = 0;
                    }
                }
                data = lookup;
            } else {
                data = 0;
            }
            if ((data != 0) &&
                    (((mask >> data->kind) & 1) != 0) &&
                    (data->tailCount != 0) &&
                    (index != excluded)) {
                *out++ = index;
            }

            limit = g_InvSlotLimit;
            index++;
        } while (index < limit);
    }

    g_InvPackedListCursor = 0;
    g_InvPackedListCount = (out - g_InvPackedList);

}
