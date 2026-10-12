#ifndef PE1_INVENTORY_SORT_H
#define PE1_INVENTORY_SORT_H

#include "pe1/inventory.h"

extern int g_InvSortStatIndex;
extern ItemDataRecord *(*g_InvSortLookup)(int index);
extern unsigned char *D_8009D0B8;
extern unsigned char *D_8009D0BC;

int Inv_CompareItemsByStatForSort(const void *leftElement, const void *rightElement);
int Inv_CompareItemsForSort(const void *leftElement, const void *rightElement);

void Inv_SortInventoryByMode(int mode, int key);
void Sort_ContainerItems(int key);
void Sort_InventoryItems(int mode, int key);

/* Wayne storage item IDs, sorted in place with qsort. */
extern s16 g_WayneStorageItems[];

#endif
