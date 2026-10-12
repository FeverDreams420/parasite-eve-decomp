#ifndef PE1_FIELD_EFFECT_MODULES_H
#define PE1_FIELD_EFFECT_MODULES_H

#include "common.h"
#include "pe1/gte_types.h"
#include "pe1/field_engine_state.h"

/* The eight field effect modules the descriptor table at 0x80094188 lists
 * (FieldEng_GlowEffects.c .. FieldEng_BurstEffectC.c). Each module is a class
 * of seven methods, in the order of pe1/room_module.h: a no-op, the setup
 * that plants the module's command script into the engine slot, the spawn
 * forwarder, the draw pass over the draw list, the update pass (update list,
 * then the script with the init list and slot data sizes), the close method
 * and a second no-op. The lists and scripts are the module's own data. */

struct FieldActor;
struct BattleEntity;

/* Player battle entity; the glow modules index their tables by its current
 * action id. */
extern struct BattleEntity *D_8009D254;

/* Effect resources the scene loaders install: the AKAO sound header the
 * burst modules start, the two texture pages FieldEng loads into VRAM
 * (0x380, 0x100), the third resource FieldAnim hands out with them, and the
 * asset table the task spawner searches. */
extern void *g_FieldEffectSoundHeader;
extern void *g_FieldEffectTexturePage0;
extern void *g_FieldEffectTexturePage1;
extern void *g_FieldEffectResource2;
extern void *g_FieldEffectAssetTable;
void *FieldAnim_GetEffectResource(int index);

/* Glow effect (FieldEng_GlowEffects.c). */
extern int g_GlowEffectScript[];
extern FieldEngHandler g_GlowEffectInitList[];
extern FieldEngHandler g_GlowEffectDrawList[];
extern FieldEngHandler g_GlowEffectUpdateList[];
extern u16 g_GlowEffectSlotSizes[];
extern s16 g_GlowEffectScaleByAction[];
extern GteShortVector g_GlowEffectFlareOffsets[];
extern GteShortVector g_GlowEffectHaloOffsets[];
extern GteShortVector g_GlowEffectPointOffset;
extern GteShortVector g_GlowEffectSpriteRotation;
extern struct FieldActor *g_GlowEffectOwner;
int GlowEffect_ClassNop0(void);
int GlowEffect_PlantScript(char *object);
int GlowEffect_Spawn6(int a, int b, int c, int d, int e, int f);
int GlowEffect_RegisterDrawList(void *object);
int GlowEffect_RegisterPairedTables(void *object);
int GlowEffect_CloseTarget(char *object);
int GlowEffect_ClassNop6(void);

/* Second glow effect (FieldEng_GlowEffects.c). */
extern int g_GlowEffectBScript[];
extern FieldEngHandler g_GlowEffectBInitList[];
extern FieldEngHandler g_GlowEffectBDrawList[];
extern FieldEngHandler g_GlowEffectBUpdateList[];
extern u16 g_GlowEffectBSlotSizes[];
extern s16 g_GlowEffectBScaleByAction[];
extern GteShortVector g_GlowEffectBFlareOffsets[];
extern GteShortVector g_GlowEffectBPointOffset;
extern struct FieldActor *g_GlowEffectBOwner;
int GlowEffectB_ClassNop0(void);
int GlowEffectB_PlantScript(char *object);
int GlowEffectB_Spawn6(int a, int b, int c, int d, int e, int f);
int GlowEffectB_RegisterDrawList(void *object);
int GlowEffectB_RegisterPairedTables(void *object);
int GlowEffectB_CloseTarget(char *object);
int GlowEffectB_ClassNop6(void);

/* Third glow effect (FieldEng_GlowEffects.c). */
extern int g_GlowEffectCScript[];
extern FieldEngHandler g_GlowEffectCInitList[];
extern FieldEngHandler g_GlowEffectCDrawList[];
extern FieldEngHandler g_GlowEffectCUpdateList[];
extern u16 g_GlowEffectCSlotSizes[];
extern s16 g_GlowEffectCScaleByAction[];
extern GteShortVector g_GlowEffectCFlareOffsets[];
extern struct FieldActor *g_GlowEffectCOwner;
int GlowEffectC_ClassNop0(void);
int GlowEffectC_PlantScript(char *object);
int GlowEffectC_Spawn6(int a, int b, int c, int d, int e, int f);
int GlowEffectC_RegisterDrawList(void *object);
int GlowEffectC_RegisterPairedTables(void *object);
int GlowEffectC_CloseTarget(char *object);
int GlowEffectC_ClassNop6(void);

