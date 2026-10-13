#ifndef PE1_PAD_STATE_H
#define PE1_PAD_STATE_H

#include "common.h"

/* Game button bits sampled once per frame by Field_HandleStateTransition
 * (boot/init.c). */
extern u32 g_FieldPadBits; /* held (g_FieldPadBits) */
extern u32 D_8009D238; /* held on the previous frame */
extern u32 D_8009D1F4; /* pressed this frame */
extern u32 D_8009D1E4; /* released this frame */

/* Raw controller packet of port 1 (g_AnalogStickState): the type/size
 * header, the active-low button bits and the left stick X/Y bytes. */
extern u16 D_800BE9A0;
extern u16 D_800BE9A2;
extern u8 D_800BE9A6, D_800BE9A7;

#endif /* PE1_PAD_STATE_H */
