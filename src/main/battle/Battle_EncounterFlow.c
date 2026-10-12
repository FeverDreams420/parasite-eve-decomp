/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */

#include "common.h"
#include "pe1/battle_runtime.h"
#include "pe1/battle_reset_stats.h"
#include "pe1/battle_palette.h"
#include "pe1/battle.h"
#include "pe1/battle_start.h"
#include "pe1/battle_status.h"
#include "pe1/battle_update.h"
#include "pe1/battle_entity_anim.h"
#include "pe1/aya.h"
#include "pe1/battle_level_up.h"
#include "pe1/game_state.h"
#include "pe1/akao/commands.h"
#include "pe1/battle_turn.h"
#include "pe1/battle_modifiers.h"
#include "pe1/battle_saved_state.h"

/* COMMON supplies small-data metadata to MASPSX: these words are reached
 * through $gp, every other extern stays absolute. */
u8 D_8009CE7C;
s32 D_8009D1D0;
s32 D_8009D1E8;
u8 g_BattleActionActorCount;
u8 D_8009D244;
Combatant *D_8009D278;
u8 D_8009D288;
int D_8009D28C;
s8 D_8009D2A0;
s16 D_8009D2A4;
s8 D_8009D2B0;
s32 D_8009D2FC;
u8 D_8009CE74;
u32 D_8009D304;
u16 D_8009D21C;
s32 D_8009D28C;
BattleAttributes g_BattleEquipStateBlock;

void func_8002F658(void);
void Save_ResetGlobalFlags(void);

void Save_InitSystem(void)
{
    u8 i;

    func_8002F658();
    for (i = 0; i < 7; i++) {
        D_800A5D58[i].active = 0;
    }
    D_8009D2A0 = 0;
    D_8009D2EC = 0;
    Save_ResetGlobalFlags();
}

/* COMMON supplies small-data metadata to MASPSX; the other battle words
 * stay absolute. The story-day flags byte is set through its g_GameState
 * record field: the record is not small data, so its address stays in a
 * register for the read-modify-write, as retail has it. */
u8 D_8009CE74, D_8009CE78, D_8009CE7C;
int D_8009D1D0;
int D_8009D1E8;
u16 D_8009D21C;
u8 D_8009D244;
Combatant *D_8009D278;
u8 D_8009D288;
int D_8009D28C;
s8 D_8009D2A0;
u8 D_8009D2EC;
u32 D_8009D304;

void Battle_ResetEnemyStats(int mode) {
    Combatant *actor;
    Combatant *clear_actor;
    u32 clear_flags;

    actor = D_8009D278;
    if ((s16)actor->maxHP < (s16)actor->curHP) {
        actor->curHP = actor->maxHP;
    }

    D_8009D278->actionMode12 = 4;
    actor = D_8009D278;
    actor->hpAlive = 0;
    actor->atbGauge = 0;
    actor->hpMirror = actor->curHP;
    D_8009D1D0 = 0;

    if ((mode & 0xFF) == 1) {
        D_8009D234[0] = 0x5A;
        D_8009D244 = 1;
        actor->stateFlags |= 0x800000;
    } else {
        u32 state = actor->stateFlags;
        D_8009D244 = 0;
        state &= 0xFFBFFFFF;
        state &= 0xFF7FFFFF;
        actor->stateFlags = state;
    }

    clear_actor = D_8009D278;
    clear_flags = clear_actor->stateFlags;
    clear_actor->panelA_timer = 0;
    clear_flags &= -4;
    clear_flags &= -0xD;
    clear_flags &= -0x31;
    clear_flags &= -0xC1;
    clear_flags &= -0x101;
    clear_flags &= -0x201;
    clear_flags &= -0x401;
    clear_flags &= -0x801;
    clear_flags &= -0x1001;
    clear_flags &= -0x2001;
    clear_flags &= -0x4001;
    clear_flags &= 0xFFFF7FFF;
    clear_flags &= 0xFFF7FFFF;
    clear_flags &= 0xFFFEFFFF;
    clear_flags &= 0xFFF9FFFF;
    clear_flags &= 0xFFEFFFFF;
    clear_flags &= 0xFFDFFFFF;
    clear_flags &= 0xFEFFFFFF;
    clear_flags &= 0xF1FFFFFF;
    clear_flags &= 0xEFFFFFFF;
    clear_flags &= 0xDFFFFFFF;
    clear_actor->stateFlags = clear_flags;

    D_8009D278->panelB_timer = 0;
    D_8009D278->panelAux_timer = 0;
    D_8009D2E8 &= ~0x10;
    Battle_FlushScriptSounds();
    Draw_ResetTextboxes();
    D_8009D298 = 0;
    D_8009D29A = 0;
    D_8009D29B[0] = 0;
    D_8009D29C[0] = 0;
}

/* Reset the PE gauge fill and remainder gradients of both draw buffers. */
void Battle_SetupPlayerPalette(void) {
    u8 *flags;

    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    Battle_FlushScriptSounds();
    D_8009D2EC = 0;
    D_8009D2A0 = 0;
    D_8009D254->actionCheckFn = (void *)D_800915E0[0];
    D_8009D1A0 &= -3;
    D_8009D2E8 &= -0x11;
    flags = &g_GameState.story_day_flags;
    *flags |= 2;
    BattleCmd_SyncActiveAmmo();
}

