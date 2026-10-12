# Compilation flags of the game code

Every C file is compiled by `tools/scripts/cc.sh`: the stock PSX GCC 2.7.2
`cc1`, then stock MASPSX, then GNU as. The defaults are

```
cc1     -w -O2 -G0 -funsigned-char -mips1 -mcpu=3000
maspsx  --aspsx-version=2.56, assembler -G0
```

A file changes them with marker comments. This page covers `src/main`
without `src/main/psyq` and `src/overlays`; `tools/reorg/flag_audit.py`
regenerates the tables (`--markdown`), and
`PE_CC_PRINT_FLAGS=1 tools/scripts/cc.sh <file.c> -` prints what cc.sh
resolves for one file. Evidence numbers are **lev**, the Levenshtein
distance over instruction words between the unit's object and the object
compiled without the marker part, measured with the marker removed in place
(same file name, same line numbers).

## How cc.sh reads the markers

cc.sh greps the whole `.c` file (not the headers or `.inc` files it
includes):

- `CC1_FLAGS:` the last `-G<n>` on the line replaces cc1's `-G0`; `-O1`/`-O3`
  replace `-O2`; every other option is matched against a fixed list
  (`-g3`, `-fno-schedule-insns`, `-fno-schedule-insns2`,
  `-fno-strength-reduce`, `-fno-cse-skip-blocks`,
  `-fno-expensive-optimizations`, `-fno-force-mem`, `-ffixed-<reg>`, ...).
  An option that is not on the list is silently ignored.
- `MASPSX_FLAGS:` the last `-G<n>` becomes the assembler `-G`;
  `--expand-div`, `--dont-expand-li` and `--use-comm-section` are passed to
  MASPSX.
- `MASPSX_FORCE_G0: 1` keeps MASPSX in its stock forced-G0 mode when the
  assembler `-G` is not 0; without it cc.sh passes `--dont-force-G0`. With
  assembler `-G0` the marker does nothing.
- `ASPSX_VERSION:` the ASPSX version MASPSX emulates.
- `GCC_VERSION: 2.8.1` selects the stock GCC 2.8.1 `cc1`.
- `ASSEMBLER: GNU` assembles cc1's output with plain GNU as (no MASPSX);
  `GAS_VERSION:` selects gas 2.7 / 2.8.1 (only PsyQ units use it).

## What each flag does and why a unit needs it

**cc1 `-G<n>`.** cc1 treats data objects of at most `n` bytes as small data:
it assumes they are reachable in one instruction and does not keep their
address in a register. Retail reaches some words through `$gp`; the cc1
threshold mostly changes register allocation and address reuse (a `-G8`
unit folds `lui/lw ... lui/sw` where a `-G0` unit keeps `&word` in a
register, the REG case of `docs/REORGANIZATION_G_FLAGS.md`).

**maspsx `-G<n>`, `--use-comm-section`, `MASPSX_FORCE_G0`.** The assembler
threshold decides which loads and stores become `$gp`-relative. In forced-G0
mode MASPSX emits `%gp_rel` only for words this file defines as tentative
(COMMON) definitions, and every other extern stays absolute; without
FORCE_G0 GNU as also turns any small extern it knows into `$gp`.
`--use-comm-section` makes MASPSX emit the COMMON definitions it needs to
know their size. The canonical small-data sets are

- **K8:** cc1 `-G8`, maspsx `-G8 --use-comm-section`, `MASPSX_FORCE_G0`,
  plus one COMMON definition per word retail reaches through `$gp`;
- **K0:** the same with cc1 left at `-G0`, for units whose code needs the
  cc1 `-G0` address reuse.

**maspsx `--expand-div`.** MASPSX expands `div`/`divu` into the divide plus
the divide-by-zero and overflow `break` checks, as ASPSX does; without it
the divide is emitted bare. Adding it is byte-identical for every game unit
except `math/Math_FixedDivide.c`, whose retail divide has no checks (its
neighbours `Math_FixedMul` and `math_fixed` are original assembler). It is
a soft flag: a unit needs it exactly when it divides by a variable.

**maspsx `--dont-expand-li`.** Only acts when the emulated ASPSX is older
than 2.50; with the 2.56 default it does nothing.

**`ASPSX_VERSION`.** MASPSX changes behaviour at 2.60 (`sltu` with a
negative immediate no longer goes through `$at`), 2.70 (a COMMON word may be
reached through `$gp` with an offset, `sw v1,4(gp)`) and 2.80 (`la` through
`$gp`). Compiling every game unit with 2.60 is identical to 2.56, so the
`sltu` change never matters, and 2.77 configures MASPSX exactly like 2.70. Every game unit except
`battle/Battle_EncounterFlow.c` compiles byte-identically with 2.70, so the
marker is a soft flag that can be taken as a union in merges.
`Battle_EncounterFlow` needs < 2.70 because it copies the 8-byte COMMON
`g_BattleEquipStateBlock` and retail stores its second word absolutely.

**`-g3`.** Debug line notes change GCC 2.7.2's scheduling and register
allocation. Three battle units only match with them (lev 1032 to 4292
without); this is the one flag that probably reflects how the original was
built (a debug build of those files).

**`-fno-schedule-insns`.** Disables the first (pre-allocation) scheduling
pass. Retail's order in a few small functions is the unscheduled order.

**`-fno-strength-reduce`.** Disables loop strength reduction: retail keeps
the index multiply / address computation inside the loop.

**`-fno-cse-skip-blocks`, `-fno-expensive-optimizations`, `-fno-force-mem`.**
Narrow CSE and combine options; each is needed by one or two functions whose
retail code keeps a redundant load or a memory operand in place.

**`-ffixed-<reg>`.** Reserves registers for the whole unit. Only the
`scene_e20_cruiser` hover orb uses it (8 registers), as matching debt.

**`GCC_VERSION: 2.8.1`.** `mdec_diag/DecDCTReset.c` is a PsyQ object built
by GCC 2.8.1 (it fills a return delay slot GCC 2.7.2 leaves empty).

**`ASSEMBLER: GNU`.** The unit is evidenced original assembler
(`GAME_ASM_FUNCTION`, PsyQ assembler objects). `make source-policy-check`
requires the marker on those units even where MASPSX would produce the same
bytes.

## Settings in use

Measured on 2026-10-12 with `tools/reorg/flag_audit.py --markdown`. A
setting is what cc.sh runs, so two files whose markers are spelled
differently but resolve to the same options count once.

