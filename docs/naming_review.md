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
  `D_800F3368`, the pad packet): they need a struct definition, not a name;
- `unsure` rows, which have no reader that supports a role;
- 540 symbols the proposers could not explain.

Existing names found to be wrong, not yet changed:
`g_ScreenTransitionTargetY`/`StartY`/`FadeColor` (0x800BCFEA/F2/F6) are the tint
target green, start green and duration; `g_GeomVramPacketDst` (0x800BCFA4) is the
view-matrix pointer; `g_EntityWorkBuffer` (0x800A77F0) is the script variable
bank; `g_PlayTimeSeconds` (0x800A76BC) counts frames.

Two functions at overlay addresses (`func_801909B4`, `func_8019234C`) keep their
address names: the same address is a different function in other overlays.

A name for a symbol that lives in an asm data blob needs a line in
`configs/USA/sym.main.txt`; renaming only the C use leaves the label as
`D_XXXXXXXX`, and the linker then places the C definition elsewhere.
