#ifndef PE1_PM_H
#define PE1_PM_H

#include "pe1/pm_types.h"

int Scene_LoadRoomAssets(u32 command, void *owner);
/* Stops the process in a slot; owner is compared as an address word. */
int Pm_Stop(int slot, int owner, int mode);
extern PmAuxiliaryPointerTable g_PmAuxiliaryPointerTable
    __asm__("D_800E10A0");

#ifdef PE1_PM_LEGACY_RAW_VIEWS
/* A few exact-match paths retain raw global views; typed operations use aliases
 * for the same linker symbols where the retail addressing permits it. */
extern PmPrimarySlot *g_PmSlotTableTyped __asm__("g_PmSlotTable");
extern PmSecondarySlot *g_PmSlotTable2Typed __asm__("g_PmSlotTable2");
extern u32 g_PmSlotBufferTyped[] __asm__("g_PmSlotBuffer");
extern PmPrimarySlot *g_PmSlotTable;
extern PmSecondarySlot *g_PmSlotTable2;
extern u32 g_PmSlotBuffer[];
extern PmCommand **g_PmCmdHandlerTable;
extern char *g_PmSlotTableRaw __asm__("g_PmSlotTable");
extern char *g_PmSlotTable2Raw __asm__("g_PmSlotTable2");
#else
extern PmPrimarySlot *g_PmSlotTable;
extern PmSecondarySlot *g_PmSlotTable2;
extern u32 g_PmSlotBuffer[];
extern PmCommand **g_PmCmdHandlerTable;
int Scene_FreeEntityTable(void *owner);
/* Callers pass the three result words as plain ints; Pm_ProcessManager.c
 * defines them as pointers. */
int Pm_SendCmd(int command, int arg1, int arg2, int arg3, int arg4, int arg5);
#endif

#endif
