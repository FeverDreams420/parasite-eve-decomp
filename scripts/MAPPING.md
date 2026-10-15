# Parasite Eve 1 Script VM — Central Mapping Reference

## 1. Addressing Modes (`Script_RunQueue`)

| Mode | Syntax | Backing Storage in C | Range |
| :--- | :--- | :--- | :--- |
| `0` | Immediate literal (`123`, `10.5fp`, `0x100`, `@label`) | Inline 32-bit operand (`&values[i]`) | 32-bit signed/unsigned |
| `1` | `local[idx]` | Actor entity struct `((u8 *)g_CurrentEntity + 0xAC + idx * 4)` | `0..55` |
| `2` | `global[idx]` / `global[NAME]` | Persistent save/story bank `g_ScriptVariableBank[idx]` | `0..511` |
| `3` | `reg[idx]` (folded into expressions) | Expression evaluation stack `g_ScriptOperandTable[idx]` | `0..15` |
| `4` | `room[idx]` | Room-scoped shared variables `g_ScriptNodeActiveFlags[idx]` | `0..63` |

## 2. Named Global Variables (`g_ScriptVariableBank`)

| Index | Name | Usage |
| :--- | :--- | :--- |
| `0x00` (`0`) | `global[STORY_FLAGS]` | Story / room / event progression state |
| `0x01` (`1`) | `global[ROOM]` | Story / room / event progression state |
| `0x03` (`3`) | `global[FLAGS]` | Story / room / event progression state |
| `0x18` (`24`) | `global[SCENE_FLAGS]` | Story / room / event progression state |
| `0x19` (`25`) | `global[EVENT_FLAGS]` | Story / room / event progression state |
| `0x4A` (`74`) | `global[AREA]` | Story / room / event progression state |

## 3. Symbolic Enum Constants

### `vec3_kind`
| Value | Constant |
| :--- | :--- |
| `0` | `VEC3_POS` |
| `1` | `VEC3_BASE_POS` |
| `2` | `VEC3_MOTION` |
| `3` | `VEC3_ACCEL` |
| `4` | `VEC3_GRAVITY` |
| `5` | `VEC3_ROT` |
| `6` | `VEC3_DELTA` |

### `event_slot`
| Value | Constant |
| :--- | :--- |
| `1` | `EVENT_TOUCH` |
| `2` | `EVENT_INTERACT` |
| `3` | `EVENT_DEFERRED` |

### `pad_mode`
| Value | Constant |
| :--- | :--- |
| `0` | `PAD_HELD` |
| `1` | `PAD_PRESSED` |
| `2` | `PAD_RELEASED` |
| `3` | `PAD_HELD_FRAMES` |

### `query_field`
| Value | Constant |
| :--- | :--- |
| `0` | `ACTOR_MODE` |
| `1` | `ACTOR_FLAGS` |
| `2` | `ACTOR_ANIM_FRAME` |
| `3` | `ACTOR_ACTION` |

## 4. Expression Operators (`0x09 Script_EvalExpr`)

| EOP | Operator / Function | Kind | Precedence |
| :--- | :--- | :--- | :--- |
| `0` | `+` | Binary infix | `10` |
| `1` | `-` | Binary infix | `10` |
| `2` | `|` | Binary infix | `4` |
| `3` | `&` | Binary infix | `6` |
| `4` | `^` | Binary infix | `5` |
| `5` | `||` | Binary infix | `2` |
| `6` | `&&` | Binary infix | `3` |
| `7` | `!` | Unary prefix | `13` |
| `8` | `~` | Unary prefix | `13` |
| `9` | `>` | Binary infix | `8` |
| `10` | `<` | Binary infix | `8` |
| `11` | `==` | Binary infix | `7` |
| `12` | `>=` | Binary infix | `8` |
| `13` | `<=` | Binary infix | `8` |
| `14` | `!=` | Binary infix | `7` |
| `15` | `*` | Binary infix | `11` |
| `16` | `/` | Binary infix | `11` |
| `17` | `<<` | Binary infix | `9` |
| `18` | `>>` | Binary infix | `9` |
| `19` | `mov` | Function | `14` |
| `20` | `fixed_mul` | Function | `14` |
| `21` | `fixed_div` | Function | `14` |
| `22` | `%` | Binary infix | `11` |
| `23` | `-` | Unary prefix | `13` |

## 5. Scene VM Opcodes (220 Opcodes: `0x00..0xF0`)