| # | effective setting | main | overlays |
|---|---|---|---|
| S1 | `cc1 -G0 ; maspsx -G0` (default) | 26 | 116 |
| S2 | `cc1 -G0 ; maspsx -G0 --expand-div` | 9 | 24 |
| S3 | `cc1 -G8 ; maspsx -G8 --use-comm-section FORCE_G0` | 17 | 0 |
| S4 | `cc1 -G0 ; gnu-as -G0` | 4 | 6 |
| S5 | `cc1 -G8 ; maspsx -G8 --expand-div --use-comm-section FORCE_G0` | 8 | 0 |
| S6 | `cc1 -G8 ; maspsx -G8 --use-comm-section` | 7 | 0 |
| S7 | `cc1 -G0 -fno-schedule-insns ; maspsx -G0` | 4 | 2 |
| S8 | `cc1 -G0 -fno-cse-skip-blocks ; maspsx -G0` | 0 | 3 |
| S9 | `cc1 -G0 -fno-strength-reduce ; maspsx -G0` | 1 | 2 |
| S10 | `cc1 -G8 ; maspsx -G8 --expand-div --use-comm-section FORCE_G0 aspsx 2.70` | 3 | 0 |
| S11 | `cc1 -G0 -fno-strength-reduce ; maspsx -G0 --expand-div` | 2 | 0 |
| S12 | `cc1 -G0 ; maspsx -G8 --use-comm-section FORCE_G0` | 2 | 0 |
| S13 | `cc1 -G8 ; maspsx -G0` | 0 | 2 |
| S14 | `cc1 -G8 ; maspsx -G8` | 2 | 0 |
| S15 | `cc1 -G8 ; maspsx -G8 --dont-expand-li --use-comm-section FORCE_G0` | 2 | 0 |
| S16 | `cc1 -G8 ; maspsx -G8 --use-comm-section FORCE_G0 aspsx 2.70` | 2 | 0 |
| S17 | `cc1 -G0 -ffixed-10 -ffixed-11 -ffixed-15 -ffixed-21 -ffixed-22 -ffixed-23 -ffixed-24 -ffixed-25 ; maspsx -G0 --expand-div` | 0 | 1 |
| S18 | `cc1 -G0 -fno-expensive-optimizations ; maspsx -G0` | 0 | 1 |
| S19 | `cc1 -G0 -fno-force-mem ; maspsx -G0` | 1 | 0 |
| S20 | `cc1 -G0 -fno-strength-reduce -fno-expensive-optimizations ; maspsx -G0` | 1 | 0 |
| S21 | `cc1 -G0 -fno-strength-reduce ; maspsx -G8 --use-comm-section` | 1 | 0 |
| S22 | `cc1 -G0 ; maspsx -G0 --dont-expand-li` | 0 | 1 |
| S23 | `cc1 -G0 ; maspsx -G8 --use-comm-section` | 1 | 0 |
| S24 | `cc1 -G1 -g3 ; maspsx -G1 --expand-div` | 1 | 0 |
| S25 | `cc1 -G1 -g3 ; maspsx -G1 --expand-div --use-comm-section FORCE_G0` | 1 | 0 |
| S26 | `cc1 -G8 -fno-cse-skip-blocks ; maspsx -G8 --expand-div --use-comm-section FORCE_G0` | 1 | 0 |
| S27 | `cc1 -G8 -fno-expensive-optimizations ; maspsx -G8 --use-comm-section FORCE_G0` | 1 | 0 |
| S28 | `cc1 -G8 -fno-schedule-insns ; maspsx -G8 --expand-div` | 1 | 0 |
| S29 | `cc1 -G8 -fno-strength-reduce ; maspsx -G8 --dont-expand-li --use-comm-section FORCE_G0` | 1 | 0 |
| S30 | `cc1 -G8 -g3 ; maspsx -G8 --expand-div --use-comm-section FORCE_G0` | 1 | 0 |
| S31 | `cc1 -G8 ; maspsx -G4` | 1 | 0 |
| S32 | `cc1 -G8 ; maspsx -G4 --use-comm-section FORCE_G0` | 1 | 0 |
| S33 | `cc1 -G8 ; maspsx -G8 --expand-div --use-comm-section` | 1 | 0 |
| S34 | `cc1 -G8 ; maspsx -G8 --expand-div --use-comm-section FORCE_G0 aspsx 2.77` | 1 | 0 |
| S35 | `gcc 2.8.1 ; cc1 -G0 ; maspsx -G0` | 0 | 1 |

35 distinct settings over 263 files.

#### S2: `cc1 -G0 ; maspsx -G0 --expand-div`

- `src/main/akao/Akao_SequencerFlow.c`
- `src/main/cdrom/CdRom_SceneAssetsAndBootRead.c`
- `src/main/draw/Render_FontGlyphSelection.c`
- `src/main/fieldfx/FieldEng_EmitterEffects.c`
- `src/main/fieldfx/FieldEng_GeometryAndShapes.c`
- `src/main/fieldfx/FieldEng_SpriteEffects.c`
- `src/main/fieldfx/FieldEng_SpriteRendering.c`
- `src/main/render/Render_CameraViewportFlow.c`
- `src/main/script/Task_ValueOps.c`
- `src/overlays/boot_display/Boot_StepImageSprite.c`
- `src/overlays/fx_common/camera_nodes.c`
- `src/overlays/room_lib/RoomEffect_DroppedFlare.c`
- `src/overlays/room_lib/RoomEffect_LineBurst.c`
- `src/overlays/room_lib/RoomEffect_SeekerSparkCallback.c`
- `src/overlays/room_lib/RoomEffect_SparkAttacks.c`
- `src/overlays/room_lib/RoomEffect_TrailBurstAttacks.c`
- `src/overlays/room_lib/RoomFx_DropFieldEffects.c`
- `src/overlays/room_lib/RoomFx_GrabAttackSet.c`
- `src/overlays/room_lib/RoomFx_MirrorModel.c`
- `src/overlays/room_lib/RoomLib_ActorClasses.c`
- `src/overlays/room_lib/RoomLib_FloorWalkerClass.c`
- `src/overlays/room_lib/RoomLib_WindowClass.c`
- `src/overlays/room_m023_theater/m023.c`
- `src/overlays/room_m256_museum/m256_swing.c`
- `src/overlays/room_m273_liberty/m273.c`
- `src/overlays/room_m273_liberty/m273_boss_sway.c`
- `src/overlays/room_m318_chrysler/m318.c`
- `src/overlays/room_m349_cruiser/m349.c`
- `src/overlays/room_m350_cruiser/m350.c`
- `src/overlays/room_m350_cruiser/m350_callbacks.c`
- `src/overlays/scene_e18_cruiser/e18_ring_emitters.c`
- `src/overlays/scene_e19_cruiser/e19.c`
- `src/overlays/scene_e22_liberty/e22.c`

#### S3: `cc1 -G8 ; maspsx -G8 --use-comm-section FORCE_G0`

- `src/main/aya/Aya_StatDerivation.c`
- `src/main/battle/BattleCmd_StackAndActiveList.c`
- `src/main/battle/BattleCmd_WeaponLists.c`
- `src/main/battle/Battle_PlayerTurnFlow.c`
- `src/main/boot/Boot_FrameLoop.c`
- `src/main/boot/Boot_InitSystemFlow.c`
- `src/main/item/Inv_ActiveList.c`
- `src/main/item/Inv_ArmorCapacity.c`
- `src/main/item/Inv_InventorySwap.c`
- `src/main/item/Inv_ItemActions.c`
- `src/main/menu/Menu_EquipItemSelection.c`
- `src/main/menu/Menu_EquipScreenFlow.c`
- `src/main/menu/Menu_MainMenu.c`
- `src/main/menu/Menu_StatusPanels.c`
- `src/main/menu/queue.c`
- `src/main/scene/Scene_RoomLoad.c`
- `src/main/script/Task_ActorOpcodes.c`

#### S4: `cc1 -G0 ; gnu-as -G0`

- `src/main/draw/Math_SqrtApprox3.c`
- `src/main/draw/Task_GpuPackPrimColor.c`
- `src/main/math/Math_FixedMul.c`
- `src/main/math/math_fixed.c`
- `src/overlays/mdec_diag/DecDCTPutEnv.c`
- `src/overlays/mdec_diag/DecDCTvlc.c`
- `src/overlays/mdec_diag/DecDCTvlc2.c`
- `src/overlays/mdec_diag/MdecDiag_ReceiveOutput.c`
- `src/overlays/mdec_diag/MdecDiag_Reset.c`
- `src/overlays/mdec_diag/MdecDiag_SendInput.c`

#### S5: `cc1 -G8 ; maspsx -G8 --expand-div --use-comm-section FORCE_G0`

- `src/main/battle/Battle_EncounterFlow.c`
- `src/main/battle/Battle_EnemyMovement.c`
- `src/main/battle/Battle_Setup.c`
- `src/main/battle/Entity_HitAndDropTable.c`
- `src/main/render/Render_FrameUpdateAndScreenState.c`
- `src/main/render/Render_ObjectDraw.c`
- `src/main/save/Save_GameData.c`
- `src/main/script/Task_NodeActions.c`

#### S6: `cc1 -G8 ; maspsx -G8 --use-comm-section`

- `src/main/cdrom/CD_TrackReadFlow.c`
- `src/main/draw/Draw_WipeAndGradient.c`
- `src/main/entity/Scene_LoadMap.c`
- `src/main/item/Menu_OpenItemUsePanelAtIndex.c`
- `src/main/menu/Menu_InventoryScreens.c`
- `src/main/menu/Menu_ItemActions.c`
- `src/main/menu/Menu_ListDrawers.c`

#### S7: `cc1 -G0 -fno-schedule-insns ; maspsx -G0`

- `src/main/battle/Aya_UnlockParasiteSpell.c`
- `src/main/draw/Render_FontGlyphLoad.c`
- `src/main/draw/Task_InitGpuHwRegs.c`
- `src/main/fieldfx/FieldEng_AngleMath.c`
- `src/overlays/fx_common/FxCommon_SetMotionEndpoints.c`
- `src/overlays/room_m273_liberty/RoomM273_PulseSpriteCallback.c`

#### S8: `cc1 -G0 -fno-cse-skip-blocks ; maspsx -G0`

- `src/overlays/menu_memcard/Memcard_FlipScreenPages.c`
- `src/overlays/menu_memcard/Memcard_ListAndScreenFlow.c`
- `src/overlays/scene_e18_cruiser/SceneE18_ExpandingRingCallback.c`

