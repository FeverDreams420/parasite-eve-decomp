# Naming review of the main executable

Names for 869 previously unnamed symbols of `main` (functions `func_*`, globals
`D_*`) were proposed from the code by four readers and then checked by a second
set of reviewers who tried to refute each name against the sources.
`docs/naming_review.csv` lists address, old name, name, review verdict,
proposer confidence and the evidence.

Of the 1053 candidate names that were standalone symbols, 803 held (`ok`), 106
were corrected (`fix`), 73 were rejected and 71 stayed `unsure`. The applied
set is the `ok` plus the corrected names, 869 after removing collisions with
existing names and the overlay-address functions described below.

Left unnamed on purpose:

- symbols that are fields of a larger object (`g_GameState` at 0x800B0CD8,
  `g_GeomScroll` at 0x800BCF88 and its tint block, `g_SaveRuntimeState`,
  `D_800F3368`, the pad packet): they need a struct definition, not a name.
  Where C used them, they are now reached as members of their owner
  (`g_GeomScroll`, `g_ScreenTint`, `g_GameState`, `g_GameTimers`,
  `g_PadPackets`, `g_BattleAtbMarkers`, `g_AkaoVoiceChannelTable`,
  `g_FieldFxCellQuad`, `g_TextboxChoiceCursor`, the draw and display
  environment arrays), except where the record compiles differently; the
  aliases left are commented at their declarations;
- `unsure` rows, which have no reader that supports a role;
- 540 symbols the proposers could not explain.

Existing names found to be wrong were changed: `g_EntityWorkBuffer`
(0x800A77F0) is `g_ScriptVariableBank`, and `g_PlayTimeSeconds` (0x800A76BC,
counting frames) and the combat countdown words are members of
`g_GameTimers`.

The fields of the scroll state at 0x800BCF88 (`g_GeomScroll`, with the tint
block `g_ScreenTint`) are reached as struct members; the former
`g_ScreenTransitionTargetY`/`StartY`/`FadeColor` were the tint target green,
start green and duration. Two one-field record views remain where retail
addresses the word itself: `g_ViewMatrixSlot` (0x800BCFA4, the view matrix
pointer, formerly `g_GeomVramPacketDst`) and `g_ProjectionSlot` (0x800BCFA8).

Main's labels for overlay entry points (`func_801216C4`, `func_80121A00`,
`func_80121C04`, `func_80122040`, `func_801223A8`, `func_801235DC`,
`func_801909B4`, `func_8019234C`) keep their address names: they name a
window address, and the same address is a different function in other
overlays. The overlay that defines the function there names it
(`MenuCardFirst_InitVideoBuffers`, `MenuCardFirst_OpenVideo`,
`MenuCardFirst_StepVideo`, `MenuCard_RunListScreen`,
`FxCommon_RunFrameLoop`), so no name is defined by both main and an
overlay (docs/OVERLAY_SYMBOL_COLLISIONS.md). Main's labels at the window
starts that the load-base pointers hold are `g_MdecOverlayArea` and
`g_CdLoadArea` (configs/USA/sym.main_only.txt).

A name for a symbol that lives in an asm data blob needs a line in
`configs/USA/sym.main.txt`; renaming only the C use leaves the label as
`D_XXXXXXXX`, and the linker then places the C definition elsewhere.

## Second pass, 0x80010000-0x800A0000

`docs/naming_review_main2.csv` lists the second pass over the auto-named
symbols left in this range (address, old name, new name or owner member,
evidence). A name was applied only with a reader or writer that shows the
role; members replaced labels where the member access compiles to the
retail code, and one-field views kept their own symbol where it does not
(`g_BattleActionAnimMode1/3`, `g_BattleStatusSymbolSpriteU/V`,
`g_BattleRemainingActionClut`, `g_GpuHRangeEndTable`, `g_SkinAnchorY`,
`g_FloorBlockedEdgeZHalves`, `g_RcossinTblCos`). Three existing names were
wrong and were replaced: the pad edge words `g_FieldPadPressed`,
`g_FieldPadReleased`, `g_FieldPadLastFrame` (formerly
`g_BattleControlFlags`, `g_SceneFlagBits`, `g_FieldRenderFlags`) and the
script cursor `g_ScriptCursor` / `g_ScriptCursorAddress` (formerly
`D_8009CE00` / `g_SceneDataTable0`).

Left unnamed on purpose:

- labels splat derives from retail code for interior fields that the C
  already reaches as members (sprite packets, the PE.IMG sector table, the
  font selection state, LIBGPU/LIBCD/LIBDS state blocks, the biased bases of
  `g_SineQuarterTable`);
- second spellings of already-named words that the C keeps for a different
  type (`undefined_syms_manual.txt` aliases);
- zero padding between objects, PsyQ object stamps and signature records;
- PsyQ-internal code labels (`func_8007E344`, `func_8007E3C8`), which the
  rename tool keeps;
- words that are only written or whose readers do not show a role.

## Second pass, 0x800A0000 to 0x80120000

`docs/naming_review_main3.csv` lists the 679 address names of this range
that a second pass resolved (address, old name, name or member path,
evidence; 12 of them are now written as members but keep their splat
label, see below). Of the 792 `func_`/`D_` symbols the range had, 652 are
gone:

