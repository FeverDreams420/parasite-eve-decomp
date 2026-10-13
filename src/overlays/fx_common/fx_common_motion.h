#ifndef FX_COMMON_MOTION_H
#define FX_COMMON_MOTION_H

#include "pe1/display_env.h"
#include "pe1/gpu_image.h"
#include "pe1/gte_types.h"
#include "pe1/psyq_libc.h"
#include "pe1/psyq_gpu.h"
#include "pe1/signed_rect.h"

#include "fx_common_setup.h"

extern FxCommonVec3 g_FxCommonMotionAccum1;
extern FxCommonVec3 g_FxCommonMotionAccum0;
extern FxCommonShortVec3 g_FxCommonMotionOrigin1;
extern FxCommonShortVec3 g_FxCommonMotionTarget1;
extern FxCommonShortVec3 g_FxCommonMotionOrigin0;
extern FxCommonShortVec3 g_FxCommonMotionTarget0;
/* Ordinary counter views; legacy callers use volatile scheduling declarations. */
extern s16 g_FxCommonMotionRemaining0;
extern s16 g_FxCommonMotionRemaining1;
extern u32 D_8009D1F4;
extern u32 g_FieldPadBits;
extern s8 D_800B0DB5;
extern void *g_AkaoBgmHandle[1];
extern s32 D_8019C018;
extern s32 D_8019C01C;
extern s16 D_8019C024;
extern s16 D_8019C026;
extern s16 D_8019C028;
extern s16 D_8019C02A;
extern u16 D_8019C030;
extern s16 D_8019C032;
extern s32 D_8019C038;
extern s32 D_8019C03C;
extern s32 D_8019C04C;
extern s32 D_8019C0BC;
extern s32 D_8019C0C0;
extern s16 D_8019C0C4;
extern s32 D_8019C0C8;
extern u8 D_8019C0CC;
extern u8 D_8019C0CD;
extern s16 D_8019C13C;
extern s16 D_8019C13E;
extern s32 D_8019CA68;

/* Double-buffered frame: packet buffer and ordering table, then the
 * DRAWENV and DISPENV handed to PutDrawEnv / PutDispEnv. */
typedef struct FxCommonFrame {
    FxCommonBuffer buffer;
    u8 drawEnv[0x5C];   /* 0x08 */
    u8 dispEnv[0x14];   /* 0x64 */
} FxCommonFrame;
typedef PsxSignedRect FxCommonRect;
extern FxCommonFrame g_FxCommonFrames[2];
extern s32 g_ActiveDrawSlot;
extern u8 D_8019C00E;
extern s32 D_8019CC14;
extern u32 D_8009D280;
/* Plain views of the camera position and its look-at target. */
extern FxCommonVec3 g_FxCommonCameraPosition;
extern FxCommonVec3 g_FxCommonCameraTarget;
extern RoomSpriteMatrix D_8018EFF4;
/* Transform nodes animated along the camera paths (D_801EA578..D_801EA58C). */
extern FxCommonTransformNode *g_FxCommonPathNodes[6];
/* Camera rotation angles. An incomplete array, so the -G8 camera unit keeps
 * the absolute address in a register as retail does. */
extern GteShortVector D_8019BFC4[];
extern RoomSpriteMatrix D_8019CDF0;
extern s8 D_800B0DB2;
extern s8 D_800B0DB4;

void Akao_Cmd_C1_WithSlot(int channel, int value, int mode);
void func_80196498(void);
void func_80191DE8(int enabled);
s32 VSync(s32 value);
void Field_HandleStateTransition(void);
void SetDispMask(s32 value);
void Boot_Shutdown(void);
void func_801942FC(void);
void func_8018F05C(void);
void func_8018F92C(FxCommonVec3 *eye);

/* View frustum side planes rebuilt by func_8018F92C: the four rotated
 * frustum corners, each plane normal, its distance, the opposite corner
 * distance and the normal length. */
extern GteShortVector D_8019BFD0, D_8019BFD8, D_8019BFE0, D_8019BFE8;
extern GteVector D_8019CBB0, D_8019CBD0, D_8019CBF0, D_8019CB50;
extern s32 D_8019CB48, D_8019CB4C, D_8019CBA8, D_8019CA90;
extern s32 D_8019CC04, D_8019CC0C, D_8019CC10, D_8019CBC4;
extern s32 D_8019CBC8, D_8019CC00, D_8019CC08, D_8019CBAC;
void OuterProduct0(GteVector *a, GteVector *b, GteVector *out);
s16 func_80194108(s16 value);
void func_80192740(void);
void func_80192800(void);

/* Per-frame scene draw (func_80192800): depth bias for the next node,
 * echo copies of the camera path nodes and the flicker colour. */
extern s32 D_801EA5E4;
extern FxCommonTransformNode *g_FxCommonPathTailNodes[2];
extern s16 D_8019C0D0[2];
extern u8 D_801EA264[3];
extern void *D_801EA260;
extern FxCommonTransformNode *g_FxCommonEchoNodes[4];
void func_80190E04(FxCommonTransformNode *node, FxCommonBuffer *context, u8 pass,
                   u8 force, u8 mirrored);
void func_80191114(FxCommonLodNode *node, FxCommonBuffer *context, u8 force, u8 mode);
void func_80190D3C(FxCommonTransformNode *node, void *context);
void func_80193478(void);
int func_80191E30(int id, s32 *state);
void func_80191EFC(int handle, volatile s32 *state);
void Menu_DrawTextboxEntries(void);
void func_80193AB0(void);
void ResetGraph(s32 value);
void func_80192030(void);
void func_8018F344(GteMatrix *out, GteShortVector *eye,
                   GteShortVector *target, GteVector *up);
void Akao_Cmd_F0(void);
void Akao_Cmd_F1(void);
int func_80038D48(void);
s16 func_801958D4(s16 resourceId, u8 action);
void FxCommon_SetMotionEndpoints(FxCommonShortVec3 *a, FxCommonShortVec3 *b, int bi, int ai);
void func_80195D3C(void);

PE1_STATIC_ASSERT(sizeof(FxCommonFrame) == 0x78, fx_common_frame_size);
PE1_STATIC_ASSERT(sizeof(FxCommonVec3) == 12, fx_motion_position_size);
PE1_STATIC_ASSERT(sizeof(FxCommonShortVec3) == 6, fx_motion_endpoint_size);

#endif
