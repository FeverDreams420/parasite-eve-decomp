#ifndef PE1_EXT_E08
#define PE1_EXT_E08

#include "common.h"
#include "pe1/battle.h"
#include "pe1/field_actor.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/room_fx.h"

typedef struct {
    char bytes[4];
} SceneE08FourBytes;

extern FieldGlowSprite g_SceneE08ParticleClusterSprite;
extern FieldGlowSprite g_SceneE08GlowRingsSprite;
extern RoomFxSeed8 g_SceneE08GlowRingsRotationSeed;
extern u8 g_SceneE08GlowRingsGlowRamp[], g_SceneE08GlowRingsRingRamp[];
extern s16 g_SceneE08ParticleClusterSpriteDepth;
extern FieldGlowSprite g_SceneE08GlowSpritesSprite, g_SceneE08RingSpritesFlashSprite, g_SceneE08RingSpritesGlowSprite, g_SceneE08PlayerOrbPointSprite;
extern void FieldFx_SetScriptContext(void *);
extern s16 g_SceneE08GlowSpritesSpriteDepth;
extern u8 g_SceneE08RingSpritesRing0Mode[];
extern u8 g_SceneE08RingSpritesSource[];
extern u8 g_SceneE08RingSpritesRing0Color1R, g_SceneE08RingSpritesRing0Color1G, g_SceneE08RingSpritesRing0Color1B;
extern u8 g_SceneE08RingSpritesRing0Color0R, g_SceneE08RingSpritesRing0Color0G, g_SceneE08RingSpritesRing0Color0B;
extern u16 g_SceneE08RingSpritesRing0Offset, g_SceneE08RingSpritesRing0Intensity, g_SceneE08RingSpritesRing0Extent0, g_SceneE08RingSpritesRing0Extent1;
extern struct FieldActor *g_PlayerEntity;
extern int g_SceneE08CentreX, g_SceneE08CentreZ;
extern u8 *g_SceneE08GlowModel;
extern u8 D_80199680, D_80199681, D_80199682, D_80199684, D_80199685, D_80199686;
extern s16 D_80199688, D_8019968A;
extern u8 g_SceneE08CentreGlowSprite[];
extern u8 g_SceneE08CentreGlowSpriteG, g_SceneE08CentreGlowSpriteB, g_SceneE08CentreGlowSpriteCell, g_SceneE08CentreGlowSpriteClut, g_SceneE08CentreGlowSpriteFlip;
extern s16 g_SceneE08CentreGlowSpriteOffset, g_SceneE08CentreGlowSpriteDepth;
extern FieldGlowSprite D_801996B0;
extern struct BattleEntity *D_8009D254;
extern u8 g_SceneE08RadialRibbonsRamp[];
extern u16 g_SceneE08RingEffectRing0Mode;
extern u8 g_SceneE08RingEffectSource[];
extern char g_SceneE08RingEffectRings[];
extern s32 g_SceneE08CentreX, g_SceneE08CentreZ;
extern u8 g_SceneE08SpiralRibbonsRamp[];
extern SceneE08FourBytes g_SceneE08ScreenFadeColor;
extern char g_SceneE08OrbitParticlesRamp[];
extern int rand(void);
extern FieldGlowSprite D_80199540, g_SceneE08SpinningGlowSprite, g_SceneE08TrailGlowFloorSprite;
extern u8 g_SceneE08LayeredSpriteR, g_SceneE08LayeredSpriteG, g_SceneE08LayeredSpriteB, g_SceneE08LayeredSpriteCell, g_SceneE08LayeredSpriteClut, g_SceneE08LayeredSpriteFlip;
extern s16 g_SceneE08LayeredSpriteOffset, g_SceneE08LayeredSpriteDepth;
extern s16 D_80199558;
extern volatile u8 D_80199555;
extern u8 D_80199554;
extern s16 D_8019955A;
extern u8 D_80199550;
extern u8 D_80199551;
extern u8 D_80199552;
extern u8 D_80199556;
extern s16 D_801994A0;
extern s16 D_801994A2;
extern u8 D_80199490;
extern u8 D_80199491;
extern u8 D_80199492;
extern u8 D_80199494;
extern u8 D_80199495;
extern u8 D_80199496;
extern u8 D_80199498;
extern u8 D_80199499;
extern u8 D_8019949A;
extern u8 D_8019949C;
extern u8 D_8019949D;
extern u8 D_8019949E;
extern RoomFxVec4 g_SceneE08LayeredSpriteScale;
extern u8 g_SceneE08SpinningGlowRamp;
extern s16 g_SceneE08ArmGlowSpriteDepth;
#endif /* PE1_EXT_E08 */
