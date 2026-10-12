#ifndef PE1_INVENTORY_STATE_H
#define PE1_INVENTORY_STATE_H

#include "common.h"

/* Inventory list, sort and selection state shared by src/main/item. */

typedef struct InventorySlotRange {
    s16 start;
    s16 stop;
} InventorySlotRange;

extern signed char g_AyaEquippedArmorSlot[];
extern int D_8009D04C;
extern s16 D_800C0EAA[];
extern s16 D_800C1F7E[];
extern s16 D_800C2022[];
extern u8 D_800A1E6D[][32];
extern s16 D_800A1E76[][16];
extern int g_InvBonusSlotCount;
extern s32 D_8009D0A4;
extern u16 *g_InvSortListBase;
extern s32 g_InvSortListCount;
extern s32 D_8009D0A8;
extern s32 g_InvSortRankTableA[];
extern s32 D_80092428[];
extern s32 D_80092440[];
extern s32 g_InvTypeRankTableA[];
extern s32 g_InvTypeRankTableB[];
extern s32 *g_InvSortRankTable;
extern s32 *g_InvSortTypeRankTable;
extern s32 D_80092410[];
extern u16 g_InvItemArray[];
extern u8 g_ItemCodeRemapTable[];
extern unsigned char g_ItemDropRngXorKeyHigh[];
extern unsigned char g_ItemDropRngTable[];
extern unsigned char g_ItemDropRngXorKeyLow[];
extern u8 g_CursorRenderDataBlock[];
extern s16 g_BattleCountTable[];
extern int g_InvActiveListCount;
extern s32 g_InvCategoryBaseItemId;
extern InventorySlotRange D_800923D8[];
extern InventorySlotRange D_800923F8[];
extern short g_InvPackedList[];

#endif /* PE1_INVENTORY_STATE_H */
