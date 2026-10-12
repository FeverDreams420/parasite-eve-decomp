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

    register s32 value0 asm("$2");
    register s32 value1 asm("$3");
    register s32 brightGreen asm("$4");
    s32 color1;
    s32 blue;
    s32 color2;

    u32 hp;
    s32 gameFlags;
    s32 clearMask;
    register Combatant *finishActor asm("$2");
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
        blue = 0x82;
        color2 = 0x9F;
        brightGreen = 0xFF;
        color1 = 0xF9;
        finishActor = D_8009D278;
        finishActor->knockbackDistance = 0;
        finishActor->knockbackFrames = 0;
        value0 = 7;
        D_8009D28C = value0;
        value0 = 0x46;
        BATTLE_HUD_BYTE(D_800B00ED) = value0;
        BATTLE_HUD_BYTE(D_800B00FD) = value0;
        BATTLE_HUD_BYTE(D_800B0111) = value0;
        BATTLE_HUD_BYTE(D_800B0121) = value0;
        value0 = D_8009D2E8;
        value1 = ~1;
        BATTLE_HUD_BYTE(D_800B00EC) = 0;
        BATTLE_HUD_BYTE(D_800B00EE) = blue;
        BATTLE_HUD_BYTE(D_800B00F4) = color2;
        BATTLE_HUD_BYTE(D_800B00F5) = brightGreen;
        BATTLE_HUD_BYTE(D_800B00F6) = color1;
        BATTLE_HUD_BYTE(D_800B00FC) = 0;
        BATTLE_HUD_BYTE(D_800B00FE) = blue;
        BATTLE_HUD_BYTE(D_800B0104) = color2;
        BATTLE_HUD_BYTE(D_800B0105) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0106) = color1;
        BATTLE_HUD_BYTE(D_800B692C) = color2;
        BATTLE_HUD_BYTE(D_800B692D) = brightGreen;
        BATTLE_HUD_BYTE(D_800B692E) = color1;
        BATTLE_HUD_BYTE(D_800B0110) = 0;
        BATTLE_HUD_BYTE(D_800B0112) = blue;
        BATTLE_HUD_BYTE(D_800B0118) = color2;
        BATTLE_HUD_BYTE(D_800B0119) = brightGreen;
        BATTLE_HUD_BYTE(D_800B011A) = color1;
        BATTLE_HUD_BYTE(D_800B0120) = 0;
        BATTLE_HUD_BYTE(D_800B0122) = blue;
        BATTLE_HUD_BYTE(D_800B0128) = color2;
        BATTLE_HUD_BYTE(D_800B0129) = brightGreen;
        BATTLE_HUD_BYTE(D_800B012A) = color1;
        D_8009D2E8 = value0 & value1;
        BATTLE_HUD_BYTE(D_800B6948) = color2;
        BATTLE_HUD_BYTE(D_800B694A) = color1;

        color1 = 0x36;
        value1 = 0x4A;
        value0 = 0x3B;
        color2 = 0x3D;
        BATTLE_HUD_BYTE(D_800B0135) = blue;
        BATTLE_HUD_BYTE(D_800B0145) = blue;
        BATTLE_HUD_BYTE(D_800B017D) = blue;
        BATTLE_HUD_BYTE(D_800B018D) = blue;
        blue = 0x81;
        BATTLE_HUD_BYTE(D_800B0136) = color1;
        BATTLE_HUD_BYTE(D_800B0146) = color1;
        BATTLE_HUD_BYTE(D_800B017E) = color1;
        BATTLE_HUD_BYTE(D_800B018E) = color1;
        color1 = 0x83;
        BATTLE_HUD_BYTE(D_800B013C) = value1;
        BATTLE_HUD_BYTE(D_800B014C) = value1;
        BATTLE_HUD_BYTE(D_800B0184) = value1;
        BATTLE_HUD_BYTE(D_800B0194) = value1;
        value1 = 0x13;
        BATTLE_HUD_BYTE(D_800B013E) = value0;
        BATTLE_HUD_BYTE(D_800B014E) = value0;
        BATTLE_HUD_BYTE(D_800B0186) = value0;
        BATTLE_HUD_BYTE(D_800B0196) = value0;
        value0 = 1;
        BATTLE_HUD_BYTE(D_800B6949) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0134) = 0;
        BATTLE_HUD_BYTE(D_800B013D) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0144) = 0;
        BATTLE_HUD_BYTE(D_800B014D) = brightGreen;
        BATTLE_HUD_BYTE(D_800B017C) = 0;
        BATTLE_HUD_BYTE(D_800B0185) = brightGreen;
        BATTLE_HUD_BYTE(D_800B018C) = 0;
        BATTLE_HUD_BYTE(D_800B0195) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0158) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0159) = color2;
        BATTLE_HUD_BYTE(D_800B015A) = blue;
        BATTLE_HUD_BYTE(D_800B0160) = color1;
        BATTLE_HUD_BYTE(D_800B0161) = value1;
        BATTLE_HUD_BYTE(D_800B0162) = value0;
        BATTLE_HUD_BYTE(D_800B0168) = brightGreen;
        BATTLE_HUD_BYTE(D_800B0169) = color2;
        BATTLE_HUD_BYTE(D_800B016A) = blue;
        BATTLE_HUD_BYTE(D_800B0170) = color1;
        BATTLE_HUD_BYTE(D_800B0171) = value1;
        BATTLE_HUD_BYTE(D_800B0172) = value0;
        BATTLE_HUD_BYTE(D_800B01A0) = brightGreen;
        BATTLE_HUD_BYTE(D_800B01A1) = color2;
        BATTLE_HUD_BYTE(D_800B01A2) = blue;
        BATTLE_HUD_BYTE(D_800B01A8) = color1;
        BATTLE_HUD_BYTE(D_800B01A9) = value1;
        BATTLE_HUD_BYTE(D_800B01AA) = value0;
        BATTLE_HUD_BYTE(D_800B01B0) = brightGreen;
        BATTLE_HUD_BYTE(D_800B01B1) = color2;
        BATTLE_HUD_BYTE(D_800B01B2) = blue;
        BATTLE_HUD_BYTE(D_800B01B8) = color1;
        BATTLE_HUD_BYTE(D_800B01B9) = value1;
        BATTLE_HUD_BYTE(D_800B01BA) = value0;
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
    register s32 c2 asm("$2");
    s32 c3;
    s32 c4;
    register s32 c5 asm("$6");
    s32 c6;
    register s32 c7 asm("$8");

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
            c6 = 0x46;
            c5 = 0x82;
            c4 = 0x9F;
            c2 = 0xFF;
            c3 = 0xF9;
            BATTLE_HUD_BYTE(D_800B00ED) = c6;
            BATTLE_HUD_BYTE(D_800B00FD) = c6;
            BATTLE_HUD_BYTE(D_800B0111) = c6;
            BATTLE_HUD_BYTE(D_800B0121) = c6;
            c6 = 0x36;
            BATTLE_HUD_BYTE(D_800B00F4) = c4;
            BATTLE_HUD_BYTE(D_800B0104) = c4;
            BATTLE_HUD_BYTE(D_800B692C) = c4;
            BATTLE_HUD_BYTE(D_800B0118) = c4;
            BATTLE_HUD_BYTE(D_800B0128) = c4;
            BATTLE_HUD_BYTE(D_800B6948) = c4;
            c4 = 0x4A;
            BATTLE_HUD_BYTE(D_800B00F6) = c3;
            BATTLE_HUD_BYTE(D_800B0106) = c3;
            BATTLE_HUD_BYTE(D_800B692E) = c3;
            BATTLE_HUD_BYTE(D_800B011A) = c3;
            BATTLE_HUD_BYTE(D_800B012A) = c3;
            BATTLE_HUD_BYTE(D_800B694A) = c3;
            c3 = 0x3B;
            c7 = 0x3D;
            BATTLE_HUD_BYTE(D_800B00EC) = 0;
            BATTLE_HUD_BYTE(D_800B00EE) = c5;
            BATTLE_HUD_BYTE(D_800B00F5) = c2;
            BATTLE_HUD_BYTE(D_800B00FC) = 0;
            BATTLE_HUD_BYTE(D_800B00FE) = c5;
            BATTLE_HUD_BYTE(D_800B0105) = c2;
            BATTLE_HUD_BYTE(D_800B692D) = c2;
            BATTLE_HUD_BYTE(D_800B0110) = 0;
            BATTLE_HUD_BYTE(D_800B0112) = c5;
            BATTLE_HUD_BYTE(D_800B0119) = c2;
            BATTLE_HUD_BYTE(D_800B0120) = 0;
            BATTLE_HUD_BYTE(D_800B0122) = c5;
            BATTLE_HUD_BYTE(D_800B0129) = c2;
            BATTLE_HUD_BYTE(D_800B6949) = c2;
            BATTLE_HUD_BYTE(D_800B0134) = 0;
            BATTLE_HUD_BYTE(D_800B0135) = c5;
            BATTLE_HUD_BYTE(D_800B0136) = c6;
            BATTLE_HUD_BYTE(D_800B013C) = c4;
            BATTLE_HUD_BYTE(D_800B0146) = c6;
            BATTLE_HUD_BYTE(D_800B017E) = c6;
            BATTLE_HUD_BYTE(D_800B018E) = c6;
            c6 = 0x81;
            BATTLE_HUD_BYTE(D_800B0145) = c5;
            BATTLE_HUD_BYTE(D_800B017D) = c5;
            BATTLE_HUD_BYTE(D_800B018D) = c5;
            c5 = 0x83;
            BATTLE_HUD_BYTE(D_800B014C) = c4;
            BATTLE_HUD_BYTE(D_800B0184) = c4;
            BATTLE_HUD_BYTE(D_800B0194) = c4;
            c4 = 0x13;
            BATTLE_HUD_BYTE(D_800B013E) = c3;
            BATTLE_HUD_BYTE(D_800B014E) = c3;
            BATTLE_HUD_BYTE(D_800B0186) = c3;
            BATTLE_HUD_BYTE(D_800B0196) = c3;
            c3 = 1;
            BATTLE_HUD_BYTE(D_800B013D) = c2;
            BATTLE_HUD_BYTE(D_800B0144) = 0;
            BATTLE_HUD_BYTE(D_800B014D) = c2;
            BATTLE_HUD_BYTE(D_800B017C) = 0;
            BATTLE_HUD_BYTE(D_800B0185) = c2;
            BATTLE_HUD_BYTE(D_800B018C) = 0;
            BATTLE_HUD_BYTE(D_800B0195) = c2;
            BATTLE_HUD_BYTE(D_800B0158) = c2;
            BATTLE_HUD_BYTE(D_800B0159) = c7;
            BATTLE_HUD_BYTE(D_800B015A) = c6;
            BATTLE_HUD_BYTE(D_800B0160) = c5;
            BATTLE_HUD_BYTE(D_800B0161) = c4;
            BATTLE_HUD_BYTE(D_800B0162) = c3;
            BATTLE_HUD_BYTE(D_800B0168) = c2;
            BATTLE_HUD_BYTE(D_800B0169) = c7;
            BATTLE_HUD_BYTE(D_800B016A) = c6;
            BATTLE_HUD_BYTE(D_800B0170) = c5;
            BATTLE_HUD_BYTE(D_800B0171) = c4;
            BATTLE_HUD_BYTE(D_800B0172) = c3;
            BATTLE_HUD_BYTE(D_800B01A0) = c2;
            BATTLE_HUD_BYTE(D_800B01A1) = c7;
            BATTLE_HUD_BYTE(D_800B01B0) = c2;
            c2 = 0xC;
            BATTLE_HUD_BYTE(D_800B01A2) = c6;
            BATTLE_HUD_BYTE(D_800B01A8) = c5;
            BATTLE_HUD_BYTE(D_800B01A9) = c4;
            BATTLE_HUD_BYTE(D_800B01AA) = c3;
            BATTLE_HUD_BYTE(D_800B01B1) = c7;
            BATTLE_HUD_BYTE(D_800B01B2) = c6;
            BATTLE_HUD_BYTE(D_800B01B8) = c5;
            BATTLE_HUD_BYTE(D_800B01B9) = c4;
            BATTLE_HUD_BYTE(D_800B01BA) = c3;
            D_8009D28C = c2;
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


