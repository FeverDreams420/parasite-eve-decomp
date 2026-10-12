/* CC1_FLAGS: -G0 */
/* MASPSX_FLAGS: -G8 --use-comm-section --expand-div */
/* ASPSX_VERSION: 2.77 */
#include "pe1/battle_runtime.h"

/* COMMON declarations preserve GP size metadata for stock MASPSX;
 * storage is provided by the existing retail data/linker symbols. */
u8 D_8009D25C;
s8 g_AyaAttackStep, g_EvasionStatus, g_EvasionReaction;
s16 g_AyaActionTimer;
u16 g_AyaSavedRotation[2], g_AyaSavedRotationZ;
u8 D_8009D1D4;
int D_8009D258;

/* Advances Aya's 17-state action, saving her pose, visiting the selected
 * targets, and restoring normal animation/visibility at completion.
 * Matching debt: register pins and empty compiler barriers retain
 * retail allocation and scheduling. No instruction ASM or tool patches. */
s32 Battle_StepAyaAction(void)
{
  BattleEntity *targetEntity;
  BattleEntity *effectTarget;
  BattleEntity *entity;
  s16 targetAngle;
  BattleEntity *assetOwner;
  s32 targetDistance;
  s32 slot;
  s32 flags;
  s32 finished;
  s8 attackStep;
  u32 effectId;
  u32 targetEffectId;
  int hitCountMask;
  register s32 transfer asm("$2");
  EnemyCombatant *targetCore;
  finished = 0;
  switch (D_8009D25C)
  {
    case 0:
    {
      register int rate asm("$5");
      int mask;
      RenderObjectEntity *render;
      unsigned entityFlags;
      Scene_InitEntityPlayer(1);
      {
        BattleEntity *player = D_8009D254;
        transfer = player->posX.fixed;
        g_AyaSavedPosX = transfer;
        transfer = player->posY.fixed;
        g_AyaSavedPosY = transfer;
        transfer = player->posZ.fixed;
        g_AyaSavedPosZ = transfer;
        transfer = (u16) player->rotationX;
        g_AyaSavedRotation[0] = transfer;
        rate = 30;
        g_AyaAttackStep = 0;
        g_AyaSavedRotation[1] = (u16) player->facingAngle;
        mask = ~4;
        transfer = (u16) player->rotationZ;
        g_AyaSavedRotationZ = transfer;
        render = &player->renderObject;
        transfer = D_8009D2E8;
        asm volatile("" : : "r"(transfer));
        entityFlags = player->entityFlags;
        transfer &= mask;
        entityFlags |= 0x80;
        asm volatile("" : : "r"(entityFlags));
        D_8009D2E8 = transfer;
        player->entityFlags = entityFlags;
        Anim_SetInterpRate(render, rate);
      }
      D_8009D254->renderObject.flags_9C |= 2;
      Anim_SetInterpRate(&D_800B0CEC, 0x1E);
      D_800B0CEC.flags_9C |= 2;
      Pm_StopLowerHalf();
      D_8009D258 = Scene_LoadRoomAssets(0x6BU, D_8009D254);
      D_8009D25C += 1;
      break;
    }

    case 1:
      Scene_InitEntityPlayer(1);
      if (D_8009D254->renderObject.variant_visible == 0)
    {
      if (D_800B0CEC.variant_visible == 0)
      {
        D_8009D25C++;
        break;
      }
    }
      break;

    case 2:
      if (Scene_InitEntityPlayer(1) == 0)
    {
      Entity_SetActionMode(D_8009D254, 5);
      {
        BattleEntity *player = D_8009D254;
        player->renderObject.variant_visible = 1;
        player->entityFlags |= 0x100;
        Anim_SetInterpRate(&D_8009D254->renderObject, 0x1E);
      }
      D_8009D254->renderObject.flags_9C |= 4;
      assetOwner = D_8009D254;
      Scene_LoadRoomAssets(0x6CU, assetOwner);
      g_AyaActionTimer = 0x1E;
      D_8009D25C++;
      break;
    }
      break;

    case 3:
      if (g_AyaActionTimer == 0)
    {
      D_8009D25C += 1;
      D_8009D254->entityFlags &= ~0x100;
    }
    else
    {
      g_AyaActionTimer -= 1;
    }

    /* State 3 also waits for the animation handled by state 4. */
    case 4:
      if (D_8009D254->animLastFrame == (u16)(D_8009D254->animFrame >> 16))
    {
      entity = (BattleEntity *)D_8009D20C;
      if (entity != 0)
      {
        do
        {
          if ((entity != D_8009D254) && (entity->core != 0))
          {
            entity->motionX = 0;
            entity->motionY = 0;
            entity->motionZ = 0;
          }
          entity = entity->next;
        }
        while (entity != 0);
      }
      Pm_Stop(D_8009D258, D_8009D254, 0);
      effectId = 0x6D;
      Scene_LoadRoomAssets(effectId, D_8009D254);
      D_8009D25C += 1;
      break;
    }

    default:
      break;

    case 5:
      Entity_SetActionMode(D_8009D254, 6);
      Anim_SetInterpRate(&D_8009D254->renderObject, 0xF);
      D_8009D25C += 1;
      D_8009D254->renderObject.flags_9C |= 2;
      break;

    case 6:
      if (D_8009D254->renderObject.variant_visible == 0)
    {
      {
        s32 product;
        int reachScale;
        targetEntity = D_800BE830[D_8009D1D4].actor;
        reachScale = (((EnemyCombatant *) targetEntity->core)->statusFlags2 >> 0x13) & 0x1F;
        targetDistance = ((targetEntity->renderObject.hit_cylinder.radius * reachScale) / 10) + D_8009D254->renderObject.hit_cylinder.radius;
        targetAngle = Battle_CalcAngleToTarget(&targetEntity->renderObject, &g_AyaSavedPosX);
        D_8009D254->facingAngle = targetAngle;
        product = targetDistance * rsin(targetAngle);
        asm("" : "=r"(product) : "0"(product));
        D_8009D254->posX.fixed = (D_800BE830[D_8009D1D4].actor->renderObject.target_x << 16) + (product * 16);
        product = targetDistance * rcos(D_8009D254->facingAngle);
        D_8009D254->posZ.fixed = (D_800BE830[D_8009D1D4].actor->renderObject.target_z << 16) + (product * 16);
      }
      D_8009D254->baseX = D_8009D254->posX.fixed;
      D_8009D254->baseY = D_8009D254->posY.fixed;
      D_8009D254->baseZ = D_8009D254->posZ.fixed;
      Asset_Find08Alt(0x4B6, 0, D_8009D254->posX.parts.integer, D_8009D254->posY.parts.integer, D_8009D254->posZ.parts.integer);
      g_AyaActionTimer = 0x1E;
      D_8009D25C++;
      break;
    }
      break;

    case 7:
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009D254, 7);
      D_8009D254->animFrame = D_8009D254->animLastFrame << 0xF;
    }
      if (g_AyaActionTimer == 0)
    {
      D_8009D254->renderObject.variant_visible = 1;
      Anim_SetInterpRate(&D_8009D254->renderObject, 0xF);
      D_8009D25C += 1;
      D_8009D254->renderObject.flags_9C |= 4;
    }
    else
    {
      g_AyaActionTimer -= 1;
    }
      break;

    case 8:
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
    {
      {
        u16 flags = D_8009D254->renderObject.flags_9C;
        flags |= 0x20;
        D_8009D254->renderObject.flags_9C = flags;
      }
      Entity_SetActionMode(D_8009D254, ((g_AyaAttackStep * 2) + 8) & 0xFFFE);
      D_8009D25C += 1;
      break;
    }
      break;

    case 9:
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009D254, ((g_AyaAttackStep * 2) + 9) & 0xFFFF);
      {
        Combatant *attacker;
        u32 hitFlags;
        u32 classFlags;
        register unsigned attackMask asm("$6");
        unsigned countMask;
        register unsigned index asm("$3");
        unsigned targetIndex;
        unsigned int value;
        BattleEntity *target;
        attackMask = 0xFFF30000;
        asm("" : "=r"(attackMask) : "0"(attackMask));
        index = D_8009D1D4;
        value = 2;
        g_EvasionReaction = value;
        value = 1;
        hitCountMask = 7;
        g_EvasionStatus = value;
        asm("" : "=r"(index) : "0"(index), "m"(g_EvasionStatus));
        targetIndex = index;
        target = D_800BE830[targetIndex].actor;
        attackMask |= 0xFFFF;
        /* Keep the word lvalue view: direct members change stock GCC scheduling. */
        targetCore = target->core;
        countMask = 0xFFFC7FFF;
        hitFlags = ((*(u32 *)&targetCore->coreFlags) & (~0x6000)) | 0x2000;
        attacker = D_8009D278;
        (*(u32 *)&targetCore->coreFlags) = hitFlags;
        classFlags = (hitFlags & attackMask) | ((((attacker->action->attackWord) >> 0x14) & 3) << 0x12);
        (*(u32 *)&targetCore->coreFlags) = classFlags;
        (*(u32 *)&targetCore->coreFlags) = ((classFlags & countMask) | ((g_EvasionReaction & hitCountMask) << 0xF));
        if (g_AyaAttackStep == 2)
        {
          effectTarget = D_800BE830[targetIndex].actor;
          targetEffectId = 0x6F;
        }
        else
          if (g_AyaAttackStep == 5)
        {
          effectTarget = D_800BE830[targetIndex].actor;
          targetEffectId = 0x71;
        }
        else
        {
          targetEffectId = 0x6E;
          if (g_AyaAttackStep == 6)
          {
            effectTarget = D_800BE830[targetIndex].actor;
            targetEffectId = 0x70;
          }
          else
          {
            effectTarget = D_800BE830[targetIndex].actor;
          }
        }
        Scene_LoadRoomAssets(targetEffectId, effectTarget);
      }
      attackStep = ((u8) g_AyaAttackStep) + 1;
      g_AyaAttackStep = attackStep;
      if (attackStep < 7)
      {
        slot = D_8009D1D4 & 0xFF;
        if (D_800BE830[slot].actor == D_800BE830[slot + 1].actor)
        {
          {
            u8 next = D_8009D1D4 + 1;
            D_8009D25C = 8;
            D_8009D1D4 = next;
          }
          break;
        }
      }
      D_8009D25C += 1;
      break;
    }
      break;

    case 10:
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
    {
      Entity_SetActionMode(D_8009D254, 7);
      if (g_AyaAttackStep < 7)
      {
        D_8009D25C = 5;
        D_8009D1D4 += 1;
        break;
      }
      Anim_SetInterpRate(&D_8009D254->renderObject, 15);
      D_8009D25C += 1;
      D_8009D254->renderObject.flags_9C |= 2;
      break;
    }
      break;

    case 11:
    {
      BattleEntity *player = D_8009D254;
      s32 *saved;
      register u16 rotationZ asm("$3");
      s32 x;
      s32 y;
      register int effect asm("$4");
      if (player->renderObject.variant_visible == 0)
      {
        effect = 0x4B6;
        saved = &g_AyaSavedPosX;
        transfer = *saved;
        player->posX.fixed = transfer;
        x = player->posX.parts.integer;
        transfer = g_AyaSavedPosY;
        player->posY.fixed = transfer;
        y = player->posY.parts.integer;
        transfer = g_AyaSavedPosZ;
        player->posZ.fixed = transfer;
        transfer = *saved;
        player->baseX = transfer;
        transfer = g_AyaSavedPosY;
        player->baseY = transfer;
        transfer = g_AyaSavedPosZ;
        player->baseZ = transfer;
        transfer = g_AyaSavedRotation[0];
        player->rotationX = transfer;
        transfer = g_AyaSavedRotation[1];
        player->facingAngle = transfer;
        asm volatile("" : : "r"(transfer));
        rotationZ = g_AyaSavedRotationZ;
        player->rotationZ = rotationZ;
        transfer = player->posZ.parts.integer;
        Asset_Find08Alt(effect, 0, x, y, transfer);
        g_AyaActionTimer = 0x1E;
        D_8009D25C++;
        break;
      }
      break;
    }

    case 12:
      if (g_AyaActionTimer == 0)
    {
      Entity_SetActionMode(D_8009D254, 7);
      D_8009D254->renderObject.variant_visible = 1;
      Anim_SetInterpRate(&D_8009D254->renderObject, 0x1E);
      D_801F1F38 = 1;
      g_AyaActionTimer = 0xF;
      D_8009D25C += 1;
      D_8009D254->renderObject.flags_9C |= 4;
      break;
    }
      g_AyaActionTimer -= 1;
      break;
      break;

    case 13:
      if (g_AyaActionTimer == 0)
    {
      if (D_8009D254->animLastFrame == D_8009D254->animPrev.parts.integer)
      {
        Entity_SetActionMode(D_8009D254, 4);
        D_8009D25C += 1;
        break;
      }
      break;
    }
      g_AyaActionTimer -= 1;
      break;

    case 14:
      if (D_8009D254->animLastFrame == (u16)(D_8009D254->animFrame >> 16))
    {
      D_8009D254->entityFlags |= 0x100;
      Anim_SetInterpRate(&D_8009D254->renderObject, 0xF);
      effectId = 0x72;
      D_8009D254->renderObject.flags_9C |= 2;
      Scene_LoadRoomAssets(effectId, D_8009D254);
      D_8009D25C += 1;
      break;
    }
      break;

    case 15:
      if (D_8009D254->renderObject.variant_visible == 0)
    {
      if (Scene_InitEntityPlayer(0) == 0)
      {
        Entity_SetActionMode(D_8009D254, D_8009D278->actionMode12);
        {
          register int mask asm("$3");
          register int rate asm("$5") = 30;
          BattleEntity *player;
          register RenderObjectEntity *render asm("$4");
          unsigned flags;
          mask = ~0x100;
          asm("" : "=r"(mask) : "0"(mask), "r"(rate));
          player = D_8009D254;
          D_8009D2E8 |= 4;
          player->renderObject.variant_visible = 1;
          render = &D_8009D254->renderObject;
          flags = player->entityFlags;
          flags &= mask;
          mask = ~0x80;
          flags &= mask;
          player->entityFlags = flags;
          Anim_SetInterpRate(render, rate);
        }
        D_8009D254->renderObject.flags_9C |= 4;
        D_800B0CEC.variant_visible = 1;
        Anim_SetInterpRate(&D_800B0CEC, 0x1E);
        {
          g_AyaActionTimer = 0xF;
          D_800B0CEC.flags_9C |= 4;
          D_8009D25C++;
        }
        break;
      }
    }
      break;

    case 16:
      if (g_AyaActionTimer == 0)
    {
      {
        int mask = ~0x100;
        unsigned flags = D_8009D1A0;
        u8 next = D_8009D1D4 + 1;
        D_8009D1A0 = flags & mask;
        D_8009D1D4 = next;
      }
      finished = 1;
    }
    else
    {
      g_AyaActionTimer -= 1;
      break;
    }
      break;

  }

  return finished;
}