| Opcode | DSL Name | C Handler Symbol | Arguments | Description |
| :--- | :--- | :--- | :--- | :--- |
| `0x00` (`0`) | `Jump` | `Script_JumpToEntityOffset` | `label` | Unconditional jump to @label |
| `0x01` (`1`) | `DestroySelf` | `Entity_MarkCurrentDead` | `—` | Mark current actor dead (flags |= 0x10) and yield |
| `0x02` (`2`) | `Wait` | `Entity_SetCurrentNodeActive` | `int` | Yield current script thread for N frames |
| `0x03` (`3`) | `SetFogNearFar` | `Script_SetFogParams` | `int, int, int` | Set fog near/far parameters |
| `0x04` (`4`) | `KillOtherThreads` | `Entity_MarkNodeFree` | `—` | Terminate all other script threads on current actor |
| `0x05` (`5`) | `JumpIfZero` | `Script_JumpIfZero` | `int, label` | Jump to @label if condition == 0 |
| `0x08` (`8`) | `SpawnActor` | `Script_SpawnEntityAt` | `int, int, fixed, fixed, fixed` | Spawn actor (type_id, sub_id) at (x, y, z) |
| `0x09` (`9`) | `EvalExpr` | `Script_EvalExpr` | `int, int, int, int` | Evaluate arithmetic/logical expression |
| `0x0A` (`10`) | `Assign` | `Script_AssignValue` | `int, int` | Assign value to destination variable |
| `0x0B` (`11`) | `SetEntityVec3` | `Script_SetEntityVec3` | `vec3_kind, fixed_if_pos, fixed_if_pos, fixed_if_pos` | Set 3D vector on current actor (pos, motion, rot, etc.) |
| `0x0C` (`12`) | `GetEntityVec3` | `Script_GetEntityVec3` | `vec3_kind, int, int, int` | Read 3D vector from current actor into (out_x, out_y, out_z) |
| `0x0D` (`13`) | `OpenTextbox` | `Script_StopEntityAnim` | `int` | Open dialogue textbox for page_id |
| `0x0E` (`14`) | `QueryActorField` | `Entity_QueryField` | `query_field, int, int, int` | Query (field, type_id, sub_id, out_val) |
| `0x11` (`17`) | `CheckPadInput` | `Script_DispatchCmd` | `pad_mode, hex, int` | Test controller button state (mode, mask, out_result) |
| `0x12` (`18`) | `SpawnThread` | `Script_SpawnChildNode` | `label` | Spawn concurrent child thread on current actor at @label |
| `0x13` (`19`) | `SetActorFlags` | `Entity_SetCurrentFlags` | `hex` | OR mask into current actor flags |
| `0x14` (`20`) | `SetEventHandler` | `Entity_SelectFieldSetter` | `event_slot, label_or_neg` | Bind actor event callback (EVENT_TOUCH / EVENT_INTERACT / EVENT_DEFERRED) |
| `0x15` (`21`) | `TestPlayerInQuad` | `Script_PlayerPointInPoly` | `fixed, fixed, fixed, fixed, fixed, fixed, fixed, fixed, int` | Test if player is inside 4-vertex quad |
| `0x16` (`22`) | `WaitMoveComplete` | `Entity_YieldWhileMoving` | `—` | Yield while current actor movement flag (0x2) is set |
| `0x17` (`23`) | `JumpToTargetParabolic` | `Entity_ComputeVelocityToTarget` | `fixed, fixed, fixed, int` | Launch actor toward (x, y, z) over N frames with gravity |
| `0x18` (`24`) | `KillThread` | `Entity_FindNodeById` | `int` | Terminate script thread by thread seq ID |
| `0x19` (`25`) | `GetThreadId` | `Entity_GetCurrentNodeId` | `int` | Read current script thread seq ID into out_id |
| `0x1A` (`26`) | `Random` | `Script_Random` | `int, int, int` | Generate random integer in [min, max) into out_val |
| `0x1B` (`27`) | `SetMoveSpeed` | `Script_SetEntityMoveSpeed` | `hex` | Set current actor move_speed |
| `0x1C` (`28`) | `PlayFieldSfx` | `Script_PlayFieldSfx` | `int, int, int` | Queue field sound effect (type_id, sub_id, arg) |
| `0x1D` (`29`) | `CondBranch` | `Script_CondBranch` | `int, int, label` | Branch to @label if lhs != rhs (switch-case test) |
| `0x1E` (`30`) | `EnableActorCollision` | `Entity_SetCurrentFlag80` | `—` | Set actor flag 0x80 |
| `0x1F` (`31`) | `GetThreadTarget14` | `Script_GetNodeTarget14` | `int` | Read current script thread target14 into out_val |
| `0x20` (`32`) | `ExitThread` | `Entity_MarkCurrentNodeFree` | `—` | Terminate current script thread and yield |
| `0x22` (`34`) | `WaitTextbox` | `Script_YieldWhileTableEntrySet` | `int` | Yield while textbox page_id is active |
| `0x23` (`35`) | `CloseTextbox` | `Script_ClearTableEntry` | `int` | Close textbox page_id |
| `0x24` (`36`) | `GetThreadTargets18_1C` | `Script_GetNodeTargets18_1C` | `int, int` | Read current thread target18 and target1c into (out_a, out_b) |
| `0x25` (`37`) | `GetActorSerialId` | `Script_GetCurrentEntityField24` | `int` | Read current actor field_sfx_id (serial) into out_id |
| `0x26` (`38`) | `GetSubId` | `Script_GetCurrentEntityFieldD` | `int` | Read current actor sub_id into out_sub_id |
| `0x27` (`39`) | `SetSubId` | `Script_SetCurrentEntityFieldD` | `int` | Set current actor sub_id |
| `0x28` (`40`) | `ClearBit` | `Script_ClearBit` | `int, int` | Clear bit N in target variable (*arg0 &= ~(1 << *arg1)) |
| `0x29` (`41`) | `TestBit` | `Script_TestBit` | `int, int, int` | Test bit N in variable (*arg2 = *arg0 & (1 << *arg1)) |
| `0x2A` (`42`) | `SetBit` | `Script_SetBit` | `int, int` | Set bit N in target variable (*arg0 |= (1 << *arg1)) |
| `0x2B` (`43`) | `UnlockPlayerMoveMask` | `Entity_ClearGlobalFlags` | `hex` | Clear bits in g_FieldMoveLock |
| `0x2C` (`44`) | `GetPlayerMoveLock` | `Entity_GetGlobalFlags` | `int` | Read g_FieldMoveLock into out_mask |
| `0x2D` (`45`) | `LockPlayerMoveMask` | `Entity_SetGlobalFlags` | `hex` | Set bits in g_FieldMoveLock |
| `0x2E` (`46`) | `SetAnim` | `Entity_SetCurrentActionMode` | `int` | Set current actor animation/action mode |
| `0x2F` (`47`) | `PlayAnimToFrame` | `Script_SetEntityAnimFrame` | `int` | Play current animation up to target frame |
| `0x30` (`48`) | `WaitAnimEnd` | `Script_WaitAnimFrameMatch` | `—` | Yield until animation reaches anim_frame_target |
| `0x31` (`49`) | `ChangeRoom` | `Script_EncodeBarcode` | `hex` | Request room transition with dispatch token |
| `0x32` (`50`) | `DisableActorCollision` | `Entity_ClearCurrentFlag80` | `—` | Clear actor flag 0x80 |
| `0x35` (`53`) | `ReloadSceneData` | `Script_LoadSceneData` | `int` | Reload scene data block |
| `0x36` (`54`) | `CameraTrackActorZoom` | `Render_CameraTrackEntityZoom` | `int, int, int` | Track actor with camera zoom |
| `0x37` (`55`) | `SetActorFlag4000` | `Entity_SetCurrentFlag4000` | `—` | Set actor flag 0x4000 |
| `0x39` (`57`) | `ClearActorFlag4000` | `Entity_ClearCurrentFlag4000` | `—` | Clear actor flag 0x4000 |
| `0x3E` (`62`) | `SetHitCylinderRadius` | `Script_SetEntityField224` | `int` | Set actor hit_cylinder.radius |
| `0x3F` (`63`) | `UnlockPlayerInput` | `Entity_ClearGlobalFlag1` | `—` | Clear bit 0 of g_FieldMoveLock |
| `0x40` (`64`) | `LockPlayerInput` | `Entity_SetGlobalFlag1` | `—` | Set bit 0 of g_FieldMoveLock |
| `0x41` (`65`) | `HideActor` | `Entity_SetCurrentFlag40` | `—` | Set actor flag 0x40 (hidden/inactive) |
| `0x42` (`66`) | `ShowActor` | `Entity_ClearCurrentFlag40` | `—` | Clear actor flag 0x40 |
| `0x43` (`67`) | `GetChoiceResult` | `Script_GetMenuChoice` | `int` | Read selected dialogue/menu choice index into out_choice |
| `0x44` (`68`) | `MoveTowardPoint` | `Script_MoveTowardPoint` | `fixed, fixed, int` | Move actor toward (x, z) turning by step; yield until reached |
| `0x45` (`69`) | `GetAngleToActor` | `Script_GetAngleToEntity` | `int, int, int` | Compute yaw angle to actor (type_id, sub_id) into out_angle |
| `0x46` (`70`) | `SetScrollMode` | `Script_SetFogParamsAlt` | `int, int, int` | Configure camera scroll mode (Render_SetScrollMode) |
| `0x47` (`71`) | `IsBattlePhase4` | `Script_IsBattlePhase4` | `int` | Check if battle render state phase == 4 |
| `0x48` (`72`) | `ResetCameraToPlayer` | `Script_ResetPlayerCameraPos` | `—` | Snap camera scroll position to player |
| `0x49` (`73`) | `SetBattleRenderMode80` | `Script_SetBattleStateMode` | `—` | Set battle render state flag 0x80 |
| `0x4A` (`74`) | `TestActorFlags` | `Entity_TestCurrentFlags` | `hex, int` | Test mask in current actor flags into out_bool |
| `0x4B` (`75`) | `CameraSnapToActor` | `Render_CameraSnapToEntity` | `int, int, int` | Snap/pan camera to actor (type_id, sub_id, speed) |
| `0x4C` (`76`) | `Noop` | `Script_Noop` | `—` | No operation |
| `0x4D` (`77`) | `SetAnimStep` | `Script_SetEntityField1C` | `hex` | Set current actor anim_step (16.16 fixed speed) |
| `0x4E` (`78`) | `SetAnimFrame` | `Script_SetEntityAnimSpeed` | `int` | Set current actor animation frame immediately |
| `0x4F` (`79`) | `PauseActorAnim` | `Entity_SetCurrentFlag100` | `—` | Set actor flag 0x100 (pause animation) |
| `0x50` (`80`) | `ResumeActorAnim` | `Entity_ClearCurrentFlag100` | `—` | Clear actor flag 0x100 |
| `0x51` (`81`) | `TestGameStateFlags` | `Scene_TestGlobalFlags` | `hex, int` | Test mask in g_GameStateFlags into out_bool |
| `0x52` (`82`) | `SetGameStateFlags` | `Scene_SetGlobalFlags` | `hex` | OR mask into g_GameStateFlags |
| `0x53` (`83`) | `ClearGameStateFlags` | `Scene_ClearGlobalFlags` | `hex` | Clear mask from g_GameStateFlags |
| `0x54` (`84`) | `GetDistanceToActor` | `Entity_GetDistanceComponents` | `int, int, int` | Compute 2D distance to actor (type_id, sub_id) into out_dist |
| `0x55` (`85`) | `MemCardStepInit` | `Boot_MemCardStepInit` | `int` | Step memory card initialization state machine |
| `0x59` (`89`) | `GetBattleField` | `Entity_GetField` | `int, int` | Read battle/actor context field (field_id, out_val) |
| `0x5A` (`90`) | `SetBattleField` | `Entity_SetField` | `int, int` | Write battle/actor context field (field_id, val) |
| `0x5B` (`91`) | `ClearRenderFlagsC0` | `Script_ClearBattleFlags` | `—` | Clear bits 0xC0 in g_RenderStateFlags |
| `0x5C` (`92`) | `SetRenderFlagsC0` | `Script_SetBattleFlags` | `—` | Set bits 0xC0 in g_RenderStateFlags |
| `0x5D` (`93`) | `SetMoveFactor` | `Script_SetCurrentEntityField20` | `hex` | Set current actor move_factor (16.16 fixed) |
| `0x5E` (`94`) | `GetActorPositionByType` | `Entity_GetPositionByType` | `int, int, int, int, int, int, int` | Read position/rotation of actor (type_id, sub_id) into outputs |
| `0x5F` (`95`) | `SuspendOtherThreads` | `Entity_MarkAnimPending` | `—` | Mark all other threads on current actor as pending |
| `0x60` (`96`) | `ResumeOtherThreads` | `Entity_AdvancePendingAnim` | `—` | Resume pending threads on current actor |
| `0x61` (`97`) | `CameraTrackRelativeOffset` | `Render_CameraTrackRelativeOffset` | `fixed, fixed, int` | Track relative camera offset |
| `0x62` (`98`) | `SuspendThreadById` | `Entity_MarkNodeAnimPendingById` | `int` | Suspend thread by seq ID |
| `0x63` (`99`) | `ResumeThreadById` | `Entity_AdvanceNodeById` | `int` | Resume suspended thread by seq ID |
| `0x64` (`100`) | `TriggerAnimEvent` | `Script_RunEntityCommand` | `int, int` | Trigger battle/entity anim event (cmd, out_result) |
| `0x65` (`101`) | `StopActorMotion` | `Entity_ClearMotionVectors` | `—` | Zero motion_x/y/z and accel_x/y/z on current actor |
| `0x66` (`102`) | `ApplyForwardVelocity` | `Entity_ComputeVelocity` | `—` | Compute motion_x/z from current rot_y and move_speed |
| `0x67` (`103`) | `SetGameTimer` | `Script_ComputeAudioTimer` | `int, int, int, int` | Configure countdown timer 3 (hours, mins, secs, mode) |
| `0x68` (`104`) | `IsGameTimerExpired` | `Script_GetCombatModeFlag` | `int` | Check if game timer 3 flag 0x4 is set |
| `0x69` (`105`) | `SetGameTimerFlag4` | `Script_SetCombatModeFlag` | `—` | Set flag 0x4 on game timer 3 |
| `0x6A` (`106`) | `LoadRoomAssets` | `Script_GetEntityEffect` | `int, int` | Load room asset slot (slot, out_status) |
| `0x6B` (`107`) | `SendPmCommand0` | `Pm_ScriptSendCommand0` | `int, int, int, int, int` | Send PM process command variant 0 |
| `0x6C` (`108`) | `SendPmCommand1` | `Pm_ScriptSendCommand1` | `int, int, int, int, int` | Send PM process command variant 1 |
| `0x6D` (`109`) | `SetPmState` | `Pm_ScriptSetState` | `int, int` | Set PM slot state |
| `0x6E` (`110`) | `GetPmState` | `Pm_ScriptGetState` | `int, int` | Get PM slot state |
| `0x6F` (`111`) | `AllocBattleActorSlot` | `Script_InitEntityMoveState` | `—` | Allocate battle slot for current actor (Entity_AllocSlot) |
| `0x70` (`112`) | `SetupModelPartAnim11` | `Script_SetEntityAnim12Args` | `int, int, int, int, int, int, int, int, int, int, int` | Configure 11-arg model part animation |
| `0x71` (`113`) | `SetGeomEntryPos` | `Script_SetSceneEntryAnim` | `int, int, int, int` | Transform geometry entry point (index, x, y, z) |
| `0x72` (`114`) | `SetGeomEntryVisible` | `Script_SetRenderEntryEnabled` | `int, int` | Set geometry entry visibility (index, visible) |
| `0x73` (`115`) | `ClipGeomEntryPoint` | `Script_SetRenderEntryPos` | `int, int, int` | Clip geometry entry point |
| `0x74` (`116`) | `SetGeomEntryLimit` | `Script_SetObjEntryLimit` | `int, int` | Set geometry entry frame limit |
| `0x75` (`117`) | `SetGeomFlagSlot` | `Script_SetFlagSlot` | `int, int` | Set geometry flag slot |
| `0x76` (`118`) | `TurnToAngleStep` | `Render_CameraStepAngleFade` | `int, int` | Turn actor rot_y toward target_angle by step; yield until reached |
| `0x77` (`119`) | `TestPointInQuad` | `Script_PointInPoly` | `fixed, fixed, fixed, fixed, fixed, fixed, fixed, fixed, fixed, fixed, int` | Test if point (arg8, arg9) lies in 4-vertex quad (arg0..7) -> arg10 |
| `0x78` (`120`) | `EnableActorRendering` | `Entity_ClearCurrentFlag20` | `—` | Clear actor flag 0x20 |
| `0x79` (`121`) | `DisableActorRendering` | `Entity_SetCurrentFlag20` | `—` | Set actor flag 0x20 |
| `0x7A` (`122`) | `GetGeomEntryField6` | `Script_GetObjEntryField6` | `int, int` | Read geometry entry field 6 into out_val |
| `0x7B` (`123`) | `SetGeomEntryField8` | `Script_SetObjEntryField8` | `int, int` | Set geometry entry field 8 |
| `0x7C` (`124`) | `FillGeomEntrySlots` | `Script_FillObjEntrySlotValues` | `int, int` | Fill geometry entry slot values |
| `0x7D` (`125`) | `SetGeomEntrySlotValue` | `Script_SetObjEntrySlotValue` | `int, int, int` | Set geometry entry slot value |
| `0x7E` (`126`) | `SetGeomEntryFlags` | `Script_SetObjEntryFlags` | `int, hex` | Set flags on geometry entry |
| `0x7F` (`127`) | `ClearGeomEntryFlags` | `Script_ClearObjEntryFlags` | `int, hex` | Clear flags on geometry entry |
| `0x80` (`128`) | `DisableCollisionTriangle` | `Script_SetObjEntryFlag80` | `int` | Set flag 0x80 on collision triangle (disable walk/collide) |
| `0x81` (`129`) | `EnableCollisionTriangle` | `Script_ClearObjEntryFlag80` | `int` | Clear flag 0x80 on collision triangle (enable walk/collide) |
| `0x82` (`130`) | `LoadGeomState` | `Script_LoadGeomState` | `int` | Load room geometry state slot |
| `0x83` (`131`) | `WaitAnimCrossFrame` | `Script_WaitAnimRange` | `—` | Yield until animation crosses anim_frame_target |
| `0x84` (`132`) | `SetCameraBaseAngle` | `Script_SetScreenScrollPos` | `int, int` | Set digital camera base angles (x, y) |
| `0x85` (`133`) | `StartScreenFade` | `Script_SetCdRomSeekPos` | `int` | Start screen fade/tint transition |
| `0x86` (`134`) | `SetFadeColor` | `Script_SetFogColor` | `hex` | Set screen fade RGB color word |
| `0x87` (`135`) | `StartScreenFadeRect` | `Script_SetCdRomScreenPos` | `int, int, int, int, int` | Configure screen fade parameters |
| `0x88` (`136`) | `ClearScreenFadeFlag40` | `Script_ClearEntityFlag40` | `—` | Clear bit 0x40 in g_ScreenTint.fadeMode |
| `0x89` (`137`) | `SetBattleModeState6` | `Menu_SetModeState6` | `—` | Set g_BattleModeState = 6 |
| `0x8A` (`138`) | `SetBattleModeState5` | `Menu_SetModeState5` | `—` | Set g_BattleModeState = 5 |
| `0x8B` (`139`) | `GetActorBattleField` | `Entity_FindByTypeId` | `int, int, int, int` | Read battle context field from actor (type_id, sub_id, field_id, out_val) |
| `0x8C` (`140`) | `WaitSeqElapsedOrDraw` | `Script_WaitFrameOrDraw` | `int` | Yield until sequence elapsed matches arg0 or draw finishes |
| `0x8D` (`141`) | `Noop1` | `Script_Noop1` | `int` | No-op taking 1 argument |
| `0x8E` (`142`) | `Noop2` | `Script_Noop2` | `—` | No-op |
| `0x8F` (`143`) | `WaitDrawIdle` | `Entity_YieldOnDrawBusy` | `—` | Yield while GPU draw status is busy |
| `0x90` (`144`) | `Noop4` | `Script_Noop4` | `—` | No-op |
| `0x91` (`145`) | `Noop3` | `Script_Noop3` | `—` | No-op |
| `0x92` (`146`) | `PlayModelAnimAndWait` | `Script_PlayEntityAnim` | `int` | Play model animation and yield until completion |
| `0x93` (`147`) | `SetAnimInterpRate` | `Script_SetAnimInterpRate` | `int` | Set animation interpolation rate |
| `0x94` (`148`) | `GetBattleModeState` | `Menu_GetModeState` | `int` | Read g_BattleModeState into out_state |
| `0x95` (`149`) | `ClearBattleModeState` | `Menu_ClearModeState` | `—` | Set g_BattleModeState = 0 |
| `0x96` (`150`) | `SetBattleModeState8` | `Menu_SetModeState8` | `—` | Set g_BattleModeState = 8 |
| `0x97` (`151`) | `StopPmSlotOnSelf` | `Pm_ScriptStopCurrentEntity` | `int` | Stop PM task slot on current actor |
| `0x98` (`152`) | `AttachToActor` | `Script_SetEntityTarget` | `int, int` | Link current actor to target actor (type_id, sub_id) |
| `0x99` (`153`) | `StopAllPmSlots` | `Script_ResetRenderState` | `—` | Stop all active PM task slots (Pm_StopAllBoth) |
| `0x9A` (`154`) | `SetMenuSlotEntry` | `Script_SetMenuSlotEntry` | `int, int, int` | Set menu slot entry parameters |
| `0x9B` (`155`) | `SetHitCylinder` | `Script_SetGteMatrix` | `int, int, int` | Set actor collision cylinder (radius, height, offset_y) |
| `0x9C` (`156`) | `WaitScreenFade` | `Entity_YieldOnLoading` | `—` | Yield while screen fade transition is active |
| `0x9D` (`157`) | `ScaleShadowRadius` | `Script_ScaleAnimValue` | `hex` | Scale actor shadow radius by 16.16 factor |
| `0xA4` (`164`) | `CopyActorRenderObject` | `Script_CopyEntityRenderObject` | `int, int, int` | Copy render object from actor (type_id, sub_id, mode) |
| `0xA5` (`165`) | `WaitDiscIdle` | `Boot_CheckModeSwitch` | `—` | Yield until CD-ROM / scene streaming reaches idle mode |
| `0xA6` (`166`) | `ClampMenuRange` | `Script_ClampMenuRange` | `int` | Clamp menu range |
| `0xA7` (`167`) | `GiveItem` | `Script_GiveItem` | `int, int` | Add item_id to inventory and store result in out_status |
| `0xA8` (`168`) | `CountItemInInventory` | `Script_CountItemsByValue` | `int, int` | Count occurrences of item_id in inventory -> out_count |
| `0xA9` (`169`) | `RunSubSceneWithTimeout` | `Entity_InitWithTimeout` | `int, int` | Run sub-scene / menu flow (mode, out_result) |
| `0xAA` (`170`) | `PauseSceneEvents` | `Script_SetSceneFlag2000` | `—` | Set bit 0x2000 in g_GameStateFlags |
| `0xAB` (`171`) | `ResumeSceneEvents` | `Script_ClearSceneFlag2000` | `—` | Clear bit 0x2000 in g_GameStateFlags |
| `0xAC` (`172`) | `SetWallCollisionPolygon` | `Script_SetAnimSlotPointer` | `poly_label, int` | Register wall collision line/polygon table (@poly, count) |
| `0xAD` (`173`) | `UnlockPlayerMoveBit2` | `Script_ClearInputFlagBit2` | `—` | Clear bit 2 (0x4) in g_FieldMoveLock |
| `0xAE` (`174`) | `LockPlayerMoveBit2` | `Script_SetInputFlagBit2` | `—` | Set bit 2 (0x4) in g_FieldMoveLock |
| `0xAF` (`175`) | `ResolveActorPosition` | `Entity_ResolveCurrentPosition` | `int` | Resolve current actor position on floor/collision mesh |
| `0xB0` (`176`) | `SetModelPartCoords` | `Script_SetBattleEntryCoords` | `int, int, int` | Set model part attachment coordinates (part, x, y) |
| `0xB1` (`177`) | `SelectWorldMapDestination` | `Scene_SelectMapById` | `int` | Open world map destination selector |
| `0xB2` (`178`) | `GetEquipSlotState` | `Script_GetFloorNumber` | `int` | Read Menu_GetEquipSlotStateOrIndex into out_val |
| `0xB3` (`179`) | `SelectFloorByRoll` | `Script_SetFloorByEntityRoll` | `int` | Select floor/map entry |
| `0xB4` (`180`) | `EnablePlayerMovement` | `Script_SetMoveFlag` | `—` | Enable player movement in textbox/scene |
| `0xB5` (`181`) | `DisablePlayerMovement` | `Script_ClearMoveFlag` | `—` | Disable player movement in textbox/scene |
| `0xB6` (`182`) | `OffsetPosForward` | `Entity_PolarToPosition2` | `int, int, int` | Offset actor pos along current yaw by radius -> (out_x, out_z) |
| `0xB7` (`183`) | `SetupModelPartAnim5` | `Script_SetEntityAnim5Args` | `int, int, int, int, int` | Configure 5-arg model part animation |
| `0xB8` (`184`) | `TurnTowardPoint` | `Script_TurnTowardPointStep` | `fixed, fixed, int` | Turn actor yaw toward (x, z) by step per frame; yield until facing |
| `0xB9` (`185`) | `ClearActorFlag1` | `Script_ClearEntityFlag1` | `—` | Clear bit 0 of actor flags |
| `0xBA` (`186`) | `SetActorFlag1` | `Script_SetEntityFlag1` | `—` | Set bit 0 of actor flags |
| `0xBB` (`187`) | `SetBgEntryScrolled` | `Script_SetEntryScrolled` | `int, int, int, int` | Configure scrolling background entry |
| `0xBC` (`188`) | `SetBgEntryMirrored` | `Script_SetEntryMirrored` | `int, int, int, int` | Configure mirrored background entry |
| `0xBD` (`189`) | `SetRenderTintParams` | `Script_SetRenderFlag8Params` | `int, int, int` | Set render object tint parameters and flag 0x8 |
| `0xBE` (`190`) | `ClearRenderFlag8` | `Script_ClearRenderFlag8` | `—` | Clear flag 0x8 on actor render_object |
| `0xBF` (`191`) | `SetRenderAlphaValue` | `Script_SetRenderFlag10Value` | `int` | Set render_object value9a and flag 0x10 |
| `0xC0` (`192`) | `ClearRenderFlag10` | `Script_ClearRenderFlag10` | `—` | Clear flag 0x10 on actor render_object |
| `0xC1` (`193`) | `SetActorFlag400` | `Script_SetEntityFlag400` | `—` | Set actor flag 0x400 |
| `0xC2` (`194`) | `ClearActorFlag400` | `Script_ClearEntityFlag400` | `—` | Clear actor flag 0x400 |
| `0xC3` (`195`) | `WaitActorFlag8` | `Script_WaitEntityFlags8` | `—` | Yield until actor flag 0x8 is set |
| `0xC4` (`196`) | `RotateActorYaw` | `Script_RotateEntityAngle` | `int` | Add delta to actor rot_y |
| `0xC5` (`197`) | `CopyAttachmentData` | `Entity_CopyAttachmentData` | `int, int, int` | Copy attachment transform data from actor |
| `0xC6` (`198`) | `PlayAnimOnceAndWait` | `Script_SetEntityActionAndWait` | `int` | Set actor animation mode and yield until one cycle finishes |
| `0xC7` (`199`) | `SetThreadFlag80` | `Script_SetNodeFlag80` | `—` | Set flag 0x80 on current script thread |
| `0xC8` (`200`) | `ClearActorRoamFlag` | `Script_ClearEntityRoamFlag` | `—` | Clear actor flag 0x20000 |
| `0xC9` (`201`) | `SetActorRoamFlag` | `Script_SetEntityRoamFlag` | `—` | Set actor flag 0x20000 |
| `0xCA` (`202`) | `SetRenderTableIndex` | `Script_SetEntityField1E6` | `int` | Set actor render_object.table_index |
| `0xCB` (`203`) | `GetAngleToPoint` | `Script_GetAngleToPoint` | `fixed, fixed, int` | Compute yaw angle from actor to (x, z) into out_angle |
| `0xCC` (`204`) | `ResetActorRenderObject` | `Script_ResetEntityRenderObj` | `—` | Clear object animation and parent link on current actor |
| `0xCD` (`205`) | `SnapToSpawnPosition` | `Script_SnapPosToSpawnPos` | `—` | Reset actor pos to initial spawn coordinates |
| `0xCE` (`206`) | `SetActorBattleField` | `Entity_SetFieldByTypeId` | `int, int, int, int` | Write battle context field on actor (type_id, sub_id, field_id, val) |
| `0xCF` (`207`) | `SignalSceneExit` | `Script_SignalSceneExit` | `—` | Signal current scene exit |
| `0xD0` (`208`) | `SetTextboxRect` | `Script_DrawSaveOverlay` | `int, int, int, int` | Set custom textbox rectangle (x, y, width, height) |
| `0xD1` (`209`) | `OpenStyledTextbox` | `Script_ResetEntityAnimSlot` | `int` | Open styled textbox (style=1) for page_id in custom rect |
| `0xD2` (`210`) | `GetGameTimeCounter` | `Script_GetFloorTableValue` | `int, int` | Read GAME_TIME_COUNTER(index) into out_val |
| `0xD3` (`211`) | `ConvertFramesToHMS` | `GameTime_ConvertSecondsToHMS` | `int, int, int, int` | Convert frame count to (hours, minutes, seconds) |
| `0xD4` (`212`) | `SetParentActor` | `Script_SetEntityParentLink` | `int, int` | Link current actor parent to (type_id, sub_id) |
| `0xD5` (`213`) | `DetachFromParent` | `Script_ClearEntityParentLink` | `—` | Clear parent link on current actor |
| `0xD6` (`214`) | `DetachAllChildren` | `Entity_ClearParentLink` | `—` | Detach all actors whose parent is current actor |
| `0xD7` (`215`) | `Sin` | `Script_GetSin` | `int, int` | Compute rsin(angle) << 4 into out_val |
| `0xD8` (`216`) | `Cos` | `Script_GetCos` | `int, int` | Compute rcos(angle) << 4 into out_val |
| `0xD9` (`217`) | `Atan2` | `Script_GetAtan2` | `int, int, int` | Compute ratan2(y, x) into out_angle |
| `0xDA` (`218`) | `Sqrt` | `Script_GetSqrt` | `int, int` | Compute integer square root into out_val |
| `0xDB` (`219`) | `CameraTrackActorAngle` | `Render_CameraTrackEntityAngle` | `int, int, int` | Track actor angle with camera |
| `0xDC` (`220`) | `SetActorFlag1000000` | `Script_SetEntityFlag1000000` | `—` | Set actor flag 0x1000000 |
| `0xDD` (`221`) | `PolarOffsetToPos` | `Entity_PolarToPosition` | `int, int, int, int` | Offset actor position by (radius, angle) -> (out_x, out_z) |
| `0xDE` (`222`) | `PopCount` | `Entity_PopCount` | `int, int` | Count set bits in arg0 -> arg1 |
| `0xDF` (`223`) | `EnableActorGravity` | `Script_EnableEntityGravity` | `—` | Set actor flag 0x2 |
| `0xE0` (`224`) | `DisableActorGravity` | `Script_DisableEntityGravity` | `—` | Clear actor flag 0x2 |
| `0xE1` (`225`) | `SetMeshFadeLevel` | `Script_SetMeshFadeLevel` | `int` | Set g_RenderMeshTintLevel |
| `0xE2` (`226`) | `TestPointInPolygon` | `Entity_CallAction` | `fixed, fixed, int, poly_label, int` | Test if (x, z) is inside polygon(count, @poly) -> out_inside |
| `0xE3` (`227`) | `SetWalkableFloorPolygon` | `Script_SetInventorySlotPointer` | `poly_label, int` | Register walkable floor polygon (@poly, vertex_count) |
| `0xE4` (`228`) | `GetInventoryTotal` | `Script_GetInventoryTotal` | `int` | Read total inventory item count into out_total |
| `0xE5` (`229`) | `GetInventorySlotLimit` | `Script_GetInventorySlotLimit` | `int` | Read Aya inventory slot capacity into out_limit |
| `0xE6` (`230`) | `SetBgEntryScreenPos` | `Script_SetActorScreenPos` | `int, int, int` | Set 2D screen position of background entry (index, x, y) |
| `0xE7` (`231`) | `OpenMemCardDialog` | `Script_OpenMemCardDialog` | `—` | Open memory card dialog screen and yield until closed |
| `0xE8` (`232`) | `ShowReceivedItemPopup` | `Script_ShowReceivedItem` | `int` | Show received item popup dialog and yield until closed |
| `0xE9` (`233`) | `RunInventoryCommand` | `Script_RunInvCommand` | `int, int, int, int, int` | Execute inventory/menu command and yield while active |
| `0xEA` (`234`) | `AkaoCmd` | `Akao_ProcessCommand` | `hex, int, int, int, int, int` | Dispatch AKAO sound/music/stream command |
| `0xEB` (`235`) | `EnableActorShadow` | `Script_SetEntityShadowFlag` | `—` | Set actor shadow flag 0x20000000 |
| `0xEC` (`236`) | `DisableActorShadow` | `Script_ClearEntityShadowFlag` | `—` | Clear actor shadow flag 0x20000000 |
| `0xED` (`237`) | `SceneCmd` | `Script_DispatchScriptCmd` | `int, int, int, int, int, int, int, int` | Dispatch scene/light/sprite command (2100..3200) |
| `0xEE` (`238`) | `OpenEquipMenu` | `Script_BeginSceneTransition` | `int` | Open equipment menu screen and yield until closed |
| `0xEF` (`239`) | `OpenSaveLoadMenu` | `Script_OpenSaveLoadMenu` | `int` | Open save/load menu screen and yield until closed |
| `0xF0` (`240`) | `GetPlayerInputFlag` | `Script_GetPlayerInputFlag` | `int` | Read player input status flag into out_flag |

