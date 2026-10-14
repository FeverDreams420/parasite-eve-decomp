"""Central mapping of Parasite Eve 1 script opcodes, variables, and enums.

Covers both virtual machines in Parasite Eve 1:
1. Scene / Actor Task Script VM (`Script_RunQueue` in `src/main/script/`, 241 opcodes)
2. Room Effect / FieldFx Script VM (`FieldFx_RunScript` in `src/main/fieldfx/object_script.c`)
"""

from __future__ import annotations
from dataclasses import dataclass

# ---------------------------------------------------------------------------
# Addressing modes in `Script_RunQueue` (3 bits per argument)
# ---------------------------------------------------------------------------
MODE_IMM = 0     # Immediate 32-bit literal (&values[i])
MODE_LOCAL = 1   # Actor local variable: ((u8 *)g_CurrentEntity + 0xAC + idx * 4) [0..55]
MODE_GLOBAL = 2  # Persistent global variable: g_ScriptVariableBank[idx] [0..511]
MODE_REG = 3     # Temporary expression register: g_ScriptOperandTable[idx] [0..15]
MODE_ROOM = 4    # Room-shared variable: g_ScriptNodeActiveFlags[idx] [0..63]

# ---------------------------------------------------------------------------
# Named global variables in `g_ScriptVariableBank` (`global[...]`)
# ---------------------------------------------------------------------------
GLOBAL_VARS: dict[int, str] = {
    0x00: "STORY_FLAGS",
    0x01: "ROOM",
    0x03: "FLAGS",
    0x18: "SCENE_FLAGS",
    0x19: "EVENT_FLAGS",
    0x4A: "AREA",
}
GLOBAL_VARS_BY_NAME: dict[str, int] = {v: k for k, v in GLOBAL_VARS.items()}

# ---------------------------------------------------------------------------
# Expression operators in `0x09 Script_EvalExpr`
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class ExprOpSpec:
    code: int
    symbol: str
    precedence: int
    is_unary: bool = False
    is_func: bool = False
    direct_local_dst: bool = False


EXPR_OPS: dict[int, ExprOpSpec] = {
    0:  ExprOpSpec(0,  "+",         10),
    1:  ExprOpSpec(1,  "-",         10),
    2:  ExprOpSpec(2,  "|",          4),
    3:  ExprOpSpec(3,  "&",          6),
    4:  ExprOpSpec(4,  "^",          5),
    5:  ExprOpSpec(5,  "||",         2),
    6:  ExprOpSpec(6,  "&&",         3),
    7:  ExprOpSpec(7,  "!",         13, is_unary=True),
    8:  ExprOpSpec(8,  "~",         13, is_unary=True),
    9:  ExprOpSpec(9,  ">",          8),
    10: ExprOpSpec(10, "<",          8),
    11: ExprOpSpec(11, "==",         7),
    12: ExprOpSpec(12, ">=",         8),
    13: ExprOpSpec(13, "<=",         8),
    14: ExprOpSpec(14, "!=",         7),
    15: ExprOpSpec(15, "*",         11),
    16: ExprOpSpec(16, "/",         11),
    17: ExprOpSpec(17, "<<",         9),
    18: ExprOpSpec(18, ">>",         9),
    19: ExprOpSpec(19, "mov",       14, is_unary=True, is_func=True),
    20: ExprOpSpec(20, "fixed_mul", 14, is_func=True, direct_local_dst=True),
    21: ExprOpSpec(21, "fixed_div", 14, is_func=True, direct_local_dst=True),
    22: ExprOpSpec(22, "%",         11),
    23: ExprOpSpec(23, "-",         13, is_unary=True),
}

BINARY_SYMBOL_TO_EOP: dict[str, int] = {
    spec.symbol: code
    for code, spec in EXPR_OPS.items()
    if not spec.is_unary and not spec.is_func
}

UNARY_SYMBOL_TO_EOP: dict[str, int] = {
    "!": 7,
    "~": 8,
    "-": 23,
}

# ---------------------------------------------------------------------------
# Named enum constants for multi-mode opcode arguments
# ---------------------------------------------------------------------------
ENUM_GROUPS: dict[str, dict[int, str]] = {
    "vec3_kind": {
        0: "VEC3_POS",
        1: "VEC3_BASE_POS",
        2: "VEC3_MOTION",
        3: "VEC3_ACCEL",
        4: "VEC3_GRAVITY",
        5: "VEC3_ROT",
        6: "VEC3_DELTA",
    },
    "event_slot": {
        1: "EVENT_TOUCH",
        2: "EVENT_INTERACT",
        3: "EVENT_DEFERRED",
    },
    "pad_mode": {
        0: "PAD_HELD",
        1: "PAD_PRESSED",
        2: "PAD_RELEASED",
        3: "PAD_HELD_FRAMES",
    },
    "query_field": {
        0: "ACTOR_MODE",
        1: "ACTOR_FLAGS",
        2: "ACTOR_ANIM_FRAME",
        3: "ACTOR_ACTION",
    },
}

ALL_ENUM_CONSTANTS: dict[str, int] = {}
for _group in ENUM_GROUPS.values():
    for _val, _name in _group.items():
        ALL_ENUM_CONSTANTS[_name] = _val


# ---------------------------------------------------------------------------
# Complete 241-entry Scene VM Opcode Table (`0x00..0xF0`)
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class OpcodeSpec:
    opcode: int
    name: str
    c_symbol: str
    argc: int
    arg_types: tuple[str, ...]
    doc: str = ""


