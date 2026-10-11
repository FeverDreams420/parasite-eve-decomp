#ifndef PE1_EXT_853D3F2A2C9F
#define PE1_EXT_853D3F2A2C9F
extern volatile u32 *gpu_status __asm__("D_80094574");
extern volatile u32 *hblank_counter __asm__("D_80094578");
extern volatile u32 last_hblank __asm__("D_8009457C");
extern int last_vblank __asm__("D_80094580");
extern volatile int g_VSyncCount;
#endif /* PE1_EXT_853D3F2A2C9F */
