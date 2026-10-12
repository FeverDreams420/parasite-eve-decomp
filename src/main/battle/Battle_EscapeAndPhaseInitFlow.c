/* CC1_FLAGS: -G8 */
/* MASPSX_FLAGS: -G8 --use-comm-section */
/* MASPSX_FORCE_G0: 1 */
#include "pe1/battle_runtime.h"
#include "pe1/battle_turn.h"

/* COMMON supplies small-data metadata to MASPSX; the other battle words
 * stay absolute. */
u8 D_8009CE74;
u8 D_8009D244;
Combatant *D_8009D278;
s32 D_8009D28C;


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
    Tbl_ResetAll();
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
