#ifndef PE1_MENU_MAIN_UPDATE_VIEWS_H
#define PE1_MENU_MAIN_UPDATE_VIEWS_H
/* Match debt: separate names for the active draw slot word (D_8009CDDC).
 * Retail reloads the slot before the last colour store of each ATB colour
 * case; with one symbol CSE reuses the first load and cross-jumping merges
 * the cases (3341 differing lines). */
extern s32 draw_slot_status0 __asm__("D_8009CDDC");
extern s32 draw_slot_status1 __asm__("D_8009CDDC");
extern s32 draw_slot_status2 __asm__("D_8009CDDC");
#endif