def _op(
    opcode: int,
    name: str,
    c_symbol: str,
    arg_types: tuple[str, ...] = (),
    doc: str = "",
) -> OpcodeSpec:
    return OpcodeSpec(opcode, name, c_symbol, len(arg_types), arg_types, doc)


OPCODE_SPECS: list[OpcodeSpec] = [
    _op(0x00, "Jump",                      "Script_JumpToEntityOffset",       ("label",),                                                       "Unconditional jump to @label"),
    _op(0x01, "DestroySelf",               "Entity_MarkCurrentDead",          (),                                                               "Mark current actor dead (flags |= 0x10) and yield"),
    _op(0x02, "Wait",                      "Entity_SetCurrentNodeActive",     ("int",),                                                         "Yield current script thread for N frames"),
    _op(0x03, "SetFogNearFar",             "Script_SetFogParams",             ("int", "int", "int"),                                            "Set fog near/far parameters"),
    _op(0x04, "KillOtherThreads",          "Entity_MarkNodeFree",             (),                                                               "Terminate all other script threads on current actor"),
    _op(0x05, "JumpIfZero",                "Script_JumpIfZero",               ("int", "label"),                                                 "Jump to @label if condition == 0"),
    _op(0x08, "SpawnActor",                "Script_SpawnEntityAt",            ("int", "int", "fixed", "fixed", "fixed"),                        "Spawn actor (type_id, sub_id) at (x, y, z)"),
    _op(0x09, "EvalExpr",                  "Script_EvalExpr",                 ("int", "int", "int", "int"),                                     "Evaluate arithmetic/logical expression"),
    _op(0x0A, "Assign",                    "Script_AssignValue",              ("int", "int"),                                                   "Assign value to destination variable"),
    _op(0x0B, "SetEntityVec3",             "Script_SetEntityVec3",            ("vec3_kind", "fixed_if_pos", "fixed_if_pos", "fixed_if_pos"),     "Set 3D vector on current actor (pos, motion, rot, etc.)"),
    _op(0x0C, "GetEntityVec3",             "Script_GetEntityVec3",            ("vec3_kind", "int", "int", "int"),                               "Read 3D vector from current actor into (out_x, out_y, out_z)"),
    _op(0x0D, "OpenTextbox",               "Script_StopEntityAnim",           ("int",),                                                         "Open dialogue textbox for page_id"),
    _op(0x0E, "QueryActorField",           "Entity_QueryField",               ("query_field", "int", "int", "int"),                             "Query (field, type_id, sub_id, out_val)"),
    _op(0x11, "CheckPadInput",             "Script_DispatchCmd",              ("pad_mode", "hex", "int"),                                       "Test controller button state (mode, mask, out_result)"),
    _op(0x12, "SpawnThread",               "Script_SpawnChildNode",           ("label",),                                                       "Spawn concurrent child thread on current actor at @label"),
    _op(0x13, "SetActorFlags",             "Entity_SetCurrentFlags",          ("hex",),                                                         "OR mask into current actor flags"),
    _op(0x14, "SetEventHandler",           "Entity_SelectFieldSetter",        ("event_slot", "label_or_neg"),                                   "Bind actor event callback (EVENT_TOUCH / EVENT_INTERACT / EVENT_DEFERRED)"),
    _op(0x15, "TestPlayerInQuad",          "Script_PlayerPointInPoly",        ("fixed", "fixed", "fixed", "fixed", "fixed", "fixed", "fixed", "fixed", "int"), "Test if player is inside 4-vertex quad"),
    _op(0x16, "WaitMoveComplete",          "Entity_YieldWhileMoving",         (),                                                               "Yield while current actor movement flag (0x2) is set"),
    _op(0x17, "JumpToTargetParabolic",     "Entity_ComputeVelocityToTarget",  ("fixed", "fixed", "fixed", "int"),                               "Launch actor toward (x, y, z) over N frames with gravity"),
    _op(0x18, "KillThread",                "Entity_FindNodeById",             ("int",),                                                         "Terminate script thread by thread seq ID"),
    _op(0x19, "GetThreadId",               "Entity_GetCurrentNodeId",         ("int",),                                                         "Read current script thread seq ID into out_id"),
    _op(0x1A, "Random",                    "Script_Random",                   ("int", "int", "int"),                                            "Generate random integer in [min, max) into out_val"),
    _op(0x1B, "SetMoveSpeed",              "Script_SetEntityMoveSpeed",       ("hex",),                                                         "Set current actor move_speed"),
    _op(0x1C, "PlayFieldSfx",              "Script_PlayFieldSfx",             ("int", "int", "int"),                                            "Queue field sound effect (type_id, sub_id, arg)"),
    _op(0x1D, "CondBranch",                "Script_CondBranch",               ("int", "int", "label"),                                          "Branch to @label if lhs != rhs (switch-case test)"),
    _op(0x1E, "EnableActorCollision",      "Entity_SetCurrentFlag80",         (),                                                               "Set actor flag 0x80"),
    _op(0x1F, "GetThreadTarget14",         "Script_GetNodeTarget14",          ("int",),                                                         "Read current script thread target14 into out_val"),
    _op(0x20, "ExitThread",                "Entity_MarkCurrentNodeFree",      (),                                                               "Terminate current script thread and yield"),
    _op(0x22, "WaitTextbox",               "Script_YieldWhileTableEntrySet",  ("int",),                                                         "Yield while textbox page_id is active"),
    _op(0x23, "CloseTextbox",              "Script_ClearTableEntry",          ("int",),                                                         "Close textbox page_id"),
    _op(0x24, "GetThreadTargets18_1C",     "Script_GetNodeTargets18_1C",      ("int", "int"),                                                   "Read current thread target18 and target1c into (out_a, out_b)"),
    _op(0x25, "GetActorSerialId",          "Script_GetCurrentEntityField24",  ("int",),                                                         "Read current actor field_sfx_id (serial) into out_id"),
    _op(0x26, "GetSubId",                  "Script_GetCurrentEntityFieldD",   ("int",),                                                         "Read current actor sub_id into out_sub_id"),
    _op(0x27, "SetSubId",                  "Script_SetCurrentEntityFieldD",   ("int",),                                                         "Set current actor sub_id"),
    _op(0x28, "ClearBit",                  "Script_ClearBit",                 ("int", "int"),                                                   "Clear bit N in target variable (*arg0 &= ~(1 << *arg1))"),
    _op(0x29, "TestBit",                   "Script_TestBit",                  ("int", "int", "int"),                                            "Test bit N in variable (*arg2 = *arg0 & (1 << *arg1))"),
    _op(0x2A, "SetBit",                    "Script_SetBit",                   ("int", "int"),                                                   "Set bit N in target variable (*arg0 |= (1 << *arg1))"),
    _op(0x2B, "UnlockPlayerMoveMask",      "Entity_ClearGlobalFlags",         ("hex",),                                                         "Clear bits in g_FieldMoveLock"),
    _op(0x2C, "GetPlayerMoveLock",         "Entity_GetGlobalFlags",           ("int",),                                                         "Read g_FieldMoveLock into out_mask"),
    _op(0x2D, "LockPlayerMoveMask",        "Entity_SetGlobalFlags",           ("hex",),                                                         "Set bits in g_FieldMoveLock"),
    _op(0x2E, "SetAnim",                   "Entity_SetCurrentActionMode",     ("int",),                                                         "Set current actor animation/action mode"),
    _op(0x2F, "PlayAnimToFrame",           "Script_SetEntityAnimFrame",       ("int",),                                                         "Play current animation up to target frame"),
    _op(0x30, "WaitAnimEnd",               "Script_WaitAnimFrameMatch",       (),                                                               "Yield until animation reaches anim_frame_target"),
    _op(0x31, "ChangeRoom",                "Script_EncodeBarcode",            ("hex",),                                                         "Request room transition with dispatch token"),
    _op(0x32, "DisableActorCollision",     "Entity_ClearCurrentFlag80",       (),                                                               "Clear actor flag 0x80"),
    _op(0x35, "ReloadSceneData",           "Script_LoadSceneData",            ("int",),                                                         "Reload scene data block"),
    _op(0x36, "CameraTrackActorZoom",      "Render_CameraTrackEntityZoom",    ("int", "int", "int"),                                            "Track actor with camera zoom"),
    _op(0x37, "SetActorFlag4000",          "Entity_SetCurrentFlag4000",       (),                                                               "Set actor flag 0x4000"),
    _op(0x39, "ClearActorFlag4000",        "Entity_ClearCurrentFlag4000",     (),                                                               "Clear actor flag 0x4000"),
    _op(0x3E, "SetHitCylinderRadius",      "Script_SetEntityField224",        ("int",),                                                         "Set actor hit_cylinder.radius"),
    _op(0x3F, "UnlockPlayerInput",         "Entity_ClearGlobalFlag1",         (),                                                               "Clear bit 0 of g_FieldMoveLock"),
    _op(0x40, "LockPlayerInput",           "Entity_SetGlobalFlag1",           (),                                                               "Set bit 0 of g_FieldMoveLock"),
    _op(0x41, "HideActor",                 "Entity_SetCurrentFlag40",         (),                                                               "Set actor flag 0x40 (hidden/inactive)"),
    _op(0x42, "ShowActor",                 "Entity_ClearCurrentFlag40",       (),                                                               "Clear actor flag 0x40"),
    _op(0x43, "GetChoiceResult",           "Script_GetMenuChoice",            ("int",),                                                         "Read selected dialogue/menu choice index into out_choice"),
    _op(0x44, "MoveTowardPoint",           "Script_MoveTowardPoint",          ("fixed", "fixed", "int"),                                        "Move actor toward (x, z) turning by step; yield until reached"),
    _op(0x45, "GetAngleToActor",           "Script_GetAngleToEntity",         ("int", "int", "int"),                                            "Compute yaw angle to actor (type_id, sub_id) into out_angle"),
    _op(0x46, "SetScrollMode",             "Script_SetFogParamsAlt",          ("int", "int", "int"),                                            "Configure camera scroll mode (Render_SetScrollMode)"),
    _op(0x47, "IsBattlePhase4",            "Script_IsBattlePhase4",           ("int",),                                                         "Check if battle render state phase == 4"),
    _op(0x48, "ResetCameraToPlayer",       "Script_ResetPlayerCameraPos",     (),                                                               "Snap camera scroll position to player"),
    _op(0x49, "SetBattleRenderMode80",     "Script_SetBattleStateMode",       (),                                                               "Set battle render state flag 0x80"),
    _op(0x4A, "TestActorFlags",            "Entity_TestCurrentFlags",         ("hex", "int"),                                                   "Test mask in current actor flags into out_bool"),
    _op(0x4B, "CameraSnapToActor",         "Render_CameraSnapToEntity",       ("int", "int", "int"),                                            "Snap/pan camera to actor (type_id, sub_id, speed)"),
    _op(0x4C, "Noop",                      "Script_Noop",                     (),                                                               "No operation"),
    _op(0x4D, "SetAnimStep",               "Script_SetEntityField1C",         ("hex",),                                                         "Set current actor anim_step (16.16 fixed speed)"),
    _op(0x4E, "SetAnimFrame",              "Script_SetEntityAnimSpeed",       ("int",),                                                         "Set current actor animation frame immediately"),
    _op(0x4F, "PauseActorAnim",            "Entity_SetCurrentFlag100",        (),                                                               "Set actor flag 0x100 (pause animation)"),
    _op(0x50, "ResumeActorAnim",           "Entity_ClearCurrentFlag100",      (),                                                               "Clear actor flag 0x100"),
    _op(0x51, "TestGameStateFlags",        "Scene_TestGlobalFlags",           ("hex", "int"),                                                   "Test mask in g_GameStateFlags into out_bool"),
    _op(0x52, "SetGameStateFlags",         "Scene_SetGlobalFlags",            ("hex",),                                                         "OR mask into g_GameStateFlags"),
    _op(0x53, "ClearGameStateFlags",       "Scene_ClearGlobalFlags",          ("hex",),                                                         "Clear mask from g_GameStateFlags"),
    _op(0x54, "GetDistanceToActor",        "Entity_GetDistanceComponents",    ("int", "int", "int"),                                            "Compute 2D distance to actor (type_id, sub_id) into out_dist"),
    _op(0x55, "MemCardStepInit",           "Boot_MemCardStepInit",            ("int",),                                                         "Step memory card initialization state machine"),
    _op(0x59, "GetBattleField",            "Entity_GetField",                 ("int", "int"),                                                   "Read battle/actor context field (field_id, out_val)"),
    _op(0x5A, "SetBattleField",            "Entity_SetField",                 ("int", "int"),                                                   "Write battle/actor context field (field_id, val)"),
    _op(0x5B, "ClearRenderFlagsC0",        "Script_ClearBattleFlags",         (),                                                               "Clear bits 0xC0 in g_RenderStateFlags"),
    _op(0x5C, "SetRenderFlagsC0",          "Script_SetBattleFlags",           (),                                                               "Set bits 0xC0 in g_RenderStateFlags"),
    _op(0x5D, "SetMoveFactor",             "Script_SetCurrentEntityField20",  ("hex",),                                                         "Set current actor move_factor (16.16 fixed)"),
    _op(0x5E, "GetActorPositionByType",    "Entity_GetPositionByType",        ("int", "int", "int", "int", "int", "int", "int"),                "Read position/rotation of actor (type_id, sub_id) into outputs"),
    _op(0x5F, "SuspendOtherThreads",       "Entity_MarkAnimPending",          (),                                                               "Mark all other threads on current actor as pending"),
    _op(0x60, "ResumeOtherThreads",        "Entity_AdvancePendingAnim",       (),                                                               "Resume pending threads on current actor"),
    _op(0x61, "CameraTrackRelativeOffset", "Render_CameraTrackRelativeOffset",("fixed", "fixed", "int"),                                        "Track relative camera offset"),
    _op(0x62, "SuspendThreadById",         "Entity_MarkNodeAnimPendingById",  ("int",),                                                         "Suspend thread by seq ID"),
    _op(0x63, "ResumeThreadById",          "Entity_AdvanceNodeById",          ("int",),                                                         "Resume suspended thread by seq ID"),
    _op(0x64, "TriggerAnimEvent",          "Script_RunEntityCommand",         ("int", "int"),                                                   "Trigger battle/entity anim event (cmd, out_result)"),
    _op(0x65, "StopActorMotion",           "Entity_ClearMotionVectors",       (),                                                               "Zero motion_x/y/z and accel_x/y/z on current actor"),
    _op(0x66, "ApplyForwardVelocity",      "Entity_ComputeVelocity",          (),                                                               "Compute motion_x/z from current rot_y and move_speed"),
    _op(0x67, "SetGameTimer",              "Script_ComputeAudioTimer",        ("int", "int", "int", "int"),                                     "Configure countdown timer 3 (hours, mins, secs, mode)"),
    _op(0x68, "IsGameTimerExpired",        "Script_GetCombatModeFlag",        ("int",),                                                         "Check if game timer 3 flag 0x4 is set"),
    _op(0x69, "SetGameTimerFlag4",         "Script_SetCombatModeFlag",        (),                                                               "Set flag 0x4 on game timer 3"),
    _op(0x6A, "LoadRoomAssets",            "Script_GetEntityEffect",          ("int", "int"),                                                   "Load room asset slot (slot, out_status)"),
    _op(0x6B, "SendPmCommand0",            "Pm_ScriptSendCommand0",           ("int", "int", "int", "int", "int"),                              "Send PM process command variant 0"),
    _op(0x6C, "SendPmCommand1",            "Pm_ScriptSendCommand1",           ("int", "int", "int", "int", "int"),                              "Send PM process command variant 1"),
    _op(0x6D, "SetPmState",                "Pm_ScriptSetState",               ("int", "int"),                                                   "Set PM slot state"),
    _op(0x6E, "GetPmState",                "Pm_ScriptGetState",               ("int", "int"),                                                   "Get PM slot state"),
    _op(0x6F, "AllocBattleActorSlot",      "Script_InitEntityMoveState",      (),                                                               "Allocate battle slot for current actor (Entity_AllocSlot)"),
    _op(0x70, "SetupModelPartAnim11",      "Script_SetEntityAnim12Args",      ("int", "int", "int", "int", "int", "int", "int", "int", "int", "int", "int"), "Configure 11-arg model part animation"),
    _op(0x71, "SetGeomEntryPos",           "Script_SetSceneEntryAnim",        ("int", "int", "int", "int"),                                     "Transform geometry entry point (index, x, y, z)"),
    _op(0x72, "SetGeomEntryVisible",       "Script_SetRenderEntryEnabled",    ("int", "int"),                                                   "Set geometry entry visibility (index, visible)"),
    _op(0x73, "ClipGeomEntryPoint",        "Script_SetRenderEntryPos",        ("int", "int", "int"),                                            "Clip geometry entry point"),
    _op(0x74, "SetGeomEntryLimit",         "Script_SetObjEntryLimit",         ("int", "int"),                                                   "Set geometry entry frame limit"),
    _op(0x75, "SetGeomFlagSlot",           "Script_SetFlagSlot",              ("int", "int"),                                                   "Set geometry flag slot"),
    _op(0x76, "TurnToAngleStep",           "Render_CameraStepAngleFade",      ("int", "int"),                                                   "Turn actor rot_y toward target_angle by step; yield until reached"),
    _op(0x77, "TestPointInQuad",           "Script_PointInPoly",              ("fixed", "fixed", "fixed", "fixed", "fixed", "fixed", "fixed", "fixed", "fixed", "fixed", "int"), "Test if point (arg8, arg9) lies in 4-vertex quad (arg0..7) -> arg10"),
    _op(0x78, "EnableActorRendering",      "Entity_ClearCurrentFlag20",       (),                                                               "Clear actor flag 0x20"),
    _op(0x79, "DisableActorRendering",     "Entity_SetCurrentFlag20",         (),                                                               "Set actor flag 0x20"),
    _op(0x7A, "GetGeomEntryField6",        "Script_GetObjEntryField6",        ("int", "int"),                                                   "Read geometry entry field 6 into out_val"),
    _op(0x7B, "SetGeomEntryField8",        "Script_SetObjEntryField8",        ("int", "int"),                                                   "Set geometry entry field 8"),
    _op(0x7C, "FillGeomEntrySlots",        "Script_FillObjEntrySlotValues",   ("int", "int"),                                                   "Fill geometry entry slot values"),
    _op(0x7D, "SetGeomEntrySlotValue",     "Script_SetObjEntrySlotValue",     ("int", "int", "int"),                                            "Set geometry entry slot value"),
    _op(0x7E, "SetGeomEntryFlags",         "Script_SetObjEntryFlags",         ("int", "hex"),                                                   "Set flags on geometry entry"),
    _op(0x7F, "ClearGeomEntryFlags",       "Script_ClearObjEntryFlags",       ("int", "hex"),                                                   "Clear flags on geometry entry"),
    _op(0x80, "DisableCollisionTriangle",  "Script_SetObjEntryFlag80",        ("int",),                                                         "Set flag 0x80 on collision triangle (disable walk/collide)"),
    _op(0x81, "EnableCollisionTriangle",   "Script_ClearObjEntryFlag80",      ("int",),                                                         "Clear flag 0x80 on collision triangle (enable walk/collide)"),
    _op(0x82, "LoadGeomState",             "Script_LoadGeomState",            ("int",),                                                         "Load room geometry state slot"),
    _op(0x83, "WaitAnimCrossFrame",        "Script_WaitAnimRange",            (),                                                               "Yield until animation crosses anim_frame_target"),
    _op(0x84, "SetCameraBaseAngle",        "Script_SetScreenScrollPos",       ("int", "int"),                                                   "Set digital camera base angles (x, y)"),
    _op(0x85, "StartScreenFade",           "Script_SetCdRomSeekPos",          ("int",),                                                         "Start screen fade/tint transition"),
    _op(0x86, "SetFadeColor",              "Script_SetFogColor",              ("hex",),                                                         "Set screen fade RGB color word"),
    _op(0x87, "StartScreenFadeRect",       "Script_SetCdRomScreenPos",        ("int", "int", "int", "int", "int"),                              "Configure screen fade parameters"),
    _op(0x88, "ClearScreenFadeFlag40",     "Script_ClearEntityFlag40",        (),                                                               "Clear bit 0x40 in g_ScreenTint.fadeMode"),
    _op(0x89, "SetBattleModeState6",       "Menu_SetModeState6",              (),                                                               "Set g_BattleModeState = 6"),
    _op(0x8A, "SetBattleModeState5",       "Menu_SetModeState5",              (),                                                               "Set g_BattleModeState = 5"),
    _op(0x8B, "GetActorBattleField",       "Entity_FindByTypeId",             ("int", "int", "int", "int"),                                     "Read battle context field from actor (type_id, sub_id, field_id, out_val)"),
    _op(0x8C, "WaitSeqElapsedOrDraw",      "Script_WaitFrameOrDraw",          ("int",),                                                         "Yield until sequence elapsed matches arg0 or draw finishes"),
    _op(0x8D, "Noop1",                     "Script_Noop1",                    ("int",),                                                         "No-op taking 1 argument"),
    _op(0x8E, "Noop2",                     "Script_Noop2",                    (),                                                               "No-op"),
    _op(0x8F, "WaitDrawIdle",              "Entity_YieldOnDrawBusy",          (),                                                               "Yield while GPU draw status is busy"),
    _op(0x90, "Noop4",                     "Script_Noop4",                    (),                                                               "No-op"),
    _op(0x91, "Noop3",                     "Script_Noop3",                    (),                                                               "No-op"),
    _op(0x92, "PlayModelAnimAndWait",      "Script_PlayEntityAnim",           ("int",),                                                         "Play model animation and yield until completion"),
    _op(0x93, "SetAnimInterpRate",         "Script_SetAnimInterpRate",        ("int",),                                                         "Set animation interpolation rate"),
    _op(0x94, "GetBattleModeState",        "Menu_GetModeState",               ("int",),                                                         "Read g_BattleModeState into out_state"),
    _op(0x95, "ClearBattleModeState",      "Menu_ClearModeState",             (),                                                               "Set g_BattleModeState = 0"),
    _op(0x96, "SetBattleModeState8",       "Menu_SetModeState8",              (),                                                               "Set g_BattleModeState = 8"),
    _op(0x97, "StopPmSlotOnSelf",          "Pm_ScriptStopCurrentEntity",      ("int",),                                                         "Stop PM task slot on current actor"),
    _op(0x98, "AttachToActor",             "Script_SetEntityTarget",          ("int", "int"),                                                   "Link current actor to target actor (type_id, sub_id)"),
    _op(0x99, "StopAllPmSlots",            "Script_ResetRenderState",         (),                                                               "Stop all active PM task slots (Pm_StopAllBoth)"),
    _op(0x9A, "SetMenuSlotEntry",          "Script_SetMenuSlotEntry",         ("int", "int", "int"),                                            "Set menu slot entry parameters"),
    _op(0x9B, "SetHitCylinder",            "Script_SetGteMatrix",             ("int", "int", "int"),                                            "Set actor collision cylinder (radius, height, offset_y)"),
    _op(0x9C, "WaitScreenFade",            "Entity_YieldOnLoading",           (),                                                               "Yield while screen fade transition is active"),
    _op(0x9D, "ScaleShadowRadius",         "Script_ScaleAnimValue",           ("hex",),                                                         "Scale actor shadow radius by 16.16 factor"),
    _op(0xA4, "CopyActorRenderObject",     "Script_CopyEntityRenderObject",   ("int", "int", "int"),                                            "Copy render object from actor (type_id, sub_id, mode)"),
    _op(0xA5, "WaitDiscIdle",              "Boot_CheckModeSwitch",            (),                                                               "Yield until CD-ROM / scene streaming reaches idle mode"),
    _op(0xA6, "ClampMenuRange",            "Script_ClampMenuRange",           ("int",),                                                         "Clamp menu range"),
    _op(0xA7, "GiveItem",                  "Script_GiveItem",                 ("int", "int"),                                                   "Add item_id to inventory and store result in out_status"),
    _op(0xA8, "CountItemInInventory",      "Script_CountItemsByValue",        ("int", "int"),                                                   "Count occurrences of item_id in inventory -> out_count"),
    _op(0xA9, "RunSubSceneWithTimeout",    "Entity_InitWithTimeout",          ("int", "int"),                                                   "Run sub-scene / menu flow (mode, out_result)"),
    _op(0xAA, "PauseSceneEvents",          "Script_SetSceneFlag2000",         (),                                                               "Set bit 0x2000 in g_GameStateFlags"),
    _op(0xAB, "ResumeSceneEvents",         "Script_ClearSceneFlag2000",       (),                                                               "Clear bit 0x2000 in g_GameStateFlags"),
    _op(0xAC, "SetWallCollisionPolygon",   "Script_SetAnimSlotPointer",       ("poly_label", "int"),                                            "Register wall collision line/polygon table (@poly, count)"),
    _op(0xAD, "UnlockPlayerMoveBit2",      "Script_ClearInputFlagBit2",       (),                                                               "Clear bit 2 (0x4) in g_FieldMoveLock"),
    _op(0xAE, "LockPlayerMoveBit2",        "Script_SetInputFlagBit2",         (),                                                               "Set bit 2 (0x4) in g_FieldMoveLock"),
    _op(0xAF, "ResolveActorPosition",      "Entity_ResolveCurrentPosition",   ("int",),                                                         "Resolve current actor position on floor/collision mesh"),
    _op(0xB0, "SetModelPartCoords",        "Script_SetBattleEntryCoords",     ("int", "int", "int"),                                            "Set model part attachment coordinates (part, x, y)"),
    _op(0xB1, "SelectWorldMapDestination", "Scene_SelectMapById",             ("int",),                                                         "Open world map destination selector"),
    _op(0xB2, "GetEquipSlotState",         "Script_GetFloorNumber",           ("int",),                                                         "Read Menu_GetEquipSlotStateOrIndex into out_val"),
    _op(0xB3, "SelectFloorByRoll",         "Script_SetFloorByEntityRoll",     ("int",),                                                         "Select floor/map entry"),
    _op(0xB4, "EnablePlayerMovement",      "Script_SetMoveFlag",              (),                                                               "Enable player movement in textbox/scene"),
    _op(0xB5, "DisablePlayerMovement",     "Script_ClearMoveFlag",            (),                                                               "Disable player movement in textbox/scene"),
    _op(0xB6, "OffsetPosForward",          "Entity_PolarToPosition2",         ("int", "int", "int"),                                            "Offset actor pos along current yaw by radius -> (out_x, out_z)"),
    _op(0xB7, "SetupModelPartAnim5",       "Script_SetEntityAnim5Args",       ("int", "int", "int", "int", "int"),                              "Configure 5-arg model part animation"),
    _op(0xB8, "TurnTowardPoint",           "Script_TurnTowardPointStep",      ("fixed", "fixed", "int"),                                        "Turn actor yaw toward (x, z) by step per frame; yield until facing"),
    _op(0xB9, "ClearActorFlag1",           "Script_ClearEntityFlag1",         (),                                                               "Clear bit 0 of actor flags"),
    _op(0xBA, "SetActorFlag1",             "Script_SetEntityFlag1",           (),                                                               "Set bit 0 of actor flags"),
    _op(0xBB, "SetBgEntryScrolled",        "Script_SetEntryScrolled",         ("int", "int", "int", "int"),                                     "Configure scrolling background entry"),
    _op(0xBC, "SetBgEntryMirrored",        "Script_SetEntryMirrored",         ("int", "int", "int", "int"),                                     "Configure mirrored background entry"),
    _op(0xBD, "SetRenderTintParams",       "Script_SetRenderFlag8Params",     ("int", "int", "int"),                                            "Set render object tint parameters and flag 0x8"),
    _op(0xBE, "ClearRenderFlag8",          "Script_ClearRenderFlag8",         (),                                                               "Clear flag 0x8 on actor render_object"),
    _op(0xBF, "SetRenderAlphaValue",       "Script_SetRenderFlag10Value",     ("int",),                                                         "Set render_object value9a and flag 0x10"),
    _op(0xC0, "ClearRenderFlag10",         "Script_ClearRenderFlag10",        (),                                                               "Clear flag 0x10 on actor render_object"),
    _op(0xC1, "SetActorFlag400",           "Script_SetEntityFlag400",         (),                                                               "Set actor flag 0x400"),
    _op(0xC2, "ClearActorFlag400",         "Script_ClearEntityFlag400",       (),                                                               "Clear actor flag 0x400"),
    _op(0xC3, "WaitActorFlag8",            "Script_WaitEntityFlags8",         (),                                                               "Yield until actor flag 0x8 is set"),
    _op(0xC4, "RotateActorYaw",            "Script_RotateEntityAngle",        ("int",),                                                         "Add delta to actor rot_y"),
    _op(0xC5, "CopyAttachmentData",        "Entity_CopyAttachmentData",       ("int", "int", "int"),                                            "Copy attachment transform data from actor"),
    _op(0xC6, "PlayAnimOnceAndWait",       "Script_SetEntityActionAndWait",   ("int",),                                                         "Set actor animation mode and yield until one cycle finishes"),
    _op(0xC7, "SetThreadFlag80",           "Script_SetNodeFlag80",            (),                                                               "Set flag 0x80 on current script thread"),
    _op(0xC8, "ClearActorRoamFlag",        "Script_ClearEntityRoamFlag",      (),                                                               "Clear actor flag 0x20000"),
    _op(0xC9, "SetActorRoamFlag",          "Script_SetEntityRoamFlag",        (),                                                               "Set actor flag 0x20000"),
    _op(0xCA, "SetRenderTableIndex",       "Script_SetEntityField1E6",        ("int",),                                                         "Set actor render_object.table_index"),
    _op(0xCB, "GetAngleToPoint",           "Script_GetAngleToPoint",          ("fixed", "fixed", "int"),                                        "Compute yaw angle from actor to (x, z) into out_angle"),
    _op(0xCC, "ResetActorRenderObject",    "Script_ResetEntityRenderObj",     (),                                                               "Clear object animation and parent link on current actor"),
    _op(0xCD, "SnapToSpawnPosition",       "Script_SnapPosToSpawnPos",        (),                                                               "Reset actor pos to initial spawn coordinates"),
    _op(0xCE, "SetActorBattleField",       "Entity_SetFieldByTypeId",         ("int", "int", "int", "int"),                                     "Write battle context field on actor (type_id, sub_id, field_id, val)"),
    _op(0xCF, "SignalSceneExit",           "Script_SignalSceneExit",          (),                                                               "Signal current scene exit"),
    _op(0xD0, "SetTextboxRect",            "Script_DrawSaveOverlay",          ("int", "int", "int", "int"),                                     "Set custom textbox rectangle (x, y, width, height)"),
    _op(0xD1, "OpenStyledTextbox",         "Script_ResetEntityAnimSlot",      ("int",),                                                         "Open styled textbox (style=1) for page_id in custom rect"),
    _op(0xD2, "GetGameTimeCounter",        "Script_GetFloorTableValue",       ("int", "int"),                                                   "Read GAME_TIME_COUNTER(index) into out_val"),
    _op(0xD3, "ConvertFramesToHMS",        "GameTime_ConvertSecondsToHMS",    ("int", "int", "int", "int"),                                     "Convert frame count to (hours, minutes, seconds)"),
    _op(0xD4, "SetParentActor",            "Script_SetEntityParentLink",      ("int", "int"),                                                   "Link current actor parent to (type_id, sub_id)"),
    _op(0xD5, "DetachFromParent",          "Script_ClearEntityParentLink",    (),                                                               "Clear parent link on current actor"),
    _op(0xD6, "DetachAllChildren",         "Entity_ClearParentLink",          (),                                                               "Detach all actors whose parent is current actor"),
    _op(0xD7, "Sin",                       "Script_GetSin",                   ("int", "int"),                                                   "Compute rsin(angle) << 4 into out_val"),
    _op(0xD8, "Cos",                       "Script_GetCos",                   ("int", "int"),                                                   "Compute rcos(angle) << 4 into out_val"),
    _op(0xD9, "Atan2",                     "Script_GetAtan2",                 ("int", "int", "int"),                                            "Compute ratan2(y, x) into out_angle"),
    _op(0xDA, "Sqrt",                      "Script_GetSqrt",                  ("int", "int"),                                                   "Compute integer square root into out_val"),
    _op(0xDB, "CameraTrackActorAngle",     "Render_CameraTrackEntityAngle",   ("int", "int", "int"),                                            "Track actor angle with camera"),
    _op(0xDC, "SetActorFlag1000000",       "Script_SetEntityFlag1000000",     (),                                                               "Set actor flag 0x1000000"),
    _op(0xDD, "PolarOffsetToPos",          "Entity_PolarToPosition",          ("int", "int", "int", "int"),                                     "Offset actor position by (radius, angle) -> (out_x, out_z)"),
    _op(0xDE, "PopCount",                  "Entity_PopCount",                 ("int", "int"),                                                   "Count set bits in arg0 -> arg1"),
    _op(0xDF, "EnableActorGravity",        "Script_EnableEntityGravity",      (),                                                               "Set actor flag 0x2"),
    _op(0xE0, "DisableActorGravity",       "Script_DisableEntityGravity",     (),                                                               "Clear actor flag 0x2"),
    _op(0xE1, "SetMeshFadeLevel",          "Script_SetMeshFadeLevel",         ("int",),                                                         "Set g_RenderMeshTintLevel"),
    _op(0xE2, "TestPointInPolygon",        "Entity_CallAction",               ("fixed", "fixed", "int", "poly_label", "int"),                   "Test if (x, z) is inside polygon(count, @poly) -> out_inside"),
    _op(0xE3, "SetWalkableFloorPolygon",   "Script_SetInventorySlotPointer",  ("poly_label", "int"),                                            "Register walkable floor polygon (@poly, vertex_count)"),
    _op(0xE4, "GetInventoryTotal",         "Script_GetInventoryTotal",        ("int",),                                                         "Read total inventory item count into out_total"),
    _op(0xE5, "GetInventorySlotLimit",     "Script_GetInventorySlotLimit",    ("int",),                                                         "Read Aya inventory slot capacity into out_limit"),
    _op(0xE6, "SetBgEntryScreenPos",       "Script_SetActorScreenPos",        ("int", "int", "int"),                                            "Set 2D screen position of background entry (index, x, y)"),
    _op(0xE7, "OpenMemCardDialog",         "Script_OpenMemCardDialog",        (),                                                               "Open memory card dialog screen and yield until closed"),
    _op(0xE8, "ShowReceivedItemPopup",     "Script_ShowReceivedItem",         ("int",),                                                         "Show received item popup dialog and yield until closed"),
    _op(0xE9, "RunInventoryCommand",       "Script_RunInvCommand",            ("int", "int", "int", "int", "int"),                              "Execute inventory/menu command and yield while active"),
    _op(0xEA, "AkaoCmd",                   "Akao_ProcessCommand",             ("hex", "int", "int", "int", "int", "int"),                       "Dispatch AKAO sound/music/stream command"),
    _op(0xEB, "EnableActorShadow",         "Script_SetEntityShadowFlag",      (),                                                               "Set actor shadow flag 0x20000000"),
    _op(0xEC, "DisableActorShadow",        "Script_ClearEntityShadowFlag",    (),                                                               "Clear actor shadow flag 0x20000000"),
    _op(0xED, "SceneCmd",                  "Script_DispatchScriptCmd",        ("int", "int", "int", "int", "int", "int", "int", "int"),         "Dispatch scene/light/sprite command (2100..3200)"),
    _op(0xEE, "OpenEquipMenu",             "Script_BeginSceneTransition",     ("int",),                                                         "Open equipment menu screen and yield until closed"),
    _op(0xEF, "OpenSaveLoadMenu",          "Script_OpenSaveLoadMenu",         ("int",),                                                         "Open save/load menu screen and yield until closed"),
    _op(0xF0, "GetPlayerInputFlag",        "Script_GetPlayerInputFlag",       ("int",),                                                         "Read player input status flag into out_flag"),
]

OPCODES_BY_ID: dict[int, OpcodeSpec] = {spec.opcode: spec for spec in OPCODE_SPECS}

OPCODES_BY_NAME: dict[str, OpcodeSpec] = {}
for _spec in OPCODE_SPECS:
    OPCODES_BY_NAME[_spec.name] = _spec
    OPCODES_BY_NAME[_spec.c_symbol] = _spec
    OPCODES_BY_NAME[f"op_{_spec.opcode:02X}"] = _spec
    OPCODES_BY_NAME[f"op_{_spec.opcode:02x}"] = _spec
