#ifndef PE1_EXT_M273
#define PE1_EXT_M273

#include "common.h"
#include "pe1/battle.h"
#include "pe1/battle_setup.h"
#include "pe1/field_actor.h"
#include "pe1/gte_types.h"
#include "pe1/room_fx.h"

#include "room_m273.h"
#include "room_m273_effects.h"

typedef GteShortVector Vector;

/* Threshold model triple callbacks and controller. */
typedef struct { short x,y,z,phase,scaleXZ,scaleY; unsigned short parameter12,unknown14; } ThresholdTripleEffect;

typedef struct { void *asset,*pool; } AssetState;

typedef struct { unsigned char r, g, b, unknown; } Color;

/* Partial storage views; the fourth halfword carries the emitter flag. */
typedef RoomM273PairPoolRecord Entry;

typedef struct { Entry entries[2]; unsigned char unknown[14]; unsigned char stopped; } EmitterState;

typedef struct {
    char pad_00[4];
    s32 *source;
    s16 scaleX;
    s16 scaleY;
    u16 value;
    u16 step;
} RoomM273PulseState;

typedef struct {
    u16 *source;
} RoomM273PhaseState;

extern FieldActor *g_PlayerEntity;
void func_80192838(RoomSelectionState *state);
void func_80192D50(RoomSelectionState *state);
extern struct BattleEntity *D_8009D254;
extern u8 D_8019A8C8[];
extern s32 Inv_ScrambleGrid(void);
extern short D_800966EE[];
extern GteRotation D_8019AB68;
extern unsigned char D_8019AE5C;
extern u8 D_8019AE9A;
extern RoomM273PulseSeed D_8019AC20[2];
extern int FieldFx_UpdateCallbackList(void *);
extern short D_8019AC18[];
extern int D_8019AC0C[];
extern AssetState D_8019AE8C;
extern void *D_8019AE90;
extern unsigned char D_8019AE9A;
extern void FieldFx_ModelSetTexture(int,int);
int func_80193CB8(int mode,ThresholdTripleEffect *effect);
extern short D_8019AE60, D_8019AE64;
int func_80199F84(int mode, RoomM273PulseState *state);
extern s16 D_8019ACA8;
extern s16 D_8019AE6C, D_8019AE70, D_8019AE74, D_8019AE78;
extern u8 D_8019AEF8, D_8019AEFA;
extern unsigned char D_8019AEF8;
extern short D_8019AEE8;
extern void *D_8019AE7C;
int func_80195984(int mode, RoomM273PulseInput *input);
int func_8019A290(int mode, RoomM273PhaseState *state);
extern Color D_8018F1E4;
extern unsigned char D_8019AF69,D_8019AF68;
extern s16 D_8019AEB2;
extern RoomFxSeed8 D_8019AEAC;
extern unsigned short D_8019AF60;
extern Vector D_8019AD60;
extern int Math_IntSqrt(int);
extern short D_8019AD68[];
extern RoomM273RisingBatchBuffer D_8019AF0C;
extern short D_8019AE84;
extern s16 D_8019AF0A;
extern unsigned char D_8019AD70[],D_8019AF6A,D_8019AF69;
extern volatile unsigned short D_8019AEFE,D_8019AF00;
extern char D_8019AE00[];
extern unsigned char D_8019AE18[], D_8019AB70[];
extern void *D_8019AF6C;
extern EmitterState D_8019AF84;
extern short D_8019AE88;
extern short D_8019AE98;
extern u8 D_8019AE68;
extern s16 D_8019AEEA;
extern s16 D_8019AEEE;
extern s16 D_8019AEF2;
extern s16 D_8019AEF6;
#endif /* PE1_EXT_M273 */