void Battle_StartEncounter(int mode) {
    int i;
    int neg_one;
    BattleEntity *entity;
    Combatant *actor;
    s32 value;
    int mode_reg;
    u32 initial_flags;
    entity = D_8009D254;
    D_8009D278 = (Combatant *)entity->core;
    D_8009D1E8 = 0;
    *(u32 *)&D_8009D290 = 0;
    D_8009D28C = 0;
    D_8009CE7C = 0;
    D_8009CE78 = 0;
    D_8009D288 = 0;
    D_8009CE74 = 0;
    mode_reg = mode;
    Save_ResetGlobalFlags();

    i = 0;
    neg_one = -1;
    *(u8 *)&D_8009D1AC = 0;
    initial_flags = D_8009D1AC;
    D_8009D1A8.summary = 0;
    D_8009D1CE = 0;
    D_8009D235[0] = 0;
    D_8009D304 = 0;
    D_8009D21C = 0;
    initial_flags &= -0x301;
    D_8009D1AC = initial_flags;

    for (; (u8)i < 0xA; i++) {
        D_800A7FF0[(u8)i].id = 0;
        D_800A7FF0[(u8)i].extra = neg_one;
    }

    for (i = 0; (u8)i < 7; i++) {
        g_BattleActionActors[(u8)i] = 0;
    }

    srand(D_8009D250);
    Battle_ResetEnemyStats(0);
    Battle_SetupEnemyAnims();

    actor = D_8009D278;
    value = actor->exp_or_acc;
    if (value <= 0) {
        actor->exp_or_acc = 0x10000;
    } else {
        s32 max_value;

        max_value = actor->maxAtk;
        if (value >= max_value) {
            actor->exp_or_acc = max_value;
            actor->atbGauge = 0xF0;
        }
    }

    Battle_CheckDropChance();
    {
        BattleEntity *post_entity;
        RenderMatrix *post_actor;
        void *callback;
        post_entity = D_8009D254;
        post_actor = post_entity->renderObject.matrices;
        callback = Entity_CheckActionIdMatch;
        post_entity->actionCheckFn = callback;
        D_8009D27C = post_actor->translation[1] - 0x64;
    }
    Window_SetBoundsByMode(mode_reg & 0xFF);
    {
        Combatant *final_actor;
        void *final_entity;
        final_actor = D_8009D278;
        final_entity = D_8009D254;
        Entity_SetActionMode(final_entity, final_actor->actionMode12);
    }
}

#define BATTLE_HUD_SLOT_BYTE(symbol, stride) \
    (*(&(symbol)[0] +(D_8009CDDC * (stride))))