## 6. FieldFx VM Opcodes (`FieldFx_RunScript` in `src/main/fieldfx/object_script.c`)

Each instruction is a 32-bit word `(op << 16) | (operand & 0xFFFF)`. After executing a non-halting instruction, `SCRIPT->pc++` runs, so relative branch offsets `(s8)(op & 0xFF)` target `target_pc = pc + rel + 1`.

| Word / High 16-bit `op` | DSL Syntax | C Semantics (`FieldFx_RunScript`) |
| :--- | :--- | :--- |
| `0xFFFFFFFF` (`-1`) | `halt;` | `SCRIPT->halted = 1; stopped = 1;` |
| `0xFFFFFFFE` (`-2`) | `nop;` | No-op (`command = 0`) |
| `0xFFFFFFFD` (`-3`) | `free_all_slots;` | `FieldFx_FreeAllSlots()` |
| `0x0001` | `start_slot(slot_id);` | `FieldFx_StartSlot(obj, (u8)operand, sizes, init_handlers)` |
| `0x0002` | `wait(frames);` | `SCRIPT->wait = operand; stopped = 1;` |
| `0x0010..0x001F` | `rN = imm16;` | `SCRIPT->regs[op - 0x10] = (s16)operand;` |
| `0x0020..0x002F` | `rN += imm16;` | `SCRIPT->regs[op - 0x20] += (s16)operand;` |
| `0x0030..0x003F` | `rN -= imm16;` | `SCRIPT->regs[op - 0x30] -= (s16)operand;` |
| `0x0040..0x004F` | `rN = vars[idx];` | `SCRIPT->regs[op - 0x40] = SCRIPT->vars[(u16)operand];` |
| `0x0050..0x005F` | `vars[idx] = rN;` | `SCRIPT->vars[(u16)operand] = SCRIPT->regs[op - 0x50];` |
| `0x1000..0x1FFF` | `if (rN == imm16) goto @label;` | `if (SCRIPT->regs[(op >> 8) & 0xF] == (s16)operand) SCRIPT->pc += (s8)op;` |
| `0x2000..0x2FFF` | `if (rN != imm16) goto @label;` | `if (SCRIPT->regs[(op >> 8) & 0xF] != (s16)operand) SCRIPT->pc += (s8)op;` |
| `0x3000..0x3FFF` | `goto @label;` | `SCRIPT->pc += (s8)op;` |