/* Four-layer glow effect (FieldEng_GlowEffects.c). */
extern int g_FourLayerGlowScript[];
extern FieldEngHandler g_FourLayerGlowInitList[];
extern FieldEngHandler g_FourLayerGlowDrawList[];
extern FieldEngHandler g_FourLayerGlowUpdateList[];
extern u16 g_FourLayerGlowSlotSizes[];
extern s16 g_FourLayerGlowScaleByAction[];
extern GteShortVector g_FourLayerGlowFlareOffsets[];
extern GteShortVector g_FourLayerGlowHaloOffsets[];
extern GteShortVector g_FourLayerGlowPointOffsetA;
extern GteShortVector g_FourLayerGlowPointOffsetB;
extern GteShortVector g_FourLayerGlowSpriteRotation;
extern struct FieldActor *g_FourLayerGlowOwner;
int FourLayerGlow_ClassNop0(void);
int FourLayerGlow_PlantScript(char *object);
int FourLayerGlow_Spawn6(int a, int b, int c, int d, int e, int f);
int FourLayerGlow_RegisterDrawList(void *object);
int FourLayerGlow_RegisterPairedTables(void *object);
int FourLayerGlow_CloseTarget(char *object);
int FourLayerGlow_ClassNop6(void);

/* Point sprite effect (FieldEng_PointSpriteEffect.c). */
extern u8 g_PointSpriteScript[];
extern FieldEngHandler g_PointSpriteInitList[];
extern FieldEngHandler g_PointSpriteDrawList[];
extern FieldEngHandler g_PointSpriteUpdateList[];
extern u16 g_PointSpriteSlotSizes[];
extern struct FieldActor *g_PointSpriteOwner;
int PointSprite_ClassNop0(void);
int PointSprite_PlantScript(char *object);
int PointSprite_Spawn6(int a, int b, int c, int d, int e, int f);
int PointSprite_RegisterDrawList(void *object);
int PointSprite_RegisterPairedTables(void *object);
int PointSprite_CloseTarget(char *object);
int PointSprite_ClassNop6(void);

/* Burst effect (FieldEng_BurstEffect.c, _2.c, _3.c); its script and setup
 * are declared with the burst records in pe1/field_anim.h. */
extern FieldEngHandler g_BurstEffectInitList[];
extern FieldEngHandler g_BurstEffectDrawList[];
extern FieldEngHandler g_BurstEffectUpdateList[];
extern u16 g_BurstEffectSlotSizes[];
extern GteShortVector g_BurstEffectSpriteRotation;
extern struct FieldActor *g_BurstEffectTarget;
int BurstEffect_ClassNop0(void);
int BurstEffect_Spawn6(int a, int b, int c, int d, int e, int f);
int BurstEffect_RegisterDrawList(void *object);
int BurstEffect_RegisterPairedTables(void *object);
int BurstEffect_CloseTarget(u8 *object);
int BurstEffect_ClassNop6(void);

/* Second burst effect (FieldEng_BurstEffectB.c). */
extern int g_BurstEffectBScript[];
extern FieldEngHandler g_BurstEffectBInitList[];
extern FieldEngHandler g_BurstEffectBDrawList[];
extern FieldEngHandler g_BurstEffectBUpdateList[];
extern u16 g_BurstEffectBSlotSizes[];
extern struct FieldActor *g_BurstEffectBTarget;
int BurstEffectB_ClassNop0(void);
int BurstEffectB_PlantScript(char *object);
int BurstEffectB_Spawn6(int a, int b, int c, int d, int e, int f);
int BurstEffectB_RegisterDrawList(void *object);
int BurstEffectB_RegisterPairedTables(void *object);
int BurstEffectB_CloseTarget(u8 *object);
int BurstEffectB_ClassNop6(void);

/* Third burst effect (FieldEng_BurstEffectC.c). */
extern int g_BurstEffectCScript[];
extern FieldEngHandler g_BurstEffectCInitList[];
extern FieldEngHandler g_BurstEffectCDrawList[];
extern FieldEngHandler g_BurstEffectCUpdateList[];
extern u16 g_BurstEffectCSlotSizes[];
extern struct FieldActor *g_BurstEffectCTarget;
int BurstEffectC_ClassNop0(void);
int BurstEffectC_PlantScript(char *object);
int BurstEffectC_Spawn6(int a, int b, int c, int d, int e, int f);
int BurstEffectC_RegisterDrawList(void *object);
int BurstEffectC_RegisterPairedTables(void *object);
int BurstEffectC_CloseTarget(char *object);
int BurstEffectC_ClassNop6(void);

#endif /* PE1_FIELD_EFFECT_MODULES_H */