void Battle_Update(void) {
    /* Retail reserves 0x1A0 unused bytes; original local layout is unknown. */
    volatile u8 matchingStackReserve[0x1A0];
    s32 menuMode;
    BattleEntity *player;
    Combatant *active;
    s8 menuResult;
    int phase;
    int opened;
    u8 previousPhase;
    Combatant *status;
    BattleEntity *entity;
    BattleEntity *victoryEntity;

    menuMode = 1;
    if (D_8009D278->stateFlags & 0x80000) {
        if (D_8009D28C == 6) {
            D_8009CE7C = 6;
            D_8009D28C = 0;
        }
    }
    if (!(D_8009D278->stateFlags & 0x80000)) {
        previousPhase = D_8009CE7C;
        if (previousPhase == 6) {
            D_8009CE7C = 0;
            D_8009D28C = previousPhase;
        }
    }
    active = D_8009D254->core;
    g_BattleStatusPanelSlot = 0;
    D_8009D278 = active;
    D_8009D2A4 = Menu_RunFrameWithArg((int)&D_800A76D8);
    if (D_8009D28C == 0 && D_8009D244 != 0) {
        if ((u16) D_8009D278->hpAlive >= 0x2328U) {
            D_8009D278->hpAlive = 0x2328U;
            if (D_8009D288 == 0) {
                srand(D_8009D250);
                if ((D_800B0E08[0]) != 0) {
                    Akao_SendTableCommand((D_800B0E08[0]), 0x454, 0, 0x80, 0x7F);
                }
                D_8009D288 = 1;
            }
            if (!(D_8009D278->stateFlags & 0x2000) && (D_8009D2A0 != 0)) {
                if ((D_8009D250 & 3) == 0) {
                    register int topGreen asm("$8") = 0x46;
                    register int topBlue asm("$7") = 0x82;
                    register int bottomRed asm("$6") = 0x9F;
                    int bottomGreen = 0xFF;
                    int bottomBlue = 0xF9;
                    BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
                    asm volatile("" : : "r"(bottomBlue));
                    BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = bottomRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = bottomRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = bottomRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
                    asm("" : : "i"(0));
                    BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
                } else if ((D_8009D250 & 3) == 1) {
                    int red = 0x50;
                    register int green asm("$6") = 0xA3;
                    int blue = 0xBE;
                    BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
                    asm volatile("" : : "r"(blue));
                    BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
                    asm("" : : "i"(1));
                    BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
                } else if ((D_8009D250 & 3) == 2) {
                    int topRed = 0x9F;
                    register int topGreen asm("$8") = 0xFF;
                    int topBlue = 0xF9;
                    int bottomGreen = 0x46;
                    int bottomBlue = 0x82;
                    BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = topRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
                    asm volatile("" : : "r"(bottomBlue));
                    BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = topRed;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
                    BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
                    BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = 0;
                    BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
                    asm("" : : "i"(2));
                    BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
                } else if ((D_8009D250 & 3) == 3) {
                    int red = 0x50;
                    register int green asm("$6") = 0xA3;
                    int blue = 0xBE;
                    BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
                    asm volatile("" : : "r"(blue));
                    BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
                    BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
                    BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
                    BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
                }
                if ((Pad_GetMenuPressedBitOrDisabled() << 0x18) == 0) {
                    opened = 0;
                    if (D_8009D1F4 & 0x200) {
                        (D_8009D1F0[0]) = 0;
                        opened = 1;
                    } else if (D_8009D1F4 & 0x80) {
                        opened = 1;
                        (D_8009D1F0[0]) = opened;
                        Inventory_OpenAyaItemList(1);
                        Render_BeginSceneLoad();
                    }
                    if ((u8)opened != 0) {
                        Akao_Cmd_21(0, 0xFF);
                        D_8009D28C = 1;
                        (D_8009D290[0]) = 0;
                        D_8009D1A0 = D_8009D1A0 | 4;
                        menuMode = 0;
                        D_8009D2B0 = Battle_BuildTargetList();
                        Battle_Init();
                        if (D_8009D2B0 != 0) {
                            Menu_SetItemContext(0);
                        } else {
                            Menu_SetItemContext(1);
                        }
                    }
                }
            }
        }
        if ((Pad_GetMenuPressedBitOrDisabled() << 0x18) > 0) {
            if (!(D_8009D278->stateFlags & 0x4000) || (D_8009D1A0 & 0x100)) {
                menuMode = 2;
                Battle_StepScriptEntry();
            }
            if ((D_8009D294) != 0) {
                Battle_ResolveHitOnTimer();
            }
        }
        if ((D_8009D1AC) & 0x300) {
            Save_DrawSlotMetadata();
        }
        Menu_MainUpdate(menuMode);
        entity = (BattleEntity *)D_8009D20C;
        if (entity != 0) {
            do {
                if ((entity != D_8009D254) && (entity->core != 0)) {
                    Battle_UpdateEnemy(entity);
                }
                entity = entity->next;
            } while (entity != 0);
        }
        if (g_BattleActionActorCount != 0) {
            Pm_SendCmd(D_8009D2FC, 0, 1, (int)&(g_BattleActionActors[0]), 0, 0);
            Pm_SendCmd(D_8009D2FC, 0, 0, 1, 0, 0);
            g_BattleActionActorCount = 0;
        }
        if ((D_8009D235[0]) != 0) {
            Gpu_QueuePrimitive();
        }
        player = D_8009D254;
        if ((player->animLastFrame == player->animPrev.parts.integer) && ((u8)player->actionMode < 4U)) {
            if (D_8009D298 == 0) {
                Entity_SetActionMode(player, D_8009D278->actionMode12);
                if (!(D_800B0CE6 & 1) && ((Pad_GetMenuPressedBitOrDisabled() << 0x18) == 0) && (D_8009D28C == 0) && (D_8009D278->action->turnWord & 0x8000) && (D_8009D1D0 != 0)) {
                    Battle_RollEnemySpawn(D_8009D1D0);
                    D_8009D1D0 = 0;
                }
            } else {
                if (D_8009D298 >= 2U) {
                    player->entityFlags |= 0x100;
                }
                D_8009D298 = 0;
                Entity_SetActionMode(D_8009D254, D_8009D29A);
                player = D_8009D254;
                player->animFrame = D_8009D29C[0];
                player->animPrev.fixed = D_8009D29C[0] + 0xFFFF0000;
            }
        }
        if ((D_8009D254->actionMode == 0xD) && !(D_800B0CE6 & 1)) {
            D_8009D278->actionMode12 = 4U;
            if (D_8009D254->entityFlags & 0x100) {
                Asset_Find08Alt(0x453, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
                D_8009D254->entityFlags = (D_8009D254->entityFlags & ~0x100);
            }
            player = D_8009D254;
            if (player->animLastFrame == player->animPrev.parts.integer) {
                Entity_SetActionMode(player, D_8009D278->actionMode12);
                D_8009D2E8 = D_8009D2E8 & ~1;
            }
        }
        D_8009D1E8 += 1;
    } else {
        phase = D_8009D28C;
        if (phase == 1) {
            menuResult = Battle_HandleItemMenu();
            D_8009D28C = menuResult;
            if (menuResult == 0) {
                D_8009D1A0 = D_8009D1A0 & ~4;
            }

        } else if (phase == 2) {
            Battle_StepLevelUp();

        } else if (phase == 3) {
            if (D_8009D278->stateFlags & 0x800) {
                if (Battle_StepVictory() & 0xFF) {
                    D_8009D278->stateFlags = (D_8009D278->stateFlags & ~0x800);
                }
            } else if (Inv_CountByValue(0x12) != 0) {
                if (Battle_StepVictory() & 0xFF) {
                    Inv_FindItemById(0x12);
                }
            } else {
                Battle_StepPostBattle();
            }
            status = D_8009D278;
            if (status->panelA_timer != 0) {
                Battle_DrawStatusPanel(0, (BattleStatusPanel *)&status->panelA_val);
                D_8009D278->panelA_timer--;
                status = D_8009D278;
            }
            if (status->panelAux_timer != 0) {
                Battle_DrawStatusPanel(0, (BattleStatusPanel *)&status->panelAux_val);
                D_8009D278->panelAux_timer = (u8) (D_8009D278->panelAux_timer - 1);
            }
            victoryEntity = (BattleEntity *)D_8009D20C;
            if (victoryEntity != 0) {
                do {
                    if ((victoryEntity != D_8009D254) && (victoryEntity->core != 0)) {
                        Battle_UpdateEnemy(victoryEntity);
                    }
                    victoryEntity = victoryEntity->next;
                } while (victoryEntity != 0);
            }

        } else if (phase == 4) {
            Battle_StepEscapeOrDeath();

        } else if (phase == 5) {
            Battle_StepEntityDeath();

        } else if (phase == 6) {
            Battle_PhaseInitEnemyTurn();

        } else if (phase == 7) {
            Battle_PhaseHitReaction();

        } else if (phase == 8) {
            Battle_PhaseEndTurn();

        }
    }
    if (((D_8009D1CE) != 0) && (D_8009D28C == 0)) {
        Menu_SaveOverlayDraw();
    }
    if (D_8009D244 != 0) {
        Battle_DrawActiveStatus();
    }
    if (D_8009D2A4 != 0) {
        Render_BeginSceneLoad();
    }
}
#undef BATTLE_HUD_SLOT_BYTE

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

void Battle_StepLevelUp(void) {
    u8 step;
    BattleEntity *entity;
    u32 flags;

    step = D_8009CE74;
    switch (step) {
    case 0:
        entity = D_8009D254;
        if ((u16)(entity->animFrame >> 16) != 0xA && (D_8009D1A0 & 0x800) == 0) {
            break;
        }
        {
            u32 entity_flags;
            entity_flags = entity->entityFlags;
            entity->entityFlags = entity_flags | 0x100;
        }
        Pm_StopAllBoth();
        Aya_SetTotalExp(D_8009D304, D_8009D21C, D_800A7FF0);
        Render_BeginSceneLoad();
        D_8009CE74++;
        break;

    case 1:
        if (D_8009D2A4 == 0x3E8) {
            entity = D_8009D254;
            D_8009CE74 = step + 1;
            entity->entityFlags &= -0x101;
        }
        break;

    case 2:
        entity = D_8009D254;
        if (entity->animLastFrame == entity->animPrev.parts.integer) {
            flags = D_8009D1A0;
            if ((flags & 0x1800) == 0) {
                Entity_SetActionMode(entity, 0x15);
            } else {
                u32 cleared_flags;
                cleared_flags = flags & -0x1801;
                D_8009D1A0 = cleared_flags;
                Entity_SetActionMode(entity, 0x18);
            }
            D_8009CE74++;
        }
        break;

    case 3:
        if (CD_StepReadState(0) != 1) {
            Battle_SetupPlayerPalette();
            D_8009D28C = 9;
            g_GameState.flags &= 0xFFFF7FFF;
        }
        break;
    }
}

/* COMMON metadata preserves GP-relative accesses; retail symbols supply storage. */
u8 g_BattleEndTimer, D_8009CE74;
s32 D_8009D28C;

/* End-of-battle sequence: retire enemy actors, fade sound and animate the
 * textured panel before completing the battle phase. Matching debt: register
 * pins, empty barriers, a volatile packet view and 24 reserved stack bytes
 * in the other cases; case 3 sets the corners through a quad pointer.
 * Countdown and stage stores remain local to their switch cases. */
void Battle_StepPostBattle(void)
{
    volatile u8 matchingStackReserve[24];
    BattleEntity *player;
    BattleEntity *entity;
    RenderTexturedQuad *expandingPanel;
    volatile RenderTexturedQuad *fadingPanel;
    s32 remainingFrames;
    register s32 horizontalInset asm("$7");
    s8 brightness;
    register u16 horizontalEdge asm("$4");
    u16 rightEdge;
    u32 entityFlags;
    u8 fadeBrightness;
    u8 countdown;
    EnemyCombatant *core;
    EnemyCombatant *actionCore;
    EnemyCombatant *fadingCore;

    switch (D_8009CE74) {
    case 0:
        player = D_8009D254;
        if (player->actionMode != 0x13) {
            Entity_SetActionMode(player, 0x13);
            player = D_8009D254;
        }
        if (player->animLastFrame == (u16)(player->animFrame >> 16)) {
            entity = (BattleEntity *)D_8009D20C;
            if (entity != 0) {
                do {
                        if (entity != D_8009D254) {
                            core = entity->core;
                            if ((core->field04.bytes.kind != 1) && ((core != 0) || !(entity->entityFlags & 0x40))) {
                                entity->renderObject.flags_9C |= 2;
                                actionCore = entity->core;
                                entity->entityFlags |= 0x1000;
                                if (actionCore != 0) {
                                    Entity_SetActionMode(entity, (u16)(s8)actionCore->lowAndEntityId.bytes.low);
                                    entityFlags = entity->entityFlags;
                                    if ((entityFlags & 0x40000000) && (((EnemyCombatant *)entity->core)->deathPersist == 0)) {
                                        entity->entityFlags = entityFlags | 0x10;
                                        entity->core = 0;
                                    }
                                }
                                entity->motionX = 0;
                                entity->motionY = 0;
                                entity->motionZ = 0;
                            }
                        }
                        entity = entity->next;
                    } while (entity != 0);
                }
                Battle_ResetEnemyStats(0);
                g_BattleEndTimer = 0x46;
                D_8009CE74 += 1;
                D_8009D254->entityFlags |= 0x100;
            }
            else {
                player->entityFlags &= ~0x100;
            }
            break;
        case 1: {
            s32 exemptKind;
            if (g_BattleEndTimer == 0x3C) {
                Render_StartFadeIn(0x3C);
                Akao_Cmd_C1_WithSlot(0, 0x3C, 0);
                exemptKind = 1;
                entity = (BattleEntity *)D_8009D20C;
                if (entity != 0) {
                    do {
                    if (entity != D_8009D254) {
                        fadingCore = entity->core;
                        if ((fadingCore->field04.bytes.kind != exemptKind) && ((fadingCore != 0) || !(entity->entityFlags & 0x40))) {
                            Anim_SetInterpRate(&entity->renderObject, 0x3C);
                        }
                    }
                    entity = entity->next;
                } while (entity != 0);
            }
        }
        else {
            entity = (BattleEntity *)D_8009D20C;
            if (entity != 0) {
                do {
                    if ((entity != D_8009D254) && ((entity->core != 0) || !(entity->entityFlags & 0x40)) && (entity->renderObject.variant_visible == 0)) {
                        entity->entityFlags |= 0x10;
                    }
                    entity = entity->next;
                } while (entity != 0);
            }
        }
        countdown = g_BattleEndTimer;
        if (countdown == 0) {
            Pm_StopAllBoth();
            Akao_Cmd_21(0, 0xFF);
            g_BattleEndTimer = 0x1E;
            D_8009CE74++;
            break;
        }
        else {
            g_BattleEndTimer--;
        }
        break;
    }
    case 2: {
        register u16 top asm("$5");
        u8 timer;
        register RenderTexturedQuad *base asm("$3");
        s32 frame;
        brightness = 120;
        top = 122;
        asm volatile("" : : "r"(top));
        brightness -= g_BattleEndTimer * 4;
        g_BattleEndPanels[D_8009CDDC].color.bytes.r = brightness;
        g_BattleEndPanels[D_8009CDDC].color.bytes.g = brightness;
        g_BattleEndPanels[D_8009CDDC].color.bytes.b = brightness;
        base = g_BattleEndPanels;
        frame = D_8009CDDC;
        expandingPanel = &base[frame];
        timer = g_BattleEndTimer;
        asm volatile("" : "=r"(expandingPanel) : "0"(expandingPanel));
        remainingFrames = timer & 0xFF;
        horizontalInset = remainingFrames * 4;
        horizontalEdge = 0x64 - horizontalInset;
        rightEdge = ((remainingFrames * 8) + 0xDC) - horizontalInset;
        expandingPanel->x0 = horizontalEdge;
        expandingPanel->x2 = horizontalEdge;
        expandingPanel->y0 = top;
        expandingPanel->x1 = rightEdge;
        expandingPanel->y1 = top;
        expandingPanel->y2 = 0x7C;
        expandingPanel->x3 = rightEdge;
        expandingPanel->y3 = 0x7C;
        if (remainingFrames != 0) {
            {
                u8 nextTimer = timer - 1;
                g_BattleEndTimer = nextTimer;
            }
        }
        else {
            g_BattleEndTimer = 0x50;
            D_8009CE74++;
            break;
        }
        break;
    }
    case 3: {
        if ((u8) g_BattleEndTimer >= 0x1AU) {
            u8 pulse = rcos((g_BattleEndTimer - 16) * 16) * 11U >> 11;
            RenderTexturedQuad *panels = g_BattleEndPanels;
            RenderTexturedQuad *panel = &panels[D_8009CDDC];
            BATTLE_SET_QUAD_XY(*panel, 100, 122 - pulse, 220, 122 - pulse, 100, 124, 220, 124);
        }
        else {
            RenderTexturedQuad *panels = g_BattleEndPanels;
            RenderTexturedQuad *panel = &panels[D_8009CDDC];
            BATTLE_SET_QUAD_XY(*panel, 100, 100, 220, 100, 100, 124, 220, 124);
        }
        countdown = g_BattleEndTimer;
        if (countdown == 0) {
            Anim_SetInterpRate(&D_8009D254->renderObject, 0x3C);
            D_8009D254->renderObject.flags_9C |= 2;
            Anim_SetInterpRate(&D_800B0CEC, 0x3C);
            {
                u8 state = D_8009CE74;
                g_BattleEndTimer = 0x3C;
                state++;
                D_800B0CEC.flags_9C |= 2;
                D_8009CE74 = state;
            }
        }
        else {
            g_BattleEndTimer--;
        }
        break;
    }
    case 4: {
        register RenderTexturedQuad *base asm("$7");
        register u16 right asm("$6");
        if ((D_8009D254->renderObject.variant_visible == 0) && (D_800B0CEC.variant_visible == 0)) {
            D_8009CE74++;
            break;
        }
        base = g_BattleEndPanels;
        right = 220;
        fadeBrightness = g_BattleEndTimer * 2;
        fadingPanel = &base[D_8009CDDC];
        fadingPanel->color.bytes.r = fadeBrightness;
        fadingPanel->x0 = 0x64;
        fadingPanel->y0 = 0x64;
        fadingPanel->y1 = 0x64;
        fadingPanel->x2 = 0x64;
        fadingPanel->x1 = right;
        fadingPanel->y2 = 0x7C;
        fadingPanel->x3 = right;
        fadingPanel->y3 = 0x7C;
        base[D_8009CDDC].color.bytes.g = fadeBrightness;
        base[D_8009CDDC].color.bytes.b = fadeBrightness;
        g_BattleEndTimer--;
        break;
    }
    case 5:
        Battle_SetupPlayerPalette();
        D_8009D28C = -1;
        Sys_Shutdown();
        break;
    }
    AddPrim((u32 *)(D_800B0E38.ordering[D_8009CDDC] + 4),
    (u32 *)&g_BattleEndPanels[D_8009CDDC]);
}

void Battle_StepEscapeOrDeath(void)
{
    BattleEntity *entity;

    switch (D_8009CE74) {
    case 0:
        D_8009D244 = 0;
        Pm_StopUpperHalf();
        for (entity = (BattleEntity *)D_8009D20C; entity != 0; entity = entity->next) {
            if (entity->core == 0) {
                if (entity->parent == 0) continue;
            } else if (entity == D_8009D254) {
                Anim_SetInterpRate(&D_800B0CEC, 30);
                D_800B0D88_write.value = D_800B0D88_read.value | 2;
                D_8009D254->renderObject.flags_9C |= 2;
            } else {
                u32 flags;
                Entity_SetActionMode(entity,
                    (u16)(s8)((EnemyCombatant *)entity->core)->lowAndEntityId.bytes.low);
                flags = entity->entityFlags;
                entity->motionX = 0;
                entity->motionY = 0;
                entity->motionZ = 0;
                entity->entityFlags = flags | 0x1000;
                asm volatile("" : : : "memory");
                if ((entity->entityFlags & 0x40000000) &&
                    !((EnemyCombatant *)entity->core)->deathPersist) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                }
            }
            Anim_SetInterpRate(&entity->renderObject, 30);
        }
        D_8009CE74++;
        break;
    case 1:
        if (D_8009D254->renderObject.variant_visible != 0 ||
            D_800B0D8A != 0) break;
        Battle_ResetEnemyStats(0);
        D_8009CE74++;
        entity = (BattleEntity *)D_8009D20C;
        if (entity != 0) {
            BattleEntity *player = D_8009D254;
            do {
                if (entity == player) continue;
                if (entity->core != 0 || entity->parent != 0)
                    entity->renderObject.flags_9C |= 2;
            } while ((entity = entity->next) != 0);
        }
        break;
    case 2: {
        int all_done = 1;
        entity = (BattleEntity *)D_8009D20C;
        if (entity != 0) {
            BattleEntity *player = D_8009D254;
            do {
                if (entity == player) continue;
                if (entity->core == 0 && entity->parent == 0) continue;
                if (entity->renderObject.variant_visible == 0) {
                    entity->core = 0;
                    entity->entityFlags |= 0x10;
                } else if (entity->entityFlags & 0x40) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                } else {
                    all_done = 0;
                }
            } while ((entity = entity->next) != 0);
        }
        if ((u8)all_done) {
            Akao_Cmd_21(0, 255);
            Pm_StopAllBoth();
            D_8009CE74++;
        }
        break;
    }
    case 3:
        if (CD_StepReadState(0) != 1) {
            Battle_ClearMotionTable();
            Entity_SetActionMode(D_8009D254, 0x15);
            Battle_SetupPlayerPalette();
            D_8009D28C = 10;
        }
        break;
    }
}

