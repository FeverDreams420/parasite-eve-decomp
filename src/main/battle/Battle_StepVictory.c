/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/battle_status.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/aya.h"
/* COMMON metadata retains retail GP-relative addressing; symbols provide storage. */
u8 g_BattleEndTimer, D_8009CE74;
Combatant *D_8009D278;
int D_8009D28C;
s8 D_8009D2A0;
/* Eight-stage transition: model fades, an effect spawn, stat recovery and
 * HUD reset. Returns 1 once the stats have been restored. */
int Battle_StepVictory(void)
{
    int result = 0;
    BattleEntity *player;
    BattleEntity *spawnPlayer;
    Combatant *core;
    u8 countdown;
    u16 brightness;
    Combatant *restoredCore;
    switch (D_8009CE74) {
    case 0:
        player = D_8009D254;
        if (player->actionMode != 0x13) {
            Entity_SetActionMode(player, 0x13);
            player = D_8009D254;
        }
        if (player->animLastFrame != (u16)(player->animFrame >> 16)) {
            player->entityFlags &= ~0x100;
            break;
        }
        g_BattleEndTimer = 16;
        player->entityFlags |= 0x100;
        D_8009CE74++;
        break;
    case 1:
        brightness = g_BattleEndTimer;
        if (brightness != 0) {
            brightness = ~(brightness << 3) & 0xFF;
            Render_FadeEntityColor(&D_8009D254->renderObject, brightness, brightness, brightness);
            Render_FadeEntityColor(&D_800B0CEC, brightness, brightness, brightness);
            g_BattleEndTimer--;
            break;
        }
        Render_FadeEntityColor(&D_8009D254->renderObject, 255, 255, 255);
        {
            RenderObjectEntity *partner = &D_800B0CEC;

            Render_FadeEntityColor(partner, 255, 255, 255);
            Anim_SetInterpRate(&D_8009D254->renderObject, 30);
            D_8009D254->renderObject.flags_9C |= 2;
            Anim_SetInterpRate(partner, 30);
            D_800B0CEC.flags_9C |= 2;
        }
        D_8009CE74++;
        break;
    case 2:
        if (D_8009D254->renderObject.variant_visible != 0 || D_800B0CEC.variant_visible != 0)
            break;
        Scene_LoadRoomAssets(0x69, D_8009D254);
        spawnPlayer = D_8009D254;
        Asset_Find08Alt(0x4AF, 0, spawnPlayer->posX.parts.integer, spawnPlayer->posY.parts.integer, spawnPlayer->posZ.parts.integer);
        g_BattleEndTimer = 0x34;
        D_8009CE74++;
        break;
    case 3:
        if (g_BattleEndTimer != 0) {
            g_BattleEndTimer--;
            break;
        }
        D_8009D254->renderObject.variant_visible = 1;
        Anim_SetInterpRate(&D_8009D254->renderObject, 30);
        D_8009D254->renderObject.flags_9C |= 4;
        Render_FadeEntityColor(&D_8009D254->renderObject, 255, 255, 255);
        D_800B0CEC.variant_visible = 1;
        {
            RenderObjectEntity *partner = &D_800B0CEC;

            Anim_SetInterpRate(partner, 30);
            D_800B0CEC.flags_9C |= 4;
            Render_FadeEntityColor(partner, 255, 255, 255);
        }
        Entity_SetActionMode(D_8009D254, 0xF);
        g_BattleEndTimer = 30;
        D_8009D254->animFrame = 0x230000;
        D_8009D254->animPrev.fixed = 0x220000;
        D_8009CE74++;
        break;
    case 4:
        countdown = g_BattleEndTimer;
        if (countdown == 16) {
            D_8009CE74++;
            player = D_8009D254;
            player->entityFlags &= ~0x100;
        } else {
            g_BattleEndTimer = countdown - 1;
        }
        break;
    case 5:
        brightness = g_BattleEndTimer;
        if (brightness != 0) {
            brightness = ((brightness << 3) + 127) & 0xFF;
            Render_FadeEntityColor(&D_8009D254->renderObject, brightness, brightness, brightness);
            Render_FadeEntityColor(&D_800B0CEC, brightness, brightness, brightness);
            g_BattleEndTimer--;
        } else {
            D_8009CE74++;
            D_8009D254->renderObject.flags_9C |= 0x20;
        }
        break;
    case 6:
        player = D_8009D254;
        if (player->animLastFrame != player->animPrev.parts.integer)
            break;
        Entity_SetActionMode(player, D_8009D278->actionMode12);
        D_8009CE74++;
        break;
    case 7:
        core = D_8009D278;
        core->curHP = (s16)core->maxHP >> 1;
        core->exp_or_acc >>= 2;
        Battle_ResetEnemyStats(1);
        restoredCore = D_8009D278;
        player = D_8009D254;
        restoredCore->panelB_val = restoredCore->curHP;
        restoredCore->panelB_x = player->renderObject.projected_x;
        restoredCore->panelB_y = player->renderObject.projected_y;
        restoredCore->panelB_timer = 30;
        D_8009D278->panelB_flag = 1;
        Aya_DeriveStats(&D_8009D278->atbStep, &D_8009D278->atbRate);
        BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[0], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[0].sprite, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[1], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[1].sprite, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
        D_8009CE74 = 0;
        D_8009D28C = 0;
        if (D_8009D2A0 == 0)
            Battle_StartDeathAnim();
        result = 1;
        break;
    }
    return result;
}
