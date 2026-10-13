#ifndef PSYQ_LIBCD_STCDINTERRUPT_EXTERNS_H
#define PSYQ_LIBCD_STCDINTERRUPT_EXTERNS_H

extern volatile u8 *D_8009B334;
extern volatile u8 *D_8009B338;
extern volatile u32 *D_8009B33C;
extern volatile u32 *D_8009B340;
extern volatile u32 *D_8009B34C;
extern volatile u32 *D_8009B35C;
extern s32 D_8009B374;
extern StHEADER *D_800A34A0;
extern s32 g_CdStreamDataReadyFlag, D_800BCD7C, g_CdStreamRingReadSlot;
extern s32 D_800C0DC0;
extern StSectorPayload *D_800C0DC4;

#endif /* PSYQ_LIBCD_STCDINTERRUPT_EXTERNS_H */