#define BATTLE_HUD_BYTE(symbol) ((symbol)[0])
#define BATTLE_HUD_SLOT_BYTE(symbol, stride) \
    (*(&BATTLE_HUD_BYTE(symbol) + (D_8009CDDC * (stride))))

/* Initialize the enemy turn and wait for HUD, animation and texture work. */
void Battle_PhaseInitEnemyTurn(void) {
    /* Retail reserves this unused local area; its original purpose is unknown. */
    volatile u8 matchingStackReserve[0x300];
    s32 entityFlags;
    u8 ready;
    Combatant *actor;
    EnemyCombatant *enemy;
    BattleEntity *entity;
    BattleEntity *player;
    Combatant *active;
    BattleEntity *initialPlayer;
    register s32 moveLock asm("$3");

    u32 hp;
    s32 gameFlags;
    s32 clearMask;
    /* Clear prior script effects and stop the player before waiting for actors. */
    Battle_FlushScriptSounds();
    ready = 1;
    Draw_ResetTextboxes();
    moveLock = D_8009D2E8 | 1;
    D_8009D2E8 = moveLock;
    initialPlayer = D_8009D254;
    active = D_8009D278;
    clearMask = ~4;
    initialPlayer->motionX = 0;
    initialPlayer->motionY = 0;
    initialPlayer->motionZ = 0;
    gameFlags = D_8009D1A0;
    hp = active->hpAlive;
    D_8009D1A0 = gameFlags & clearMask;
    /* Animate the charged gauge and status highlight using the draw slot. */
    if (hp >= 0x2328U) {
        if ((D_8009D250 & 3) == 0) {
            register int topGreen asm("$8") = 0x46;
            register int topBlue asm("$7") = 0x82;
            register int bottomRed asm("$6") = 0x9F;
            register int bottomGreen asm("$5") = 0xFF;
            int bottomBlue = 0xF9;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
            asm volatile("" : : "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
            asm("" : : "i"(0));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
        } else if ((D_8009D250 & 3) == 1) {
            register int red asm("$5") = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
            asm("" : : "i"(1));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
        } else if ((D_8009D250 & 3) == 2) {
            register int topRed asm("$7") = 0x9F;
            register int topGreen asm("$8") = 0xFF;
            register int topBlue asm("$6") = 0xF9;
            register int bottomGreen asm("$5") = 0x46;
            int bottomBlue = 0x82;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
            asm("" : : "i"(2));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
        } else if ((D_8009D250 & 3) == 3) {
            int red = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
        }
    }
    if (D_8009D278->stateFlags & 0x2000) {
        if ((D_8009D250 & 3) == 0) {
            register int topRed asm("$9") = 0xFF;
            register int topGreen asm("$8") = 0x3D;
            register int topBlue asm("$7") = 0x81;
            register int bottomRed asm("$6") = 0x83;
            register int bottomGreen asm("$5") = 0x13;
            int bottomBlue = 1;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = topRed;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = bottomGreen;
            asm("" : : "i"(0));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = bottomBlue;
        } else if ((D_8009D250 & 3) == 1) {
            register int red asm("$6") = 0xC1;
            register int green asm("$5") = 0x28;
            int blue = 0x41;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = red;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = green;
            asm("" : : "i"(1));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = blue;
        } else if ((D_8009D250 & 3) == 2) {
            register int topRed asm("$9") = 0x83;
            register int topGreen asm("$8") = 0x13;
            register int topBlue asm("$7") = 1;
            register int bottomRed asm("$6") = 0xFF;
            register int bottomGreen asm("$5") = 0x3D;
            int bottomBlue = 0x81;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = topRed;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = bottomGreen;
            asm("" : : "i"(2));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = bottomBlue;
        } else if ((D_8009D250 & 3) == 3) {
            register int red asm("$6") = 0xC1;
            register int green asm("$5") = 0x28;
            int blue = 0x41;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = red;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = blue;
        }
    }
    /* Floating status panels must finish before the next phase. */
    actor = D_8009D278;
    if (actor->panelA_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&actor->panelA_val);
        ready = 0;
        D_8009D278->panelA_timer = (u8) (D_8009D278->panelA_timer - 1);
    }
    actor = D_8009D278;
    if (actor->panelB_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&actor->panelB_val);
        ready = 0;
        D_8009D278->panelB_timer = (u8) (D_8009D278->panelB_timer - 1);
    }
    if (D_8009D278->panelAux_timer != 0) {
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&D_8009D278->panelAux_val);
        ready = 0;
        D_8009D278->panelAux_timer = (u8) (D_8009D278->panelAux_timer - 1);
    }
    /* Wait for player/enemy animations to reach their requested idle modes. */
    entity = (BattleEntity *)D_8009D20C;
    if (entity != 0) {
        do {
            if (entity == D_8009D254) {
                if (entity->actionMode != D_8009D278->actionMode12) {
                    ready = 0;
                    entityFlags = entity->entityFlags & ~0x100;
                    entity->entityFlags = entityFlags;
                    if (entity->animLastFrame == entity->animPrev.parts.integer) {
                        if (((u8) entity->actionMode < 4U) && (D_8009D298 != 0)) {
                            if (D_8009D298 >= 2U) {
                                entity->entityFlags = (entityFlags | 0x100);
                            }
                            D_8009D298 = 0;
                            Entity_SetActionMode(D_8009D254, D_8009D29A);
                            player = D_8009D254;
                            player->animFrame = (D_8009D29C[0]);
                            player->animPrev.fixed = (D_8009D29C[0] + 0xFFFF0000);
                        } else {
                            Entity_SetActionMode(entity, D_8009D278->actionMode12);
                        }
                    }
                }
            } else {
                enemy = entity->core;
                if ((enemy != 0) && (enemy->field04.bytes.kind != 1)) {
                    if ((entity->actionMode - 2) >= 2U) {
                        entity->entityFlags = (entity->entityFlags & ~0x1000);
                        Entity_TickAnimSequences(entity);
                        ready = 0;
                        if (entity->animLastFrame == entity->animPrev.parts.integer) {
                            Entity_SetActionMode(entity, (u16)(s8)((EnemyCombatant *)entity->core)->lowAndEntityId.bytes.low);
                        } else if (entity->animStep != 0x10000) {
                            entity->animStep = 0x10000;
                        }
                    }
                    Battle_UpdateEnemy(entity);
                }
            }
            entity = entity->next;
        } while (entity != 0);
    }
    /* Texture uploads can keep the phase pending after animations settle. */
    if (Asset_LoadTimTextures(0) != 0) {
        ready = 0;
    }
    if ((u8)ready != 0) {
        Akao_Cmd_21(0, 0xFF);
        {
            Combatant *finisher = D_8009D278;
            finisher->knockbackDistance = 0;
            finisher->knockbackFrames = 0;
        }
        D_8009D28C = 7;
        D_8009D2E8 &= ~1;
        BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[0], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[0].sprite, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[1], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[1].sprite, 0x9F, 0xFF, 0xF9);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
        BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    }
}

