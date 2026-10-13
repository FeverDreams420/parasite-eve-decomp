#ifndef PE1_EXT_TEXT_SCREEN
#define PE1_EXT_TEXT_SCREEN

#include "common.h"
#include "pe1/boot_display.h"

extern BootDisplayCallbacks D_80125B4C, D_80125B68;
extern u32 g_GameStateFlags, D_8009D1F4;
extern u32 g_FieldPadBits;
extern s32 D_80172CE0;
extern u32 g_BootDisplayFrameCounter;
extern void func_8012562C(void);
extern void func_80125744(void);
extern void func_80125894(s32);
s32 BootDisplay_DrawTextBlock(void);
extern void Field_HandleStateTransition(void);
extern void func_801255A4(void);
#endif /* PE1_EXT_TEXT_SCREEN */