- 61 got names from a concrete reader or writer (9 functions, among them
  the three routines in the engine data blob behind script opcodes
  2400/2402, and 52 data symbols). Views that must stay separate words for
  retail's code are named after the member they view and say so at their
  declaration (`g_RenderLight0DirY`, `g_BattleMissSprite`,
  `g_AkaoNestedKeyOffMask`, `g_PadIntQueueHandlers`, ...).
- The rest were interior bytes of larger objects. Their C uses are now
  members (`g_BattleAtbGauges[g_ActiveDrawSlot].r0`,
  `g_CdQueuedCmdSlots[1]`, `g_RenderLightingState.matrix.m[2][1]`), and the
  owner's size in `configs/USA/sym.main.txt` (`// size:0x..`, the size of
  its C type) stops splat from labelling its interior.

140 remain, by reason:

- member labels kept because a named one-field view inside the same
  record splits it (the members are reached by name in C): the
  `g_GameTimers`, `g_GameState`, `g_GeomScroll`, `g_RenderLightingState`,
  `g_SaveRuntimeState`/`g_AyaSaveState`, `g_FieldFxSpriteParams` and
  inventory array labels;
- second spellings of a named object kept for a type, qualifier or overlay
  declaration (`D_800A76A0`, `D_800A76A4`, `D_800B0CE2`, `D_800B0DBA`,
  `D_800B0DBC`, `D_800B0DCD`, `D_800B0E38`, `D_800B0E3C`, `D_800B0E4C`,
  `D_800B0E50`, `D_800B0E54`, `D_800B0E58`, `D_800B0E64`, `D_800B0E6C`,
  `D_800B89F8`, `D_800BCF88`);
- no reader: written-only words, unreferenced tails after a sized object
  and the pad words after the 0x2C-byte effect states;
- role not established: `func_800C6C18`, `func_800DFB20`, `D_800B0E2C`,
  `D_800B0E48`, `D_800B6914`, `D_800E21D0`, and the mdec_diag window bytes
  `D_8010BD00` (the window start itself is `g_MdecOverlayArea`);
- used only in `fieldfx/geometry.c`, `render/object.c` or
  `field/actor_collision.c`, which this pass did not touch (`D_800A636C`,
  `D_800B0E98`, `D_800B1640`, `D_800B1644`, `D_800C2258`, `D_800E21A8`,
  `D_800F32D8`, `D_800F336C`, `D_800F3374`, `D_800F3418`), and
  `D_800BCFA4`, which keeps its name;
- `D_800B0D88`, behind the asm-label views of `battle/encounter.c`.

## Third pass, main names left in src/ and include/

`docs/naming_review_main4.csv` lists the 46 main address names this pass
removed from `src/` and `include/` (address, old name, name or member path,
evidence). 26 aliases of `linkers/USA/undefined_syms_manual.txt` went with
them. Most were second spellings that the earlier passes kept for a type:
in the `MASPSX_FORCE_G0` units a scalar and an array spelling compile the
same, so the owner's declaration could take the other spelling's shape (one
word instead of an incomplete array, the unsigned type when the owner is
only stored to) and every unit now uses the name. Where one function still
needs its own view, the view is named after its owner
(`g_DrawEnabledByte`, `g_SeqElapsedRecord`, `g_InvSavedSelectionIndex0`) or
after the object it really is (`g_DrawBlendColorPair`, `g_BattleScanPanel`,
`g_GameClockFlags`, `g_MovieWide`).

44 remain, by reason:

- used in `fieldfx/geometry.c`, `render/object.c` or
  `field/actor_collision.c`, which this pass did not touch (`D_8009D254`,
  `D_8009D2E8`, `D_800A636C`, `D_800B0E38`, `D_800B0E98`, `D_800B1640`,
  `D_800B1644`, `D_800BCFA4`, `D_800C2258`, `D_800E21A8`, `D_800F32D8`,
  `D_800F336C`, `D_800F3374`, `D_800F3418`);
- PsyQ library words: qualifier or small-data views whose owner spelling
  changes the code (`D_80095674`, `D_8009B46C`, `D_8009B784`, `D_8009B788`),
  the LIBPAD hook pair and its enable word that PE never writes
  (`D_8009B750`, `D_8009B754`, `D_8009B760`) and the LIBCD word
  `D_800B6914`;
- no reader or no writer: `D_8009CDAC` (0x123, never written),
  `D_8009CDC8` (zero, never written), `D_8009CE78`, `D_8009CEB4`..
  `D_8009CEC0`, `D_8009CFA4`, `D_8009D084`, `D_8009D088`, `D_8009D08C`,
  `D_800A1FE8`, `D_800A1898`, `D_800A5D50`, `D_800B0E48`;
- role not established: `D_8009CFBC` (set when Inv_CheckItemEquippable
  returns 5);
- views whose owner spelling changes the code: `D_8009D2C8` (the
  AkaoTickBank view of `g_AkaoCurTrack`, about 80 uses), `D_8009D2F0` (one
  byte-pointer use in the script dispatch), `D_800BCF88` (volatile
  read-modify-write in the grab effects) and the asm-label views of
  `D_800B0D88`;
- `D_8010BD00` and `D_80120D08`, buffers in the mdec_diag and menu_memcard
  overlay windows: a main name would also label those overlays' own data
  there (`make overlay-collision-check`).