void Battle_PhaseHitReaction(void) {
    volatile u8 matchingStackReserve[0x1A0];
    Combatant *active;
    BattleEntity *player;
    BattleEntity *entity;
    EnemyCombatant *enemy;
    if (D_8009D278->hpAlive >= 0x2328U) {
        if ((D_8009D250 & 3) == 0) {
            register int topGreen asm("$8") = 0x46;
            register int topBlue asm("$7") = 0x82;
            register int bottomRed asm("$6") = 0x9F;
            register int bottomGreen asm("$5") = 0xFF;
            int bottomBlue = 0xF9;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
            asm volatile("" : : "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
            asm("" : : "i"(0));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
        } else if ((D_8009D250 & 3) == 1) {
            register int red asm("$5") = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
            asm("" : : "i"(1));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
        } else if ((D_8009D250 & 3) == 2) {
            register int topRed asm("$7") = 0x9F;
            register int topGreen asm("$8") = 0xFF;
            register int topBlue asm("$6") = 0xF9;
            register int bottomGreen asm("$5") = 0x46;
            int bottomBlue = 0x82;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
            asm("" : : "i"(2));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
        } else if ((D_8009D250 & 3) == 3) {
            int red = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
        }
    }
    active = D_8009D278;
    if (active->panelA_timer) {
        if (active->panelA_timer == 30) {
            BattleEntity *panelPlayer = D_8009D254;
            active->panelA_x = panelPlayer->renderObject.projected_x;
            active->panelA_y = panelPlayer->renderObject.projected_y;
        }
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&D_8009D278->panelA_val);
        D_8009D278->panelA_timer--;
    }
    active = D_8009D278;
    if (active->panelB_timer) {
        if (active->panelB_timer == 30) {
            player = D_8009D254;
            active->panelB_x = player->renderObject.projected_x;
            active->panelB_y = player->renderObject.projected_y;
            active->panelB_flag = 1;
        }
        Battle_DrawStatusPanel(0, (BattleStatusPanel *)&D_8009D278->panelB_val);
        D_8009D278->panelB_timer--;
    }
    for (entity = (BattleEntity *)D_8009D20C; entity; entity = entity->next) {
        if (entity != D_8009D254 && entity->core) {
            enemy = entity->core;
            if (enemy->coreFlags & 0x6000) {
                Battle_ProcessActionSlot(entity);
                if ((enemy->coreFlags & 0x6000) == 0x2000)
                    enemy->coreFlags = (enemy->coreFlags & ~0x6000) | 0x4000;
            }
            if (enemy->panelC_timer) {
                if (enemy->panelC_timer == 30) {
                    enemy->panelC_x = entity->renderObject.projected_x;
                    enemy->panelC_y = entity->renderObject.projected_y;
                }
                Battle_DrawStatusPanel(1, (BattleStatusPanel *)&enemy->panelC_val);
                enemy->panelC_timer--;
            }
        }
    }
}