#### S9: `cc1 -G0 -fno-strength-reduce ; maspsx -G0`

- `src/main/fieldfx/FieldEng_CollisionQueries.c`
- `src/overlays/room_lib/RoomFx_MotionParticles_2.c`
- `src/overlays/room_lib/RoomFx_OrbitEffectSet.c`

#### S10: `cc1 -G8 ; maspsx -G8 --expand-div --use-comm-section FORCE_G0 aspsx 2.70`

- `src/main/battle/Battle_StatusHud.c`
- `src/main/battle/Battle_TurnFlow.c`
- `src/main/field/Entity_ActorCollision.c`

#### S11: `cc1 -G0 -fno-strength-reduce ; maspsx -G0 --expand-div`

- `src/main/akao/Akao_VoiceBankLifecycle.c`
- `src/main/render/Render_MeshListAndFade.c`

#### S12: `cc1 -G0 ; maspsx -G8 --use-comm-section FORCE_G0`

- `src/main/boot/Gpu_InitPipeline.c`
- `src/main/script/Task_SystemOpcodes.c`

#### S13: `cc1 -G8 ; maspsx -G0`

- `src/overlays/fx_common/render_op.c`
- `src/overlays/room_m350_cruiser/RoomEffect_ConfiguredPulseCallback.c`

#### S14: `cc1 -G8 ; maspsx -G8`

- `src/main/draw/Gpu_RenderFrame.c`
- `src/main/draw/Render_DrawWireframeModel.c`

#### S15: `cc1 -G8 ; maspsx -G8 --dont-expand-li --use-comm-section FORCE_G0`

- `src/main/menu/Menu_SaveScreens.c`
- `src/main/menu/Menu_SkillScreens.c`

#### S16: `cc1 -G8 ; maspsx -G8 --use-comm-section FORCE_G0 aspsx 2.70`

- `src/main/draw/Draw_Textbox.c`
- `src/main/script/Task_FlowOpcodes.c`

#### S17: `cc1 -G0 -ffixed-10 -ffixed-11 -ffixed-15 -ffixed-21 -ffixed-22 -ffixed-23 -ffixed-24 -ffixed-25 ; maspsx -G0 --expand-div`

- `src/overlays/scene_e20_cruiser/RoomEffect_HoverOrbController_8018F750.c`

#### S18: `cc1 -G0 -fno-expensive-optimizations ; maspsx -G0`

- `src/overlays/fx_common/FxCommon_EffectInitializationFlow.c`

#### S19: `cc1 -G0 -fno-force-mem ; maspsx -G0`

- `src/main/asset/Asset_UnloadTableEntries.c`

#### S20: `cc1 -G0 -fno-strength-reduce -fno-expensive-optimizations ; maspsx -G0`

- `src/main/render/Geo_LoadMeshEntry.c`

#### S21: `cc1 -G0 -fno-strength-reduce ; maspsx -G8 --use-comm-section`

- `src/main/entity/Entity_PoolInit.c`

#### S22: `cc1 -G0 ; maspsx -G0 --dont-expand-li`

- `src/overlays/fx_common/FxCommon_DrawPolyResource.c`

#### S23: `cc1 -G0 ; maspsx -G8 --use-comm-section`

- `src/main/script/Task_FieldSfxQueue.c`

#### S24: `cc1 -G1 -g3 ; maspsx -G1 --expand-div`

- `src/main/battle/Entity_UpdateTimers.c`

#### S25: `cc1 -G1 -g3 ; maspsx -G1 --expand-div --use-comm-section FORCE_G0`

- `src/main/battle/Menu_MainUpdate.c`

#### S26: `cc1 -G8 -fno-cse-skip-blocks ; maspsx -G8 --expand-div --use-comm-section FORCE_G0`

- `src/main/draw/Draw_TextPrimitives.c`

#### S27: `cc1 -G8 -fno-expensive-optimizations ; maspsx -G8 --use-comm-section FORCE_G0`

- `src/main/menu/Menu_ItemDetailPanel.c`

#### S28: `cc1 -G8 -fno-schedule-insns ; maspsx -G8 --expand-div`

- `src/main/draw/Draw_AllocColorQuad.c`

#### S29: `cc1 -G8 -fno-strength-reduce ; maspsx -G8 --dont-expand-li --use-comm-section FORCE_G0`

- `src/main/script/Task_NodePool.c`

#### S30: `cc1 -G8 -g3 ; maspsx -G8 --expand-div --use-comm-section FORCE_G0`

- `src/main/battle/Battle_PhaseAndEnemyEffectFlow.c`

#### S31: `cc1 -G8 ; maspsx -G4`

- `src/main/menu/Menu_SaveSlotDraw.c`

#### S32: `cc1 -G8 ; maspsx -G4 --use-comm-section FORCE_G0`

- `src/main/entity/Entity_FrameUpdate.c`

#### S33: `cc1 -G8 ; maspsx -G8 --expand-div --use-comm-section`

- `src/main/menu/MenuWidget_Lifecycle.c`

#### S34: `cc1 -G8 ; maspsx -G8 --expand-div --use-comm-section FORCE_G0 aspsx 2.77`

- `src/main/battle/Battle_AttackResolution.c`

#### S35: `gcc 2.8.1 ; cc1 -G0 ; maspsx -G0`

- `src/overlays/mdec_diag/DecDCTReset.c`


#### Dead or repeated marker text

- `src/main/boot/Gpu_InitPipeline.c`: line 1: CC1_FLAGS -G0 is the default
- `src/main/script/Task_FieldSfxQueue.c`: line 1: CC1_FLAGS -G0 is the default
- `src/overlays/fx_common/FxCommon_UpdateResource.c`: line 1: CC1_FLAGS -fno-force-addr is not read by cc.sh
- `src/overlays/menu_memcard/Memcard_ListAndScreenFlow.c`: CC1_FLAGS written 2 times

## Evidence per unit

Each marker part of each non-default unit was removed on its own (and the
whole small-data part together) and the unit recompiled; "identical" means
the object is byte-identical without it. "Same code, symbol table differs"
means only the order of symbol-table entries changed (cc1 `-G8` emits
`.extern` lines for small words).