void Battle_StartDeathAnim(void) {
    s32 c2;
    s32 c3;
    s32 c4;
    s32 c5;
    s32 c6;
    register s32 c7 asm("$7");
    u8 *p254;
    s32 t98;

    Battle_ResetEnemyStats(0);
    c6 = 0x46;
    c5 = 0x82;
    c4 = 0x9F;
    c2 = 0xFF;
    c3 = 0xF9;
    D_800B00ED[0] = c6;
    D_800B00FD[0] = c6;
    D_800B0111[0] = c6;
    D_800B0121[0] = c6;
    c6 = 0x36;
    D_800B00F4[0] = c4;
    D_800B0104[0] = c4;
    D_800B692C[0] = c4;
    D_800B0118[0] = c4;
    D_800B0128[0] = c4;
    D_800B6948[0] = c4;
    c4 = 0x4A;
    D_800B00F6[0] = c3;
    D_800B0106[0] = c3;
    D_800B692E[0] = c3;
    D_800B011A[0] = c3;
    D_800B012A[0] = c3;
    D_800B694A[0] = c3;
    c3 = 0x3B;
    c7 = 0x3D;
    D_800B00EC[0] = 0;
    D_800B00EE[0] = c5;
    D_800B00F5[0] = c2;
    D_800B00FC[0] = 0;
    D_800B00FE[0] = c5;
    D_800B0105[0] = c2;
    D_800B692D[0] = c2;
    D_800B0110[0] = 0;
    D_800B0112[0] = c5;
    D_800B0119[0] = c2;
    D_800B0120[0] = 0;
    D_800B0122[0] = c5;
    D_800B0129[0] = c2;
    D_800B6949[0] = c2;
    D_800B0134[0] = 0;
    D_800B0135[0] = c5;
    D_800B0136[0] = c6;
    D_800B013C[0] = c4;
    D_800B0146[0] = c6;
    D_800B017E[0] = c6;
    D_800B018E[0] = c6;
    c6 = 0x81;
    D_800B0145[0] = c5;
    D_800B017D[0] = c5;
    D_800B018D[0] = c5;
    c5 = 0x83;
    D_800B014C[0] = c4;
    D_800B0184[0] = c4;
    D_800B0194[0] = c4;
    c4 = 0x13;
    D_800B013E[0] = c3;
    D_800B014E[0] = c3;
    D_800B0186[0] = c3;
    D_800B0196[0] = c3;
    c3 = 1;
    D_800B013D[0] = c2;
    D_800B0144[0] = 0;
    D_800B014D[0] = c2;
    D_800B017C[0] = 0;
    D_800B0185[0] = c2;
    D_800B018C[0] = 0;
    D_800B0195[0] = c2;
    D_800B0158[0] = c2;
    D_800B0159[0] = c7;
    D_800B015A[0] = c6;
    D_800B0160[0] = c5;
    D_800B0161[0] = c4;
    D_800B0162[0] = c3;
    D_800B0168[0] = c2;
    D_800B0169[0] = c7;
    D_800B016A[0] = c6;
    D_800B0170[0] = c5;
    D_800B0171[0] = c4;
    D_800B0172[0] = c3;
    D_800B01A0[0] = c2;
    D_800B01A1[0] = c7;
    D_800B01B0[0] = c2;
    c2 = 2;
    D_8009D28C = c2;
        {
        s32 d2e8;
        d2e8 = D_8009D2E8;
                D_800B01A8[0] = c5;
        D_800B01B8[0] = c5;
        p254 = (u8 *)D_8009D254;
                D_800B01AA[0] = c3;
        D_800B01BA[0] = c3;
        c3 = ~0x100;
                D_800B01A2[0] = c6;
        D_800B01A9[0] = c4;
        D_800B01B1[0] = c7;
        D_800B01B2[0] = c6;
        D_800B01B9[0] = c4;
                D_8009D2E8 = d2e8 | 1;
    }
    t98 = *(s32 *) (p254 + 0x98);
    *(s32 *) (p254 + 0x68) = 0;
    *(s32 *) (p254 + 0x6C) = 0;
    *(s32 *) (p254 + 0x70) = 0;
    *(s32 *) (p254 + 0x98) = t98 & c3;
    g_GameState.flags |= 0x8000;
    if (!(D_8009D1A0 & 0x1800)) {
        Entity_SetActionMode(p254, 0x14);
        {
            u8 *q;
            q = (u8 *)D_8009D254;
            Asset_Find08Alt(0x45B, 0, ((BattleEntity *)q)->posX.parts.integer, ((BattleEntity *)q)->posY.parts.integer, ((BattleEntity *)q)->posZ.parts.integer);
        }
    }
}

#undef BATTLE_HUD_SLOT_BYTE
#undef BATTLE_HUD_BYTE