void Battle_PhaseEndTurn(void) {
    /* Retail reserves 0x300 unused bytes; original local layout is unknown. */
    volatile u8 matchingStackReserve[0x300];
    BattleEntity *player;
    Combatant *active;
    BattleEntity *initialPlayer;
    BattleEntity *entity;
    EnemyCombatant *enemy;
    u8 ready;
    u8 mode;
    u8 step;
    u8 nextMode;
    u32 hp;

    initialPlayer = D_8009D254;
    active = D_8009D278;
    initialPlayer->motionX = 0;
    initialPlayer->motionY = 0;
    initialPlayer->motionZ = 0;
    hp = active->hpAlive;
    ready = 1;
    /* Animate the charged gauge and active status colors. */
    if (hp >= 0x2328U) {
        if ((D_8009D250 & 3) == 0) {
            register int topGreen asm("$8") = 0x46;
            register int topBlue asm("$7") = 0x82;
            register int bottomRed asm("$6") = 0x9F;
            register int bottomGreen asm("$5") = 0xFF;
            int bottomBlue = 0xF9;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
            asm volatile("" : : "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
            asm("" : : "i"(0));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
        } else if ((D_8009D250 & 3) == 1) {
            register int red asm("$5") = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
            asm("" : : "i"(1));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
        } else if ((D_8009D250 & 3) == 2) {
            register int topRed asm("$7") = 0x9F;
            register int topGreen asm("$8") = 0xFF;
            register int topBlue asm("$6") = 0xF9;
            register int bottomGreen asm("$5") = 0x46;
            int bottomBlue = 0x82;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = topGreen;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = 0;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = bottomGreen;
            asm("" : : "i"(2));
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = bottomBlue;
        } else if ((D_8009D250 & 3) == 3) {
            int red = 0x50;
            register int green asm("$6") = 0xA3;
            int blue = 0xBE;
            BATTLE_HUD_SLOT_BYTE(D_800B00EC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00ED, 0x24) = green;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B00EE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00F4, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00F5, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00F6, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B00FC, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B00FD, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B00FE, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0104, 0x24) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0105, 0x24) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0106, 0x24) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B692C, 0x1C) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B692D, 0x1C) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B692E, 0x1C) = blue;
        }
    }
    if (D_8009D278->stateFlags & 0x2000) {
        if ((D_8009D250 & 3) == 0) {
            register int topRed asm("$9") = 0xFF;
            register int topGreen asm("$8") = 0x3D;
            register int topBlue asm("$7") = 0x81;
            register int bottomRed asm("$6") = 0x83;
            register int bottomGreen asm("$5") = 0x13;
            int bottomBlue = 1;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = topRed;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = bottomGreen;
            asm("" : : "i"(0));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = bottomBlue;
        } else if ((D_8009D250 & 3) == 1) {
            register int red asm("$6") = 0xC1;
            register int green asm("$5") = 0x28;
            int blue = 0x41;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = red;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = green;
            asm("" : : "i"(1));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = blue;
        } else if ((D_8009D250 & 3) == 2) {
            register int topRed asm("$9") = 0x83;
            register int topGreen asm("$8") = 0x13;
            register int topBlue asm("$7") = 1;
            register int bottomRed asm("$6") = 0xFF;
            register int bottomGreen asm("$5") = 0x3D;
            int bottomBlue = 0x81;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = topRed;
            asm volatile("" : : "r"(topRed), "r"(topGreen), "r"(topBlue), "r"(bottomRed), "r"(bottomGreen), "r"(bottomBlue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = bottomGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = bottomBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = topRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = topGreen;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = topBlue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = bottomRed;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = bottomGreen;
            asm("" : : "i"(2));
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = bottomBlue;
        } else if ((D_8009D250 & 3) == 3) {
            register int red asm("$6") = 0xC1;
            register int green asm("$5") = 0x28;
            int blue = 0x41;
            BATTLE_HUD_SLOT_BYTE(D_800B0158, 0x48) = red;
            asm volatile("" : : "r"(red), "r"(green), "r"(blue));
            BATTLE_HUD_SLOT_BYTE(D_800B0159, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B015A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0160, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0161, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0162, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0168, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0169, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B016A, 0x48) = blue;
            BATTLE_HUD_SLOT_BYTE(D_800B0170, 0x48) = red;
            BATTLE_HUD_SLOT_BYTE(D_800B0171, 0x48) = green;
            BATTLE_HUD_SLOT_BYTE(D_800B0172, 0x48) = blue;
        }
    }
    player = D_8009D254;
    mode = D_8009D278->actionMode12;
    if (player->actionMode != mode) {
        Entity_SetActionMode(player, mode);
    }
    step = D_8009CE74;
    switch (step) {
    case 0:
        /* Wait for enemy actions to finish before stopping effects. */
        entity = (BattleEntity *)D_8009D20C;
        D_8009D2E8 = D_8009D2E8 | 1;
        if (entity != 0) {
            do {
                if (entity != D_8009D254) {
                    enemy = entity->core;
                    if ((enemy != 0) && (enemy->field04.bytes.kind != 1)) {
                        if ((entity->actionMode - 2) >= 2U) {
                            if (entity->animLastFrame == entity->animPrev.parts.integer) {
                                Entity_SetActionMode(entity, (u16)(s8)enemy->lowAndEntityId.bytes.low);
                            } else if (entity->animStep != 0x10000) {
                                entity->animStep = 0x10000;
                            }
                        }
                        if (!(Battle_ProcessActionSlot(entity) & 0xFF)) {
                            ready = 0;
                        }
                    }
                }
                entity = entity->next;
            } while (entity != 0);
        }
        if ((u8)ready != 0) {
            Akao_Cmd_21(0, 0xFF);
            Pm_StopAllBoth();
            D_8009CE74 += 1;
            return;
        }
        return;
    case 1:
        /* Finish the resource read, restore the player and reset HUD colors. */
        if (CD_StepReadState(0) != step) {
            Battle_ClearMotionTable();
            nextMode = 0x18;
            if (!(D_8009D1A0 & 0x1800)) {
                player = D_8009D254;
                nextMode = 0x15;
            } else {
                player = D_8009D254;
                D_8009D1A0 = D_8009D1A0 & ~0x1800;
            }
            Entity_SetActionMode(player, nextMode);
            Battle_SetupPlayerPalette();
            BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[0], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
            BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[0].sprite, 0x9F, 0xFF, 0xF9);
            BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[1], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
            BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[1].sprite, 0x9F, 0xFF, 0xF9);
            BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
            BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
            BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
            BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
            D_8009D28C = 0xC;
            Battle_ResetEnemyStats(0);
        }
        break;
    }
}