| unit | without this marker part (lev, worst functions) |
|---|---|
| `main/akao/Akao_SequencerFlow.c` | maspsx `--expand-div`: lev 119 (Akao_StepSampleLoader 15, SeqOp_NoteOnWithPitchSlide 13, sndTrackSlideVolume 9) |
| `main/akao/Akao_VoiceBankLifecycle.c` | cc1 `-fno-strength-reduce`: lev 496 (Akao_StepSequencerVoice 62, Spu_SlideVoicePanMasked 45, Spu_ParameterSlide 45); maspsx `--expand-div`: lev 209 (Spu_SlideVoicePitchMasked 26, Spu_SlideVoicePanMasked 26, Spu_ParameterSlide 26) |
| `main/asset/Asset_UnloadTableEntries.c` | cc1 `-fno-force-mem`: lev 1 (Asset_UnloadTableEntries 1) |
| `main/aya/Aya_StatDerivation.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 2 (Inv_GetBonusSlotCount 2); maspsx `--use-comm-section`: lev 1 (Inv_GetBonusSlotCount 1); FORCE_G0: lev 18 (Inv_RecalcSlotStats 7, Battle_GetEnemyEscapeFlag 5, Battle_SyncEquipSlots 4); small-data set: lev 2 (Inv_GetBonusSlotCount 2) |
| `main/battle/Aya_UnlockParasiteSpell.c` | cc1 `-fno-schedule-insns`: lev 7 (Aya_UnlockParasiteSpell 7) |
| `main/battle/BattleCmd_StackAndActiveList.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 80 (Inv_SetActiveList 37, BattleCmd_UndoPending 12, BattleCmd_AllocSlot 9); maspsx `--use-comm-section`: lev 18 (Inv_SetActiveList 7, BattleCmd_UndoPending 2, BattleCmd_UndoAllPending 2); FORCE_G0: lev 62 (Inv_SetActiveList 26, BattleCmd_GetRemainingAmmo 8, BattleCmd_ChangeWeaponAndSync 8); small-data set: lev 80 (Inv_SetActiveList 37, BattleCmd_UndoPending 12, BattleCmd_AllocSlot 9) |
| `main/battle/BattleCmd_WeaponLists.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 13 (BattleCmd_LoadWeaponModifiers 13); maspsx `--use-comm-section`: lev 2 (BattleCmd_LoadWeaponModifiers 2); FORCE_G0: identical; small-data set: lev 13 (BattleCmd_LoadWeaponModifiers 13) |
| `main/battle/Battle_AttackResolution.c` | cc1 `-G8`: lev 182 (Battle_ApplySpellEffect 79, Battle_StepAyaAction 58, Battle_StepCharacterAction 45); maspsx `-G8`: lev 395 (Battle_StepAyaAction 167, Battle_StepCharacterAction 90, Battle_ResolveHitOnTimer 90); maspsx `--use-comm-section`: lev 116 (Battle_StepAyaAction 58, Battle_ResolveHitOnTimer 27, Battle_StepCharacterAction 25); maspsx `--expand-div`: identical; FORCE_G0: lev 569 (Battle_StepAyaAction 151, Battle_ApplySpellEffect 125, Battle_ResolveHitOnTimer 96); ASPSX_VERSION 2.77: lev 63 (Battle_StepAyaAction 63); small-data set: lev 452 (Battle_StepAyaAction 176, Battle_StepCharacterAction 90, Battle_ResolveHitOnTimer 90) |
| `main/battle/Battle_EncounterFlow.c` | cc1 `-G8`: lev 12 (Battle_Update 6, Battle_PhaseInitEnemyTurn 6); maspsx `-G8`: lev 670 (Battle_Update 170, Battle_StepPostBattle 89, Battle_StepVictory 85); maspsx `--use-comm-section`: lev 176 (Battle_Update 44, Battle_StepVictory 32, Battle_StepPostBattle 24); maspsx `--expand-div`: identical; FORCE_G0: lev 1295 (Battle_PhaseInitEnemyTurn 289, Battle_PhaseEndTurn 275, Battle_Update 264); small-data set: lev 682 (Battle_Update 176, Battle_StepPostBattle 89, Battle_StepVictory 85) |
| `main/battle/Battle_EnemyMovement.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: identical; maspsx `--use-comm-section`: identical; maspsx `--expand-div`: lev 70 (Battle_StepEnemyMovement 70); FORCE_G0: lev 169 (Battle_StepEntityAnimState 73, Battle_StepEnemyMovement 71, Battle_UpdateEntityFacing 25); small-data set: same code, symbol table differs |
| `main/battle/Battle_PhaseAndEnemyEffectFlow.c` | cc1 `-g3`: lev 1588 (Battle_AdvancePhase 633, Battle_StartEnemyAttackEffect 161, LM196 37); cc1 `-G8`: same code, symbol table differs; maspsx `--expand-div`: identical; maspsx `-G8`: lev 61 (LM196 3, LM93 2, LM9 2); maspsx `--use-comm-section`: lev 30 (LM93 1, LM9 1, LM77 1); FORCE_G0: lev 99 (LM196 5, LM82 4, LM81 4); small-data set: lev 61 (LM196 3, LM93 2, LM9 2) |
| `main/battle/Battle_PlayerTurnFlow.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 51 (Battle_UpdatePlayerTurn 42, Battle_ApplyPlayerHit 9); maspsx `--use-comm-section`: lev 14 (Battle_UpdatePlayerTurn 11, Battle_ApplyPlayerHit 3); FORCE_G0: lev 74 (Battle_UpdatePlayerTurn 44, Battle_ApplyPlayerHit 30); small-data set: lev 51 (Battle_UpdatePlayerTurn 42, Battle_ApplyPlayerHit 9) |
| `main/battle/Battle_Setup.c` | cc1 `-G8`: lev 90 (Battle_CopyPadStateToRecord 57, Battle_SyncEnemyAttributes 33); maspsx `-G8`: lev 137 (Battle_StepScriptEntry 28, Battle_Init 27, Battle_BuildTargetList 22); maspsx `--use-comm-section`: lev 47 (Battle_Init 13, Battle_StepScriptEntry 8, Battle_BuildTargetList 7); maspsx `--expand-div`: lev 42 (Battle_CheckEvasion 42); FORCE_G0: lev 247 (Battle_CheckEvasion 48, Battle_SyncEnemyAttributes 32, Battle_DispatchEntityEffect 32); small-data set: lev 227 (Battle_CopyPadStateToRecord 57, Battle_SyncEnemyAttributes 33, Battle_StepScriptEntry 28) |
| `main/battle/Battle_StatusHud.c` | cc1 `-G8`: lev 80 (Battle_DrawSpellName 51, Battle_DrawActiveStatus 29); maspsx `-G8`: lev 208 (Battle_DrawActiveStatus 75, Battle_DrawStatusPanel 57, Battle_DrawEnemyHP 26); maspsx `--use-comm-section`: lev 73 (Battle_DrawActiveStatus 34, Battle_DrawStatusPanel 20, Render_AnimationFrame 6); maspsx `--expand-div`: lev 16 (Battle_DrawActiveStatus 16); FORCE_G0: lev 389 (Battle_DrawActiveStatus 62, Battle_DrawStatusPanel 53, Battle_DrawEnemyHP 53); ASPSX_VERSION 2.70: lev 65 (Battle_DrawActiveStatus 45, Battle_DrawEnemyHP 20); small-data set: lev 255 (Battle_DrawActiveStatus 71, Battle_DrawStatusPanel 57, Battle_DrawSpellName 51) |
| `main/battle/Battle_TurnFlow.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 625 (Battle_HandleItemMenu 122, Battle_StepEnemyTurn 116, Battle_StepPlayerTurn 86); maspsx `--use-comm-section`: lev 155 (Battle_StepEnemyTurn 41, Battle_HandleItemMenu 30, Battle_AdvanceTurnSlot 27); maspsx `--expand-div`: lev 80 (Battle_StepEnemyTurn 35, Battle_CycleTarget 31, Battle_FillActionQueue 14); FORCE_G0: lev 644 (Battle_HandleItemMenu 108, Battle_UpdateEnemy 94, Battle_StepPlayerTurn 84); ASPSX_VERSION 2.70: lev 6 (Battle_SetupEntityTarget 6); small-data set: lev 625 (Battle_HandleItemMenu 122, Battle_StepEnemyTurn 116, Battle_StepPlayerTurn 86) |
| `main/battle/Entity_HitAndDropTable.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 13 (Entity_ApplyHitAndSetAnim 13); maspsx `--expand-div`: lev 14 (Entity_ResolveDropTable 14); maspsx `--use-comm-section`: lev 3 (Entity_ApplyHitAndSetAnim 3); FORCE_G0: lev 54 (Entity_ResolveDropTable 31, Entity_ApplyHitAndSetAnim 23); small-data set: lev 13 (Entity_ApplyHitAndSetAnim 13) |
| `main/battle/Entity_UpdateTimers.c` | cc1 `-g3`: lev 1032 (Entity_UpdateTimers 516, LM51 23, LM41 13); cc1 `-G1`: lev 4 (LM59 2, LM58 2); maspsx `--expand-div`: lev 9 (LM51 9); maspsx `-G1`: lev 4 (LM59 2, LM58 2); small-data set: lev 4 (LM59 2, LM58 2) |
| `main/battle/Menu_MainUpdate.c` | cc1 `-g3`: lev 4292 (Menu_MainUpdate 2146, LM35 26, LM22 22); cc1 `-G1`: same code, symbol table differs; maspsx `--expand-div`: lev 18 (LM424 9, LM22 9); maspsx `-G1`: lev 10 (LM352 2, LM323 2, LM166 2); maspsx `--use-comm-section`: lev 5 (LM352 1, LM323 1, LM166 1); FORCE_G0: lev 8 (LM649 2, LM647 2, LM628 2); small-data set: lev 10 (LM352 2, LM323 2, LM166 2) |
| `main/boot/Boot_FrameLoop.c` | cc1 `-G8`: lev 13 (Boot_RunFrame 13); maspsx `-G8`: lev 61 (Boot_RunFrame 56, Boot_FlushSceneFast 5); maspsx `--use-comm-section`: lev 17 (Boot_RunFrame 15, Boot_FlushSceneFast 2); FORCE_G0: lev 52 (Boot_RunFrame 52); small-data set: lev 63 (Boot_RunFrame 58, Boot_FlushSceneFast 5) |
| `main/boot/Boot_InitSystemFlow.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 161 (Field_HandleStateTransition 150, Boot_BuildRenderFlagTable 11); maspsx `--use-comm-section`: lev 55 (Field_HandleStateTransition 50, Boot_BuildRenderFlagTable 5); FORCE_G0: lev 101 (Field_HandleStateTransition 86, Boot_InitSubsystems 9, Boot_BuildRenderFlagTable 4); small-data set: lev 161 (Field_HandleStateTransition 150, Boot_BuildRenderFlagTable 11) |
| `main/boot/Gpu_InitPipeline.c` | cc1 `-G0`: identical; maspsx `-G8`: lev 17 (Gpu_InitPipeline 17); maspsx `--use-comm-section`: lev 4 (Gpu_InitPipeline 4); FORCE_G0: identical; small-data set: lev 17 (Gpu_InitPipeline 17) |
| `main/cdrom/CD_TrackReadFlow.c` | cc1 `-G8`: lev 198 (CD_StepReadState 69, CD_ReadSectors 54, CD_SeekToTrack 53); maspsx `-G8`: lev 206 (CD_StepReadState 77, CD_ReadSectors 54, CD_SeekToTrack 53); maspsx `--use-comm-section`: lev 4 (CD_StepReadState 4); small-data set: lev 206 (CD_StepReadState 77, CD_ReadSectors 54, CD_SeekToTrack 53) |
| `main/cdrom/CdRom_SceneAssetsAndBootRead.c` | maspsx `--expand-div`: lev 19 (Akao_Calc3DPan 19) |
| `main/draw/Draw_AllocColorQuad.c` | cc1 `-G8`: lev 70 (Draw_AllocColorQuad 70); cc1 `-fno-schedule-insns`: lev 107 (Draw_AllocColorQuad 107); maspsx `-G8`: lev 70 (Draw_AllocColorQuad 70); maspsx `--expand-div`: lev 23 (Draw_AllocColorQuad 23); small-data set: lev 70 (Draw_AllocColorQuad 70) |
| `main/draw/Draw_TextPrimitives.c` | cc1 `-G8`: same code, symbol table differs; cc1 `-fno-cse-skip-blocks`: lev 22 (Draw_AllocTexturedQuad 22); maspsx `-G8`: lev 907 (Draw_AllocTexturedRectAlt 211, Draw_PrintTimeValue 81, Draw_PrintCenteredTextInWidth 65); maspsx `--use-comm-section`: lev 343 (Draw_AllocTexturedRectAlt 82, Draw_PrintTimeValue 38, Draw_PrintCenteredTextInWidth 23); maspsx `--expand-div`: lev 73 (Draw_AllocTexturedRect 13, Draw_PrintSignedNumberWidth4 12, Draw_PrintSignedNumberWidth3 12); FORCE_G0: lev 4 (Draw_SetBaseOffsetPosition 2, Draw_GetBaseY 2); small-data set: lev 907 (Draw_AllocTexturedRectAlt 211, Draw_PrintTimeValue 81, Draw_PrintCenteredTextInWidth 65) |
| `main/draw/Draw_Textbox.c` | cc1 `-G8`: lev 20 (Render_SetupColorTable 20); maspsx `-G8`: lev 259 (Menu_DrawTextboxEntries 186, Render_SetupColorTable 24, Gpu_SetLightingParams 14); maspsx `--use-comm-section`: lev 79 (Menu_DrawTextboxEntries 49, Render_SetupColorTable 7, Gpu_SetLightingParams 7); FORCE_G0: lev 108 (Menu_DrawTextboxEntries 108); ASPSX_VERSION 2.70: lev 22 (Render_SetupColorTable 16, Menu_SetTextCursorRect 6); small-data set: lev 261 (Menu_DrawTextboxEntries 186, Render_SetupColorTable 26, Gpu_SetLightingParams 14) |
| `main/draw/Draw_WipeAndGradient.c` | cc1 `-G8`: lev 312 (Draw_AllocColorGradient 113, Draw_AllocColorTriGradient 53, Draw_EmitWipeBarRect 40); maspsx `-G8`: lev 335 (Draw_AllocColorGradient 113, Draw_AllocColorTriGradient 66, Draw_EmitWipeBarRect 40); maspsx `--use-comm-section`: lev 7 (Draw_AllocColorTriGradient 4, Menu_SetSaveSlotBlendBase 1, Draw_GetBlendColor 1); small-data set: lev 335 (Draw_AllocColorGradient 113, Draw_AllocColorTriGradient 66, Draw_EmitWipeBarRect 40) |
| `main/draw/Gpu_RenderFrame.c` | cc1 `-G8`: lev 13 (Gpu_RenderFrame 13); maspsx `-G8`: lev 13 (Gpu_RenderFrame 13); small-data set: lev 13 (Gpu_RenderFrame 13) |
| `main/draw/Render_DrawWireframeModel.c` | cc1 `-G8`: lev 57 (Render_DrawWireframeModel 57); maspsx `-G8`: lev 56 (Render_DrawWireframeModel 56); small-data set: lev 57 (Render_DrawWireframeModel 57) |
| `main/draw/Render_FontGlyphLoad.c` | cc1 `-fno-schedule-insns`: lev 13 (Render_LoadFontGlyph 13) |
| `main/draw/Render_FontGlyphSelection.c` | maspsx `--expand-div`: lev 30 (Render_LoadFontGlyphAlt 30) |
| `main/draw/Task_InitGpuHwRegs.c` | cc1 `-fno-schedule-insns`: lev 4 (Task_InitGpuHwRegs 4) |
| `main/entity/Entity_FrameUpdate.c` | cc1 `-G8`: lev 70 (Entity_FrameUpdate 70); maspsx `-G4`: lev 130 (Entity_FrameUpdate 71, Entity_CollectGarbage 35, Entity_RollbackPositionHierarchy 7); maspsx `--use-comm-section`: lev 28 (Entity_CollectGarbage 12, Entity_FrameUpdate 11, Entity_IntegratePositionFull 2); FORCE_G0: lev 51 (Entity_FrameUpdate 48, Entity_DispatchCallbacks 3); small-data set: lev 155 (Entity_FrameUpdate 96, Entity_CollectGarbage 35, Entity_RollbackPositionHierarchy 7) |
| `main/entity/Entity_PoolInit.c` | cc1 `-fno-strength-reduce`: lev 3 (Entity_ResetAllPools 3); maspsx `-G8`: lev 28 (Entity_ResetAllPools 18, Entity_InitFreePool 10); maspsx `--use-comm-section`: lev 11 (Entity_ResetAllPools 7, Entity_InitFreePool 4); small-data set: lev 28 (Entity_ResetAllPools 18, Entity_InitFreePool 10) |
| `main/entity/Scene_LoadMap.c` | cc1 `-G8`: lev 44 (Scene_LoadMap 44); maspsx `--use-comm-section`: lev 2 (Scene_LoadMap 2); maspsx `-G8`: lev 50 (Scene_LoadMap 50); small-data set: lev 50 (Scene_LoadMap 50) |
| `main/field/Entity_ActorCollision.c` | cc1 `-G8`: lev 68 (Entity_SlideOnRamp 68); maspsx `-G8`: lev 562 (Geo_ClipToFloorBoundary 165, Entity_UpdateAndRender 100, Geo_ClipToFloorBoundarySub 53); maspsx `--use-comm-section`: lev 150 (Geo_ClipToFloorBoundary 48, Entity_UpdateAndRender 22, Entity_ResetStateGlobals 16); maspsx `--expand-div`: lev 193 (Geo_ClipToFloorBoundary 76, Entity_SlideOnWall 36, Entity_SlideOnRamp 36); FORCE_G0: lev 196 (Geo_ClipToFloorBoundary 71, Entity_UpdateAndRender 69, Entity_SlideOnRamp 28); ASPSX_VERSION 2.70: lev 93 (Geo_ClipToFloorBoundary 61, Entity_SlideOnRamp 32); small-data set: lev 592 (Geo_ClipToFloorBoundary 165, Entity_UpdateAndRender 100, Entity_SlideOnRamp 82) |
| `main/fieldfx/FieldEng_AngleMath.c` | cc1 `-fno-schedule-insns`: lev 2 (FieldEng_TurnToward 2) |
| `main/fieldfx/FieldEng_CollisionQueries.c` | cc1 `-fno-strength-reduce`: lev 53 (func_800C5EB0 53) |
| `main/fieldfx/FieldEng_EmitterEffects.c` | maspsx `--expand-div`: lev 14 (FieldEng_CosineEffect 14) |
| `main/fieldfx/FieldEng_GeometryAndShapes.c` | maspsx `--expand-div`: lev 143 (func_800D0728 34, func_800D3114 33, func_800D004C 29) |
| `main/fieldfx/FieldEng_SpriteEffects.c` | maspsx `--expand-div`: lev 105 (func_800D5010 32, func_800D7FBC 26, func_800D5898 24) |
| `main/fieldfx/FieldEng_SpriteRendering.c` | maspsx `--expand-div`: lev 46 (func_800C3B04 21, func_800C4E50 13, func_800C4FC4 6) |
| `main/item/Inv_ActiveList.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 580 (Inv_SetupSlotDisplay 96, Inv_PickRandomItem 44, Inv_WriteSlotById 38); maspsx `--use-comm-section`: lev 164 (Inv_SetupSlotDisplay 31, Inv_PickRandomItem 12, Inv_SelectActiveList 10); FORCE_G0: lev 11 (Inv_CompactActiveListSlots 11); small-data set: lev 580 (Inv_SetupSlotDisplay 96, Inv_PickRandomItem 44, Inv_WriteSlotById 38) |
| `main/item/Inv_ArmorCapacity.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 697 (Inv_TransferItemBetweenLists 263, Inv_BuildStorageDisplay 78, Inv_SortInventoryByMode 63); maspsx `--use-comm-section`: lev 187 (Inv_TransferItemBetweenLists 58, Inv_BuildStorageDisplay 28, Inv_SortInventoryByMode 22); FORCE_G0: lev 9 (Inv_GetActiveSlotCount 9); small-data set: lev 697 (Inv_TransferItemBetweenLists 263, Inv_BuildStorageDisplay 78, Inv_SortInventoryByMode 63) |
| `main/item/Inv_InventorySwap.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 397 (Inv_RebuildWithSlotLimit 91, Inv_TransferItemAlt 47, Inv_TransferItem 46); maspsx `--use-comm-section`: lev 108 (Inv_RebuildWithSlotLimit 18, Inv_TransferItem 17, Inv_TransferItemAlt 13); FORCE_G0: lev 90 (Inv_TransferItem 56, Inv_TransferItemAlt 27, Inv_CanAddActiveListItemToAya 7); small-data set: lev 397 (Inv_RebuildWithSlotLimit 91, Inv_TransferItemAlt 47, Inv_TransferItem 46) |
| `main/item/Inv_ItemActions.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 932 (Inv_BuildCompatibleWeaponBitset 217, Inv_RebuildSelectableMask 181, Inv_InitSlotDisplay 94); maspsx `--use-comm-section`: lev 242 (Inv_RebuildSelectableMask 50, Inv_BuildCompatibleWeaponBitset 48, Inv_InitSlotDisplay 37); FORCE_G0: lev 25 (Inv_RemoveActiveListItem 11, Inv_CheckFreeSlotCapacity 8, Item_PackParasiteSpells 6); small-data set: lev 932 (Inv_BuildCompatibleWeaponBitset 217, Inv_RebuildSelectableMask 181, Inv_InitSlotDisplay 94) |
| `main/item/Menu_OpenItemUsePanelAtIndex.c` | cc1 `-G8`: lev 22 (Menu_OpenItemUsePanelAtIndex 22); maspsx `-G8`: lev 22 (Menu_OpenItemUsePanelAtIndex 22); maspsx `--use-comm-section`: identical; small-data set: lev 22 (Menu_OpenItemUsePanelAtIndex 22) |
| `main/menu/MenuWidget_Lifecycle.c` | cc1 `-G8`: lev 282 (Menu_StepScrollCursor 123, Draw_FlushPrimList 47, MenuWidget_DrawList 46); maspsx `-G8`: lev 797 (Menu_StepScrollCursor 133, MenuWidget_DrawList 109, MenuWidget_DrawListRow 74); maspsx `--use-comm-section`: lev 179 (MenuWidget_DrawListRow 29, MenuWidget_DrawList 29, Draw_FlushFrontBuffer 27); maspsx `--expand-div`: lev 42 (Draw_SetPrimCallback 22, MenuWidget_EaseNodePosition 20); small-data set: lev 797 (Menu_StepScrollCursor 133, MenuWidget_DrawList 109, MenuWidget_DrawListRow 74) |
| `main/menu/Menu_EquipItemSelection.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 177 (Menu_CreateAmmoSpendPanel 36, Menu_StepEquipSlotSelect 29, Menu_StepItemGridScroll 24); maspsx `--use-comm-section`: lev 44 (Menu_CreateAmmoSpendPanel 17, Menu_StepEquipSlotSelect 6, Menu_AmmoSpendInputHandler 6); FORCE_G0: identical; small-data set: lev 177 (Menu_CreateAmmoSpendPanel 36, Menu_StepEquipSlotSelect 29, Menu_StepItemGridScroll 24) |
| `main/menu/Menu_EquipScreenFlow.c` | cc1 `-G8`: same code, symbol table differs; maspsx `--use-comm-section`: lev 46 (Menu_OpenItemActionSubmenu 13, Menu_StepEquipScreen 11, Menu_OnInventoryItemConfirm 9); maspsx `-G8`: lev 185 (Menu_InventoryInputHandler 56, Menu_StepEquipScreen 50, Menu_OpenItemActionSubmenu 44); FORCE_G0: lev 13 (Menu_OnInventoryItemConfirm 13); small-data set: lev 185 (Menu_InventoryInputHandler 56, Menu_StepEquipScreen 50, Menu_OpenItemActionSubmenu 44) |
| `main/menu/Menu_InventoryScreens.c` | cc1 `-G8`: lev 36 (Menu_OpenInventoryScreen 21, Menu_OpenEquipScreen 5, Menu_DrawEquipListContainer 4); maspsx `--use-comm-section`: lev 68 (Menu_ResetStateFlags 10, Menu_OpenInventoryScreen 9, Menu_StepInventoryCategory 6); maspsx `-G8`: lev 228 (Menu_OpenInventoryScreen 39, Menu_StepInventoryCategory 29, Menu_InventoryPageNavHandler 25); small-data set: lev 228 (Menu_OpenInventoryScreen 39, Menu_StepInventoryCategory 29, Menu_InventoryPageNavHandler 25) |
| `main/menu/Menu_ItemActions.c` | cc1 `-G8`: lev 362 (Menu_ItemListInputHandler 123, Spend_BonusPoints 65, Menu_StepInventoryRoot 58); maspsx `-G8`: lev 637 (Menu_ItemListInputHandler 157, Menu_StepInventoryRoot 87, Spend_BonusPoints 83); maspsx `--use-comm-section`: lev 97 (Menu_ItemListInputHandler 17, Menu_StepInventoryRoot 13, Menu_StepEquipSlotSelect2 10); small-data set: lev 637 (Menu_ItemListInputHandler 157, Menu_StepInventoryRoot 87, Spend_BonusPoints 83) |
| `main/menu/Menu_ItemDetailPanel.c` | cc1 `-G8`: same code, symbol table differs; cc1 `-fno-expensive-optimizations`: lev 34 (Menu_StepItemDetailPanel 34); maspsx `-G8`: lev 37 (Menu_StepItemDetailPanel 15, Menu_ConfirmDialogHandler 12, Menu_OnItemDiscardConfirm 10); maspsx `--use-comm-section`: lev 13 (Menu_StepItemDetailPanel 6, Menu_OnItemDiscardConfirm 3, Menu_DrawItemLabel 2); FORCE_G0: lev 3 (Menu_DrawItemLabel 3); small-data set: lev 37 (Menu_StepItemDetailPanel 15, Menu_ConfirmDialogHandler 12, Menu_OnItemDiscardConfirm 10) |
| `main/menu/Menu_ListDrawers.c` | cc1 `-G8`: lev 17 (Menu_HandleMemCardWriteOrError 6, Menu_DrawSelectableEquipSlotRow 5, Menu_DrawStatAllocationItem 2); maspsx `-G8`: lev 129 (Sfx_ContextPick 23, Menu_DrawItemSlot 13, Menu_SoundTestSelect 11); maspsx `--use-comm-section`: lev 34 (Sfx_ContextPick 5, Menu_DrawItemSlot 3, Save_StartSelectedWriteSlot 2); small-data set: lev 129 (Sfx_ContextPick 23, Menu_DrawItemSlot 13, Menu_SoundTestSelect 11) |
| `main/menu/Menu_MainMenu.c` | cc1 `-G8`: lev 11 (Aya_SetTotalExp 11); maspsx `-G8`: lev 252 (Menu_DrawBonusPointAnimFrame 72, Aya_SetTotalExp 39, Menu_StepParasiteScreen 37); maspsx `--use-comm-section`: lev 100 (Menu_DrawBonusPointAnimFrame 29, Aya_SetTotalExp 18, Menu_StepParasiteScreen 14); FORCE_G0: lev 9 (Aya_SetTotalExp 9); small-data set: lev 257 (Menu_DrawBonusPointAnimFrame 72, Aya_SetTotalExp 44, Menu_StepParasiteScreen 37) |
| `main/menu/Menu_SaveScreens.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 310 (Menu_DrawContextHelpText 107, Menu_StepNameEntryAlt 43, Menu_StepSaveSelect 41); maspsx `--use-comm-section`: lev 67 (Menu_DrawContextHelpText 18, Menu_StepSaveSelect 13, Menu_StepItemGrid2 6); maspsx `--dont-expand-li`: identical; FORCE_G0: lev 59 (Menu_StepNameEntryAlt 46, Menu_StepNameEntry 13); small-data set: lev 310 (Menu_DrawContextHelpText 107, Menu_StepNameEntryAlt 43, Menu_StepSaveSelect 41) |
| `main/menu/Menu_SaveSlotDraw.c` | cc1 `-G8`: lev 127 (Save_DrawSlotMetadata 97, Menu_SaveOverlayDraw 30); maspsx `-G4`: lev 57 (Menu_SaveOverlayDraw 30, Save_DrawSlotMetadata 27); small-data set: lev 127 (Save_DrawSlotMetadata 97, Menu_SaveOverlayDraw 30) |
| `main/menu/Menu_SkillScreens.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 177 (Menu_StepEquipConfirm 35, Menu_StepSkillList 27, Menu_OpenInventoryOrSwapView 20); maspsx `--use-comm-section`: lev 43 (Menu_StepEquipConfirm 7, Menu_StepSkillList 6, Menu_StepItemDetailPanel2 6); maspsx `--dont-expand-li`: identical; FORCE_G0: lev 4 (Evt_DeferredExec 4); small-data set: lev 177 (Menu_StepEquipConfirm 35, Menu_StepSkillList 27, Menu_OpenInventoryOrSwapView 20) |
| `main/menu/Menu_StatusPanels.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 243 (Menu_StepItemSelectScreen 44, Menu_SaveBgApplyFadeStep 35, Menu_HandleInput 27); maspsx `--use-comm-section`: lev 67 (Menu_SaveBgApplyFadeStep 14, Menu_StepItemSelectScreen 8, Menu_HandleInput 6); FORCE_G0: identical; small-data set: lev 243 (Menu_StepItemSelectScreen 44, Menu_SaveBgApplyFadeStep 35, Menu_HandleInput 27) |
| `main/menu/queue.c` | cc1 `-G8`: same code, symbol table differs; maspsx `--use-comm-section`: lev 76 (MenuInput_EnqueueStatusChanges 24, Draw_SelectBuffer 9, MenuInput_DispatchQueuedEvents 7); maspsx `-G8`: lev 223 (MenuInput_EnqueueStatusChanges 70, MenuInput_DispatchQueuedEvents 38, Queue_DequeueByMask 22); FORCE_G0: lev 19 (Draw_RemapStatusFlags 15, Draw_SelectBuffer 4); small-data set: lev 223 (MenuInput_EnqueueStatusChanges 70, MenuInput_DispatchQueuedEvents 38, Queue_DequeueByMask 22) |
| `main/render/Geo_LoadMeshEntry.c` | cc1 `-fno-strength-reduce`: lev 93 (Geo_LoadMeshEntry 93); cc1 `-fno-expensive-optimizations`: lev 16 (Geo_TransformPoint 16) |
| `main/render/Render_CameraViewportFlow.c` | maspsx `--expand-div`: lev 70 (Render_StepCameraTransition 37, Geom_StepGroupAnimations 33) |
| `main/render/Render_FrameUpdateAndScreenState.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 86 (Render_MoveGridCursor 86); maspsx `--use-comm-section`: lev 20 (Render_MoveGridCursor 20); maspsx `--expand-div`: lev 34 (Render_SetCDDCSlot 34); FORCE_G0: lev 89 (Render_SetCDDCSlot 59, CdRom_InitScreenState 19, Render_PrepareFrame 11); small-data set: lev 86 (Render_MoveGridCursor 86) |
| `main/render/Render_MeshListAndFade.c` | cc1 `-fno-strength-reduce`: lev 191 (Render_DrawSpriteEntry 76, Geom_ScrollEntries 52, Render_StepFade 23); maspsx `--expand-div`: lev 75 (Render_DrawSpriteEntry 36, Geom_ScrollEntries 23, Render_StepFade 16) |
| `main/render/Render_ObjectDraw.c` | cc1 `-G8`: lev 54 (Render_ColorEntity 32, Render_DrawObject 14, Render_TickObject 8); maspsx `-G8`: lev 3 (Render_TransformSkinnedVertices 3); maspsx `--use-comm-section`: lev 1 (Render_TransformSkinnedVertices 1); maspsx `--expand-div`: lev 48 (Render_ColorEntity 39, Anim_SetInterpRate 9); FORCE_G0: lev 121 (Render_SetEntityBlendMode 24, Render_DrawTexturedQuads 22, Render_DrawEntity 22); small-data set: lev 57 (Render_ColorEntity 32, Render_DrawObject 14, Render_TickObject 8) |
| `main/save/Save_GameData.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 526 (Inv_DispatchCommand 83, Inv_RebuildWithBonusSlots 76, Inv_MergeStorageToSlot 54); maspsx `--use-comm-section`: lev 143 (Inv_DispatchCommand 19, Save_InitMetadataState 16, Inv_MergeStorageToSlot 16); maspsx `--expand-div`: lev 33 (Stat_QueryLevelAndSubLevel 17, Stat_QueryDistanceAndSubLevel 16); FORCE_G0: lev 52 (Inv_DispatchCommand 48, Save_GetCurrentMapNumber 2, Save_BuildHeader 2); small-data set: lev 526 (Inv_DispatchCommand 83, Inv_RebuildWithBonusSlots 76, Inv_MergeStorageToSlot 54) |
| `main/scene/Scene_RoomLoad.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 63 (Scene_LoadRoom 63); maspsx `--use-comm-section`: lev 1 (Scene_LoadRoom 1); FORCE_G0: lev 305 (Scene_LoadEntityTextures 69, Scene_InitEntityPlayer 47, Scene_LoadRoom 41); small-data set: lev 63 (Scene_LoadRoom 63) |
| `main/script/Task_ActorOpcodes.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 207 (Akao_ProcessCommand 63, Camera_StepAngleFade 41, Camera_TrackRelativeOffset 28); maspsx `--use-comm-section`: lev 45 (Camera_StepAngleFade 10, Task_RunInvCommand 6, Camera_TrackRelativeOffset 5); FORCE_G0: lev 404 (Akao_ProcessCommand 82, Task_SetGteMatrix 48, Camera_TrackRelativeOffset 34); small-data set: lev 207 (Akao_ProcessCommand 63, Camera_StepAngleFade 41, Camera_TrackRelativeOffset 28) |
| `main/script/Task_FieldSfxQueue.c` | cc1 `-G0`: identical; maspsx `-G8`: lev 30 (Scene_UpdateEntityList 22, Task_QueueFieldSfx 6, Task_ClearSfxTable 2); maspsx `--use-comm-section`: lev 7 (Task_QueueFieldSfx 3, Scene_UpdateEntityList 3, Task_ClearSfxTable 1); small-data set: lev 30 (Scene_UpdateEntityList 22, Task_QueueFieldSfx 6, Task_ClearSfxTable 2) |
| `main/script/Task_FlowOpcodes.c` | cc1 `-G8`: same code, symbol table differs; maspsx `-G8`: lev 192 (Task_DispatchScriptCmd 59, Task_RunQueue 37, Entity_SelectFieldSetter 18); maspsx `--use-comm-section`: lev 49 (Task_RunQueue 10, Task_DispatchScriptCmd 9, Task_OpenSaveLoadMenu 5); FORCE_G0: lev 197 (Task_DispatchScriptCmd 66, Entity_SelectFieldSetter 26, Task_RunQueue 20); ASPSX_VERSION 2.70: lev 49 (Task_DispatchScriptCmd 49); small-data set: lev 192 (Task_DispatchScriptCmd 59, Task_RunQueue 37, Entity_SelectFieldSetter 18) |
| `main/script/Task_NodeActions.c` | cc1 `-G8`: lev 32 (Task_SpawnChildNode 32); maspsx `-G8`: lev 238 (Camera_TrackEntityZoom 46, Camera_SnapToEntity 41, Task_MoveTowardPoint 32); maspsx `--expand-div`: lev 27 (Task_WaitAnimRange 27); maspsx `--use-comm-section`: lev 45 (Task_SpawnChildNode 9, Task_MoveTowardPoint 6, Camera_TrackEntityZoom 6); FORCE_G0: lev 230 (Camera_TrackEntityZoom 54, Camera_SnapToEntity 39, Task_MoveTowardPoint 36); small-data set: lev 254 (Camera_TrackEntityZoom 46, Camera_SnapToEntity 41, Task_SpawnChildNode 40) |
| `main/script/Task_NodePool.c` | cc1 `-G8`: lev 10 (Task_AllocNode 10); cc1 `-fno-strength-reduce`: lev 7 (Task_InitNodePool 4, Task_RelocBlock 3); maspsx `-G8`: lev 51 (Task_InitNodePool 13, Task_AllocNode 11, Task_RelocBlock 8); maspsx `--use-comm-section`: lev 17 (Task_InitNodePool 5, Task_AllocNode 4, Task_RelocBlock 2); maspsx `--dont-expand-li`: identical; FORCE_G0: lev 12 (Task_GarbageCollectNodes 12); small-data set: lev 57 (Task_AllocNode 17, Task_InitNodePool 13, Task_RelocBlock 8) |
| `main/script/Task_SystemOpcodes.c` | maspsx `-G8`: lev 67 (Entity_InitWithTimeout 16, Task_WaitEntityFlags8 11, Task_WaitFrameOrDraw 8); maspsx `--use-comm-section`: lev 24 (Entity_InitWithTimeout 5, Task_WaitEntityFlags8 4, Task_WaitFrameOrDraw 3); FORCE_G0: identical; small-data set: lev 67 (Entity_InitWithTimeout 16, Task_WaitEntityFlags8 11, Task_WaitFrameOrDraw 8) |
| `main/script/Task_ValueOps.c` | maspsx `--expand-div`: lev 41 (Task_EvalExpr 41) |
| `overlays/boot_display/Boot_StepImageSprite.c` | maspsx `--expand-div`: lev 22 (Boot_StepImageSprite 22) |
| `overlays/fx_common/FxCommon_DrawPolyResource.c` | maspsx `--dont-expand-li`: identical |
| `overlays/fx_common/FxCommon_EffectInitializationFlow.c` | cc1 `-fno-expensive-optimizations`: lev 28 (func_80196498 28) |
| `overlays/fx_common/FxCommon_SetMotionEndpoints.c` | cc1 `-fno-schedule-insns`: lev 56 (FxCommon_SetMotionEndpoints 56) |
| `overlays/fx_common/FxCommon_UpdateResource.c` | cc1 `-fno-force-addr`: identical |
| `overlays/fx_common/camera_nodes.c` | maspsx `--expand-div`: lev 165 (FxCommon_CheckFourBoundsWithMargin 88, FxCommon_CheckBoundsWithMargin 44, func_8018F55C 33) |
| `overlays/fx_common/render_op.c` | cc1 `-G8`: lev 27 (func_80191EFC 15, func_80191E30 12); small-data set: lev 27 (func_80191EFC 15, func_80191E30 12) |
| `overlays/menu_memcard/Memcard_FlipScreenPages.c` | cc1 `-fno-cse-skip-blocks`: lev 12 (Memcard_FlipScreenPages 12) |
| `overlays/menu_memcard/Memcard_ListAndScreenFlow.c` | cc1 `-fno-cse-skip-blocks`: lev 71 (func_801909B4 59, func_80190660 12) |
| `overlays/room_lib/RoomEffect_DroppedFlare.c` | maspsx `--expand-div`: lev 63 (RoomEffect_DroppedFlareParticle 63) |
| `overlays/room_lib/RoomEffect_LineBurst.c` | maspsx `--expand-div`: lev 36 (RoomEffect_LineBurstController 36) |
| `overlays/room_lib/RoomEffect_SeekerSparkCallback.c` | maspsx `--expand-div`: identical |
| `overlays/room_lib/RoomEffect_SparkAttacks.c` | maspsx `--expand-div`: lev 37 (RoomEffect_BeamSparkController 37) |
| `overlays/room_lib/RoomEffect_TrailBurstAttacks.c` | maspsx `--expand-div`: lev 66 (RoomEffect_SeekingTrailParticle 66) |
| `overlays/room_lib/RoomFx_DropFieldEffects.c` | maspsx `--expand-div`: identical |
| `overlays/room_lib/RoomFx_GrabAttackSet.c` | maspsx `--expand-div`: lev 17 (RoomFx_RisingSprayController 17) |
| `overlays/room_lib/RoomFx_MirrorModel.c` | maspsx `--expand-div`: lev 29 (RoomFx_ModelUpdate 29) |
| `overlays/room_lib/RoomFx_MotionParticles_2.c` | cc1 `-fno-strength-reduce`: lev 66 (RoomFx_DrawMotionParticles 66) |
| `overlays/room_lib/RoomFx_OrbitEffectSet.c` | cc1 `-fno-strength-reduce`: lev 179 (RoomFx_UpdateOrbitBurst 38, RoomFx_DrawOrbitBurst 33, RoomFx_InitOrbitParticles 30) |
| `overlays/room_lib/RoomLib_ActorClasses.c` | maspsx `--expand-div`: lev 194 (RoomLib_HandlerE 58, RoomLib_HandlerC 33, RoomLib_AdvanceArcToTargetY 28) |
| `overlays/room_lib/RoomLib_FloorWalkerClass.c` | maspsx `--expand-div`: lev 10 (RoomLib_UpdateHandlerG 10) |
| `overlays/room_lib/RoomLib_WindowClass.c` | maspsx `--expand-div`: lev 20 (RoomLib_WindowHandler 20) |
| `overlays/room_m023_theater/m023.c` | maspsx `--expand-div`: lev 16 (func_8018F710 16) |
| `overlays/room_m256_museum/m256_swing.c` | maspsx `--expand-div`: lev 14 (func_80192680 14) |
| `overlays/room_m273_liberty/RoomM273_PulseSpriteCallback.c` | cc1 `-fno-schedule-insns`: lev 24 (RoomM273_PulseSpriteCallback 24) |
| `overlays/room_m273_liberty/m273.c` | maspsx `--expand-div`: lev 13 (func_80192F5C 13) |
| `overlays/room_m273_liberty/m273_boss_sway.c` | maspsx `--expand-div`: lev 17 (func_80197BBC 17) |
| `overlays/room_m318_chrysler/m318.c` | maspsx `--expand-div`: lev 22 (func_801955E4 22) |
| `overlays/room_m349_cruiser/m349.c` | maspsx `--expand-div`: lev 47 (func_8018F7C4 47) |
| `overlays/room_m350_cruiser/RoomEffect_ConfiguredPulseCallback.c` | cc1 `-G8`: lev 7 (func_80196C64 7); small-data set: lev 7 (func_80196C64 7) |
| `overlays/room_m350_cruiser/m350.c` | maspsx `--expand-div`: lev 56 (func_80195CE0 21, func_8019421C 18, func_80192508 17) |
| `overlays/room_m350_cruiser/m350_callbacks.c` | maspsx `--expand-div`: lev 52 (func_80198ABC 18, func_801992A0 17, func_80197B98 17) |
| `overlays/scene_e18_cruiser/SceneE18_ExpandingRingCallback.c` | cc1 `-fno-cse-skip-blocks`: identical |
| `overlays/scene_e18_cruiser/e18_ring_emitters.c` | maspsx `--expand-div`: lev 17 (func_801935F0 17) |
| `overlays/scene_e19_cruiser/e19.c` | maspsx `--expand-div`: lev 133 (func_80192F9C 133) |
| `overlays/scene_e20_cruiser/RoomEffect_HoverOrbController_8018F750.c` | maspsx `--expand-div`: lev 69 (func_8018F750 69); cc1 `-ffixed-10`: lev 6 (func_8018F750 6); cc1 `-ffixed-11`: lev 6 (func_8018F750 6); cc1 `-ffixed-15`: lev 6 (func_8018F750 6); cc1 `-ffixed-21`: lev 74 (func_8018F750 74); cc1 `-ffixed-22`: lev 74 (func_8018F750 74); cc1 `-ffixed-23`: lev 74 (func_8018F750 74); cc1 `-ffixed-24`: lev 6 (func_8018F750 6); cc1 `-ffixed-25`: lev 6 (func_8018F750 6) |
| `overlays/scene_e22_liberty/e22.c` | maspsx `--expand-div`: lev 44 (func_80198348 44) |
