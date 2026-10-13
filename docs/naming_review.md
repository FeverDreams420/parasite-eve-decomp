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

Two functions at overlay addresses (`func_801909B4`, `func_8019234C`) keep their
address names: the same address is a different function in other overlays.

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