/* Finish entity death animations and restore the player for the next phase. */
void Battle_StepEntityDeath(void)
{
    BattleEntity *entity;

    switch (D_8009CE74) {
    case 0:
        D_8009D244 = 0;
        for (entity = (BattleEntity *)D_8009D20C; entity != 0; entity = entity->next) {
            if (entity == D_8009D254) continue;
            if (entity->core == 0) {
                if (entity->parent == 0) continue;
            } else {
                u32 flags;
                Entity_SetActionMode(entity,
                    (u16)(s8)((EnemyCombatant *)entity->core)->lowAndEntityId.bytes.low);
                flags = entity->entityFlags;
                entity->motionX = 0;
                entity->motionY = 0;
                entity->motionZ = 0;
                entity->entityFlags = flags | 0x1000;
                asm volatile("" : : : "memory");
                if ((entity->entityFlags & 0x40000000) &&
                    !((EnemyCombatant *)entity->core)->deathPersist) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                }
            }
            Anim_SetInterpRate(&entity->renderObject, 30);
            entity->renderObject.flags_9C |= 2;
        }
        D_8009CE74++;
        break;
    case 1: {
        int all_done = 1;
        entity = (BattleEntity *)D_8009D20C;
        if (entity != 0) {
            BattleEntity *player = D_8009D254;
            do {
                if (entity == player) continue;
                if (entity->core == 0 && entity->parent == 0) continue;
                if (entity->renderObject.variant_visible == 0) {
                    entity->core = 0;
                    entity->entityFlags |= 0x10;
                } else if (entity->entityFlags & 0x40) {
                    entity->entityFlags |= 0x10;
                    entity->core = 0;
                } else {
                    all_done = 0;
                }
            } while ((entity = entity->next) != 0);
        }
        if ((u8)all_done) {
            Pm_StopAllBoth();
            D_8009CE74++;
        }
        break;
    }
    case 2:
        if (CD_StepReadState(0) != 1) {
            Battle_ClearMotionTable();
            Entity_SetActionMode(D_8009D254, 0x15);
            Battle_SetupPlayerPalette();
            D_8009D28C = 11;
            Battle_ResetEnemyStats(0);
        }
        break;
    }
}

