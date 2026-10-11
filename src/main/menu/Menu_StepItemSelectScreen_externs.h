#ifndef PE1_EXT_7FBDD1857833
#define PE1_EXT_7FBDD1857833
extern int D_8009CEF0, D_8009CF18, D_8009CF1C;
extern int D_8009CF30, D_8009CEF8;
extern int D_800A1888[], D_800A188C[], D_800A1890[], D_800A1894[];
extern MenuWidgetNode *MenuWidget_GetChild(MenuWidgetNode *node, int index);
extern void Menu_CreateEquipScreen(MenuWidgetNode *node);
extern void Menu_CreateSkillActionScreen(MenuWidgetNode *node);
extern void Menu_CreateInventoryListView(MenuWidgetNode *node);
extern void Menu_CreateEquipItemSelectionView(MenuWidgetNode *node);
extern MenuWidgetNode *MenuWidget_FindByModeAndSelectedBase(int mode, int base);
extern int Battle_IsActiveWrapped(void);
extern void Menu_CreateMainMenuView(MenuWidgetNode *node);
extern void Menu_CreateInvSwapView(MenuWidgetNode *node, int mode);
extern int Inv_CountByCategory(int category);
extern int WayneStorage_CountItemType(int item);
extern void Menu_StepInventoryRoot(int mode, int arg1, int arg2);
extern void BattleCmd_ResetTableCursor(void);
extern void Menu_InitScrollDirection(void);
#endif /* PE1_EXT_7FBDD1857833 */
