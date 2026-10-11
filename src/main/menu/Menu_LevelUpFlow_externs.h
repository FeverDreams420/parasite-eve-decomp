#ifndef PE1_MENU_LEVEL_UP_FLOW_VIEWS_H
#define PE1_MENU_LEVEL_UP_FLOW_VIEWS_H
/* Match debt: three names for the save state's bonus pool halfword
 * (D_800C0E00.pe_bonus_pool). Aya_SetTotalExp reads, writes and re-reads it
 * as absolute accesses; one symbol lets CSE keep its address in a register
 * (lev 11). */
extern u16 D1E_read[16] __asm__("D_800C0E1E");
extern u16 D1E_write[16] __asm__("D_800C0E1E");
extern u16 D1E_bonus[16] __asm__("D_800C0E1E");
#endif