/* Start the player and battle-HUD death sequence after death cleanup. */

/* Start the player death sequence: reset the HUD gauges, stop the player and
 * spawn the death effect unless an escape is in progress. */
void Battle_StartDeathAnim(void) {
    BattleEntity *player;

    Battle_ResetEnemyStats(0);
    BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[0], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
    BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[0].sprite, 0x9F, 0xFF, 0xF9);
    BATTLE_SET_GAUGE_GRADIENT(g_BattleAtbGauges[1], 0, 0x46, 0x82, 0x9F, 0xFF, 0xF9);
    BATTLE_SET_SPRITE_COLOR(g_BattleAtbMarkers[1].sprite, 0x9F, 0xFF, 0xF9);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][0], 0, 0x82, 0x36, 0x4A, 0xFF, 0x3B);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[0][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    BATTLE_SET_GAUGE_GRADIENT(g_BattlePeGauges[1][1], 0xFF, 0x3D, 0x81, 0x83, 0x13, 1);
    D_8009D28C = 2;
    D_8009D2E8 |= 1;
    player = D_8009D254;
    player->motionX = 0;
    player->motionY = 0;
    player->motionZ = 0;
    player->entityFlags &= ~0x100;
    g_GameState.flags |= 0x8000;
    if (!(D_8009D1A0 & 0x1800)) {
        Entity_SetActionMode(player, 0x14);
        Asset_Find08Alt(0x45B, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer,
                        D_8009D254->posZ.parts.integer);
    }
}

#undef BATTLE_HUD_SLOT_BYTE
#undef BATTLE_HUD_BYTE

void Battle_SaveAyaState(void) {
    AyaBattleState aya = D_80010928;
    BattleStateTail tail = D_80010998;
    BattleAttributes attributes;

    attributes.parameterWord.raw = 0;
    attributes.effectFlags = 0;
    g_AyaBattleState = aya;
    g_SavedBattleStateTail = tail;
    g_BattleEquipStateBlock = attributes;
}

void Battle_InitEquipLists(AyaBattleState **out) {
    *out = &g_AyaBattleState;
    g_AyaBattleState.action = &g_SavedBattleStateTail;
    g_AyaBattleState.attributes = &g_BattleEquipStateBlock;
    Inv_RecalcSlotStats();
    Inv_BuildWeaponList(0, &g_AyaBattleState.action->weapon);
    Inv_BuildArmorList(g_AyaBattleState.attributes);
}
