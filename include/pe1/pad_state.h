#ifndef PE1_PAD_STATE_H
#define PE1_PAD_STATE_H

#include "common.h"

/* Game button bits sampled once per frame by Field_HandleStateTransition
 * (Boot_RenderFlagsAndPadInput.c). */
extern u32 D_8009D26C; /* held (g_FieldPadBits) */
extern u32 D_8009D238; /* held on the previous frame */
extern u32 D_8009D1F4; /* pressed this frame */
extern u32 D_8009D1E4; /* released this frame */

#endif /* PE1_PAD_STATE_H */
