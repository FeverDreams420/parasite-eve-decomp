# Code organization contract

This repository is a long-lived reconstruction of the original program, not a
collection of independent decompilation puzzles. A byte match is necessary,
but it is not sufficient evidence that source boundaries, declarations, names,
or types are correct.

## Evidence order

Use evidence in this order when placing or combining source:

1. executable addresses, section layout, relocations, and manifest order;
2. known object or archive boundaries and matches to Psy-Q or AKAO libraries;
3. shared data, call relationships, and static state;
4. responsibility of the code and its callers;
5. existing filenames, which are hints rather than evidence.

Do not infer an original object boundary merely because splat emitted one
function or because one function is convenient to permute in isolation. Do not
merge non-contiguous code or cross a strong responsibility boundary merely to
reduce the file count.

## Directory taxonomy

- `src/main/<subsystem>/` contains code resident in `SLUS_006.62`.
- `src/overlays/<binary>/` contains code whose link address and lifetime belong
  to that overlay. Code shared by several overlays stays duplicated until
  binary evidence identifies a linked library or a reproducible generation
  rule.
- `src/overlays/room_lib/` holds such a linked library once. The room
  library (`RoomLib_ActorClasses.c`) is the same run of 53 functions, in the
  same order and with the same jump tables, in 125 room and scene overlays.
  Each overlay's manifest names the unit as a `../room_lib/RoomLib_ActorClasses`
  subsegment, and `make overlay-build` compiles it into that overlay's own
  build directory, so every overlay still links its own copy at its own
  address. Eight rooms link a sixth class right after it
  (`RoomLib_FloorWalkerClass.c`); whether that class was part of the same
  object cannot be told from the binary, so it is a unit of its own.
  Smaller effect objects shared by a few rooms live there the same way
  (`RoomEffect_DroppedFlare.c`, `RoomLib_TwelveElementEffect.c`, ...): each
  is a run of functions in the same order in every instance, with a rodata
  block (seed initialisers, jump tables) that is contiguous and in the same
  order too. Data the code reads from the room (packet templates, colour
  tables) stays in each room's manifest under one name per role, given in
  the room's symbol file. The methods of a room module's class (the run in
  front of each effect module: `RoomLib_PlantScript`, `RoomLib_Spawn6`,
  `RoomLib_RegisterDrawList`, `RoomLib_RegisterPairedTables`,
  `RoomLib_CloseTarget`) are one unit per function, because rooms replace
  single methods with their own; see `include/pe1/room_module.h`.
- `include/pe1/` owns game ABI, shared data structures, and declarations.
- `include/pe1/akao/` owns the AKAO command, queue, track, and SPU interfaces.
- hardware and SDK declarations belong in their subsystem header rather than
  being repeated in C files.

The subsystem list, the target directory for every current directory and the
naming rules are in [CONVENTIONS.md](CONVENTIONS.md).

Names such as `func_800...`, `misc`, `task7`, and `spu4` explicitly mean
"identity or module boundary not established". They are migration state, not a
stable naming convention. `make organization-check` prevents the address-name
and placeholder-name counts from increasing independently in main and overlays.

## Translation-unit rules

A merge or split is accepted only when all of the following hold:

- manifest order and addresses remain unchanged;
- the proposed unit is contiguous, unless independent object evidence proves
  otherwise;
- shared declarations and private state have a clear owner;
- `make check` remains byte-identical after a clean rebuild;
- configured overlays touched by the change pass `make overlay-check`;
- the evidence and any remaining uncertainty are recorded in the commit or in
  maintained documentation.

Large cohesive units are acceptable. Function count and aesthetics alone are
not reasons to invent source boundaries.

`src/` has a strict one-to-one relationship with committed manifest entries.
The one exception is a shared library unit under `src/overlays/room_lib/`:
many overlay manifests may list it, each at most once.
The check uses files present on disk as well as Git's index: an untracked C file
cannot enter a local build, and a manifest promotion cannot pass unless its
source will exist in a fresh clone.
Non-matching experiments are not tracked. A C file enters `src/` only in the
same change that configures its range as `c` and passes the binary check.

## Declaration ownership

New file-scope `extern` declarations in `.c` files are forbidden by the debt
gate. Put public or cross-unit declarations in the narrowest appropriate
header. Keep declarations private only when every use and definition are in a
single translation unit.

Existing declarations are migration debt tracked separately for main and
overlays. Moving a declaration must preserve its exact signedness, width,
qualifiers, and function prototype; do not "improve" an uncertain ABI while
centralizing it.

A symbol that units read with different widths or access patterns still gets
one declaration: a union with one member per load width (`RoomFloorY` in
`pe1/room_floor.h` has `y` for `lh` and `raw` for `lhu`), declared as an
incomplete array when a `-G8` unit must keep the absolute address.

### Per-file flag conflicts

A merge that one neighbour's cc1 option seems to forbid is usually a C-shape
question, not a real conflict:

- G0341: `Boot_InitSubsystems` only needed `-fno-schedule-insns2` for its
  prologue store order; starting its flush loop after `Task_InitGpuHwRegs` as
  a `for` loop gives the retail order under the default schedule.
- G0835: the background scroll step only needed strength reduction for the
  loop pointer copy it creates; walking the entry pointer by hand inside a
  guarded `do`/`while`, with the camera pointer taken before it, matches under
  `-fno-strength-reduce`.

### AKAO driver (src/main/akao)

The AKAO units have no offset macros, byte arithmetic or file-scope
`extern`s: driver-wide state is declared once in `pe1/akao/globals.h`, the
voice-mask SPU registers in `pe1/akao/spu_registers.h`, and the sequencer
and transfer units carry no pins, barriers or gotos. C shapes that only
exist for the bytes, and why:

- `Spu_InitVoiceState` writes the request, volume and slide fields of the
  two banks behind `g_AkaoCurTrack` through word views
  (`*(int *)&bank[0].volume`). GCC 2.7 assumes a store to a structure field
  through a pointer cannot alias a fixed scalar, so with plain field stores
  the scheduler hoists the `g_AkaoVoiceUpdateFlags` read-modify-write and the
  `D_800BCD54`..`D_800BCD68` stores across them. Retail keeps the source
  order, which only non-structure stores give. Declaring the scalars as a
  record instead fixes the stores but not the read-modify-write, which a
  record view turns into a register-held address. `Akao_UpdateVoiceMask`
  writes `key_off_request_mask` the same way, and `SPU_StepReverbLoad`
  reads the bank cursor through the one-field `AkaoBankCursor` record for the
  opposite reason: its reload after a bank store must stay.
- `Akao_SetBgmVolumeFade` reads `g_AkaoBgmHandle[0]` twice by index. A
  fixed-symbol load is not reused across the stack store of the fifth
  argument, which gives retail's reload without a volatile slot pointer.
- `D_8009D268`, the message-queue lock the timer interrupt tests, and
  `D_8009D2E0`, the busy flag `Spu_PollOrWaitSeqBusy` spins on, are
  `volatile`: `Spu_InitVoiceState` clears the lock and reads it back before
  copying it to the queue words, and only a volatile object keeps that
  reload.
- `Spu_UploadSampleBlock` walks the header words with `*header++` on its
  parameter. A separate cursor lets CSE fold every word into an offset from
  the untouched parameter, while retail advances the pointer.
- `Spu_RestoreVoiceFromStereoSlot` reads the stereo slot's right volume
  into a halfword before the `update_flags` read-modify-write and stores it
  after. Copying the field directly reorders the loads and stores around the
  flag update and loads the index from `$s1` instead of the argument
  register (lev 12). The 0x1FF93 that retail keeps in `$a3` is the mask
  register, not a fourth argument.
- `Spu_TickVoiceEnvelopes` computes the pitch LFO depth inside both
  branches of the speed test, into a `u16`. Retail shifts the depth before
  loading `pitch_lfo_phase`: a depth assigned once after the branches is a
  single-set pseudo that sched1 places late, and a `u32` depth assigned in
  both branches is allocated to `$v1`.
- `Akao_LookupPitchPeriod` assigns the octave copy again inside
  `if (detune)`; without it CSE substitutes the octave for the copy and the
  two values share a register. `SeqOp_LoadInstrumentFromIndex` and
  `Akao_LookupPitchPeriod` keep integer table addresses, because retail
  reuses that register for the release mode and the period (lev 15-24 with
  `AkaoInstrument` pointers).
- The `Spu_*AllVoice*`/`Spu_*VoiceMasked` loops of the voice-bank unit walk
  a cursor biased to one field of the channel record (`voice - 0xF4`), as
  retail does; a plain `AkaoTrack *` walk addresses the fields from the
  record start (lev 6-12 per function).

Crutches still in place (lev is the only metric; each was retried with the
permuter and by hand):

- `Akao_StepNoteSequencer` and `Spu_UploadStreamBlockA` (4 barriers each):
  retail allocates the header cursor before the upload size (s0, s1).
  Global-alloc priority is refs * floor_log2(refs) / live length; without the
  barriers the size has 22 refs over 84 insns against the cursor's 18 over
  83, and the barriers add the cursor references that flip the order. A
  separate copy-loop counter removes the cursor barriers but lands the counter
  in a2 (lev 3); reusing the size as the counter flips priority back (lev 31).
  Clamping the continuation upload into its own block-scope size takes the
  size's continuation references out (`Spu_UploadStreamBlockA` then differs
  only by the s0/s1 swap: cursor 18 refs over 42 insns, size 16 over 31,
  priority 1.71 against 2.06). One size reference fewer flips the order, but
  every plain C shape that drops one (the clamp as `if`/`else`, the counter
  in its own variable, the size advance inlined) changes the clamp branch or
  the counter register; permuter runs found only invalid or duplicated-code
  shapes.
- `Akao_ProcessVoiceQueue` (pin on `bit`, 1 barrier): `secondary_new` must be
  allocated before `excluded`; its extra references come only from a copy that
  CSE deletes unless `bit` is a hard register (lev 24, lev 11 with a
  `do { } while (0)` wrapper, which is no better a crutch).
- `Akao_StepSequencerVoice` (pin on `all_voices`, 1 barrier): retail builds
  0xFFFFFF once in a0 and stores it from a0; cse1 rewrites the argument copy
  to the constant and builds it twice (lev 3-15). Passing the stored field
  back (`func_80089960(pend->allocated_voice_mask)`) reloads it (lev 6), and
  the assignment as the argument builds it twice (lev 4).
- `Seq_TrackPitchSetup` (2 pins): retail computes the pitch delta in place in
  a2 (sll, subu, div, mflo), which gives it more weight than `track`; without
  the pins the subtraction goes through v0 and the registers swap (lev 20).
  Dividing in place (`delta /= duration`, the `Seq_SlideTrackPitch` shape)
  gives lev 29.
- `Akao_UpdateVoiceMask` (3 pins, 4 barriers): without the v0 pins loop.c
  also hoists the constant 2 (its threshold of 52 equals the loop's 52
  insns); with literals the hoist set matches but the bank copy and
  `voice_mask` scheduling move. Best crutch-free version is lev 36.
- `Seq_StartNestedStreams` (1 goto): retail tests `remaining` again after
  the exhausted handler, which happens only when the handler starts at a
  label CSE enters without knowing `remaining`; every goto-free shape lets
  CSE drop that test (lev 69).

### Tables, assets, process manager and sys (src/main/{table,asset,pm,sys})

These units have no file-scope `extern`s; their data lives in
`pe1/resident_tables.h`, `pe1/pm.h`, `pe1/asset_table.h`, `pe1/sys.h` and
the other owner headers. C shapes that only exist for the bytes:

- The resident data pack at 0x800A8028 (`g_ResidentDataPack`) is read
  directly by most lookups, which lets CSE address the pack base relative to
  the slot word as retail does, and through a local `ResidentDataPack *` by
  the text-table lookups, which keeps the slot load and the pack base
  independent as retail does.
- The process-manager slot helpers take the secondary index into its own
  `int idx = slot - 11`; indexing with `slot - 11` directly converts it to
  the unsigned size type first and loop strength reduction no longer
  produces the retail per-bank offset counters. `Pm_StopUpperHalf` passes
  `i + 11` so the slot number is its own induction variable.
- The keyed asset lookups store the payload in `found` before returning it
  (retail allocates `found` above the loop index), and each keeps an unused
  `char unused[1]` local because retail reserves an 8-byte frame it never
  touches.
- `Asset_SearchByKeyType` keeps its masks in locals so loop.c hoists all
  four into saved registers, as retail does; literals in the return
  expression stay in the exit block.

Crutches still in place (lev is the only metric):

- `Scene_LoadRoomAssets` (1 goto): the PE.IMG retry jumps back over the
  sector read. Any loop form (do/while, for with break) is seen by loop.c,
  which hoists the table address and the -1 constant out of the retry path;
  retail rebuilds them on every retry (lev 50).
- `Pm_Exec` (1 memory barrier): retail loads `entry->state` again after the
  active-state test; without the barrier reorg deletes that load as
  redundant while stealing the branch targets' delay slots (lev 26 across
  plain, switch, inline-predicate and wrapped forms).
- `Asset_LoadTimTextures` (2 gotos): the state machine restarts its
  `switch` after states 0 and 0x35; a `for (;;)` loop around the switch lets
  loop.c move code out of the cases (lev 81). The two volatile bank-pointer
  views are gone: the bank pointers are stored and reloaded through
  `g_PmTables` (`pe1/pm.h`), a record view of the three adjacent dispatch
  words at 0x800942E0. A record member at a fixed address may alias the
  slot stores, so loop.c keeps the reload in the loop and takes the address
  into a register, which is retail's code exactly.
- `GameState_Reset` (3 pins, 3 launders, byte offsets off `base`): the
  eight ordering barriers were redundant and are gone. Each remaining
  launder makes a -1 or the loop counter look like a fresh value after a
  store so it is not merged with an earlier constant (lev 11, 2 and 14 when
  removed one at a time); the `$3`/`$5` pins keep the fill cursor and loop
  counter in retail's registers (lev 3, 76, 77). A field-by-field rewrite
  through a `Pe1GameState *` gives retail's absolute-then-relative
  addressing and loops, but sched1 launches the shared -1 byte constant
  just before its first store while retail loads it early into v1, which
  shifts every later register (best lev 92).
- `Boot_InitMemoryLayout` (2 pins, 4 pointer-integer casts, volatile
  `D_800B0E24`..`D_800B0E68` views): without the pins the buffer cursors and
  the 0x8000/0x48000 constants trade registers in every declaration order
  (lev 44), and without volatile the scheduler interleaves the game-state
  stores with the render buffer stores differently.

### Render (src/main/render)

The render units have no file-scope `extern`s, raw offset dereferences or
alias declarations: camera and scroll state is declared in
`pe1/geom_state.h`, `pe1/render_camera.h` and `pe1/render_tint.h`, the
object and wireframe data in `pe1/render_object.h` and
`pe1/render_animation_frame.h`, and the GTE register transfers are PSY-Q
style macros in `pe1/gte.h`. C shapes that only exist for the bytes:

- Every 8 unused bytes in a render frame came from a top-tested loop with a
  variable bound, or a guarded `i = 0; if (i < count) do ... while` loop:
  combine folds the first compare into the branch and the orphaned compare
  pseudo keeps its stack slot. `Render_DrawTextDigit`, `Font_FindCodeSlot`
  and `Obj_ResetAllEntries` lost their stack pads this way.
- Offsets from the geometry header (`g_GeomState + g_GeomState->field`) go
  through the `GeomStateAddress` word view (`GEOM_STATE_OFFSET`): retail
  loads the header pointer twice and adds the offset as an integer.
- `Render_StepFade` and `Render_ApplyScreenTint` advance the primitive
  pointer inside the loop body; in the `for` header the step is scheduled
  above the colour stores (lev 2).
- The wireframe depth and screen-XY buffers are one record
  (`g_WireScratch`), so the line packet's XY stores and the depth loads are
  both structure accesses and keep retail's order without volatile.

Crutches still in place (lev is the only metric):

- `Render_SetViewport` (6 pins, 5 barriers): removing any single one costs
  lev 2-46; the best crutch-free rewrite is lev 65.
- `Render_UpdateScrollPosition` and `Render_PrepareFrame` (volatile register
  screen-centre input, view-matrix pin and slot barrier each): retail has 8
  frame bytes nothing touches. The volatile register is the only form found
  that keeps them; without it the frame shrinks (lev 6 each), a plain pin
  keeps the registers but not the frame (lev 2), and a 12-byte projection
  buffer keeps the frame but swaps the offset registers (lev 4).
- `Render_DrawSpriteEntry` (21 pins, 2 barriers, 6 pointer casts, 1 byte
  sum): each pin alone is worth lev 2-69, the natural rewrite is lev 120+.
- `Render_IncrementCounter` (2 pins): the counter word sits in .text right
  after the routine and it works in a0/a1, so the original is hand-written
  assembler.
- Font selection: `Render_StepFontLoad`, `Render_FindFontGlyphSlot` and
  `Menu_FindSelectedEquipSlotItem` store the selected slot and reload it
  through the other symbol for that byte; without the memory barrier the
  reload is scheduled first (lev 25, 10, 11). `Render_StepFontLoad` keeps an
  identity constraint on the digit (lev 19) and `Render_SetFontGlyphByCode`
  one on the stored slot (lev 14). `Render_LoadFontGlyphAlt` reads the
  glyph through `groups.codes[slots.indices[...]]` but reloads the stored
  slot as `*(u8 *)&D_80091A1F`: a plain read is CSE'd with the store
  (lev 35) and `g_FontSelectionState.selected` is hoisted above the store
  because it is another symbol (lev 36).
- `Render_DrawSpriteEntry` keeps `ot = ot_index + (u32)ordering` and the
  byte offset of the position array: `ordering + ot_index` swaps the
  `addu` operands (lev 2), and the position array lies `pos_ptr` bytes from
  the entry, an offset read from the data.
- PE.IMG and font retry loops (`Render_LoadFontGlyphAlt` 1 goto,
  `Render_FontGlyphLoad` 2, `Render_InitDisplayLists` 2): any loop form lets
  loop.c hoist the table address and constants out of the retry path, which
  retail rebuilds on every retry.
- `OpenPeImage` (2 barriers): retail leaves the delay slot of the second and
  third `found == 0` tests empty; without the barriers reorg fills it with
  the -1 constant (lev 5-8).
- `Render_DrawWireframeModel` (1 goto): the floor-edge search shares the far
  vertex assignment between two arms; written twice, the extra reference
  swaps the screen-XY registers (lev 14). Its file comment lists the
  uncounted register-allocation shapes it still needs.
- `Render_DrawActorShadow` (1 volatile store): retail copies the part matrix
  word by word and keeps the translation store that the next store
  overwrites; a structure copy keeps the store but uses GCC's four-register
  block move (lev 22).
- `Render_DrawObjectVariant` was left as is (out of scope for this pass).

### Menu (src/main/menu)

The menu units have no file-scope `extern`s and no local extern lists except
the two alias views listed below. Menu-wide state lives in
`pe1/menu_state.h`, the bonus point and level-up panels in
`pe1/menu_bonus_stats.h`, dialogs and save slots in `pe1/menu_dialog.h`,
equipment screens in `pe1/menu_equipment.h`, list adapters and row drawers
in `pe1/menu_item_rows.h`, and the event queue in `pe1/menu_queue.h`. C
shapes that only exist for the bytes:

- MASPSX fills a load delay before a `$gp` store only when it knows the
  stored symbol is small data, which it learns from COMMON definitions in
  the unit (`--use-comm-section`). Units that store a loaded value into an
  extern word therefore define it tentatively (`int g_MenuEquipSwapItem;`,
  `g_MenuInventoryViewMode`, `g_MenuPendingTotalExp`,
  `g_BonusPointDisplayValue`, `g_BonusPointSpendCurrentValue`) instead of an
  explicit NOP. `Menu_EquipScreenFlow.c` uses `MASPSX_FORCE_G0` with COMMON
  definitions for every `$gp` word so `&D_8009CF90` stays an absolute `la`.
- Retail inlines `Queue_Enqueue`, `Queue_DequeueByMask` and
  `MenuWidget_AllocNode` into their callers; the units declare them `inline`
  (non-static, so the out-of-line copies remain) and call them.
- A node variable assigned once places its copy of the call result late
  (sched1 launch priority), one assigned several times keeps it right after
  the call: `Menu_OpenRenameScreen` reuses one parent variable, the equip
  confirm dialog takes its own `dialog` variable, `Menu_StepSkillScreen`
  stores the second lookup back into `panel`.
- Two-line and item dialogs size their panel with `MAX(100, MAX(w0, w1))`
  of `Draw_MeasureTextWidth` calls: the macro's repeated calls and the
  cross-jumped tail are what retail does.
- Small decision trees are `switch` statements with an empty `case 1`
  (`Sfx_ContextPick`) or separate identical cases (`Menu_OpenInventoryOrSwapView`).
- `Menu_AlignEquipPanels` computes the x shift from one conditional
  expression; the separate `if` chain gives the loads other registers.
- The cursor stack is walked as `int` word pairs, except in
  `MenuWidget_DrawListRow`/`RowPopCursor`, where the `DrawTextCursorPair`
  structure view lets the scheduler move the global loads above the stores.

Crutches still in place (lev is the only metric):

- `Menu_StepColorSelect` (2 gotos): both arms share one clamp shift and the
  blend tail. A `do`/`while (0)` form is lev 23, duplicated tails lev 35: cse
  leaves the second arm's clamp with a copy of the channel shift (s3), so the
  tails differ and are not cross-jumped.
- `Menu_StepEquipConfirm` (4 pins, 1 barrier, 7 gotos): retail places the
  confirm tail right after the first arm and the reserved/dialog arms jump
  back into it, an order only the label layout gives (if/else chain lev 71);
  each remaining pin alone costs lev 18-43.
- `Spend_BonusPoints` (1 pin, 4 barriers, 7 gotos): the error tail sits
  between the 0x1000 and 0x10000 blocks; with return statements the first
  copy is kept (lev 64), and without the barriers the two identical
  bonus-decrement cases are cross-jumped into one (lev 52).
- `Menu_StepItemSelectScreen` (3 pins, 5 barriers, 2 gotos): retail returns
  through the cell-index variable in s0 (`li s0,1` then `move v0,s0`); plain
  returns fold to `li v0,1` (lev 44). Case 8 needs its three argument
  registers loaded before the `D_8009CF30` store (lev 2 without the pins).
- `Menu_InventoryInputHandler` (1 pin, 3 barriers): same s3 return value;
  early returns lev 19, a single-exit chain lev 45.
- `Menu_InventoryPageNavHandler` (3 pins, 1 barrier): the conditional shift
  that matches `Menu_AlignEquipPanels` gives the shift a higher global-alloc
  priority than the target node here (s1/s2 swap, lev 12).
- `Menu_DrawItemDetailPanel` (2 pins): retail computes dx right after its
  two loads; unpinned, dx is scheduled after the tail-count chain (lev 5).
- `Menu_DrawStatsList` (2 pins): with either pin gone loop.c
  strength-reduces the multiplier index into a sixth saved pointer (lev 20-25).
- `Menu_StepItemDetailPanel2` (1 pin): retail reloads the label buffer
  address before each of three calls; unpinned, CSE keeps it in a saved
  register and label/option node swap (lev 18).
- `Menu_StepSaveLoadScreen` (1 pin): the transfer callback and
  `Menu_InventoryInputHandler` addresses both live in saved registers and
  swap s0/s1 unpinned (lev 18).
- `Menu_StepSkillList` (1 pin): retail builds the 1 of `1 << type` in v1;
  every plain form folds it into a0 (lev 2).
- `Menu_StepSaveSelect` (1 pin): the dialog mode lives in a1 and is passed on
  to `Save_SetTitleStyleFlag`; unpinned, slot and mode swap a0/a1 (lev 9).
- `Menu_OpenInventoryScreen` (1 pin): retail loads 9 into a0 once before the
  branch for `MenuWidget_InitPool(9)` and `Inv_SetActiveList(9, 0)`; the
  propagated constant leaves a second `li a0,9` (lev 1).
- `Sfx_DrawActiveListSlot` (1 pin): retail copies the item ID into a1 before
  the range tests (lev 12 without).
- `Draw_InitBuffers` (1 pin, 1 barrier): retail stores the screen
  rectangles relative to s0 (the first frame's packet arena field); constant
  addresses fold to absolute stores.
- `Draw_SetPrimCallback` (1 memory barrier, 1 volatile read): retail
  re-reads y_limit and the other fields after storing them.
- `Aya_SetTotalExp` (1 barrier): retail keeps the save base in s2 for the
  total and the cleared pool; without the launder the constant base folds
  into absolute loads (lev 6). The bonus pool is read and written through
  the `g_AyaBonusPoolRecord` record view (the unit is forced-G0).
- `Menu_MainUpdate` (3 pointer-integer casts, 3 field macros, local alias
  header): the m2c variables that carry the battle entity pointers also
  carry colours, and splitting them reallocates registers (lev 28-248); the
  halfword position reads must not be structure members (the scheduler
  would hoist the `D_8009D28C` store). The draw slot reloads come from the
  `g_ActiveDrawSlotRecord` record view and the BGM handle reload from an
  array element read (see the recipes below).
- `Menu_StepItemGrid2` (two asm-labelled views of `D_800A19C0` in
  `pe1/Menu_MemCardProgress_externs.h`): retail rebuilds the text buffer
  address for each `Draw_MeasureTextWidth` call; with one symbol CSE keeps
  it in a saved register.
- `Window_SetBoundsByMode` (volatile window X and bound pointer): plain or
  multi-set forms keep the order but swap v0/v1 (lev 5).
- `Menu_DrawTextboxEntries` (7 pins, 6 barriers, 6 gotos, stack reserve, 2
  byte offsets), `Render_SetupColorTable` (5 pins, 3 barriers) and
  `Render_SetupFogLayer` (2 pins, 1 barrier, 3 byte offsets): the glyph
  spacing offset must stay in s7 across the packet calls (lev 127 with
  structure indexing), a plain digit loop is lev 95, and the fog packets
  add the offset to integer copies of the bases (typed pointers put the base
  first, lev 3).

### Field effect engine (src/main/engine) remaining crutches

Swept 2026-10-11 and 2026-10-12. Recipes that removed crutches without
changing bytes:

- preset initialisers written as plain per-record constant stores (record
  order, fields scale, rotation, cell, clut, depth, rgb); `actor->pos_x >> 16`
  instead of `Pe1Fixed_Integer()` where retail has `lh`; guarded do-while plus
  padding or `asm(&frame)` replaced by `for` loops (8-byte loop frame slot);
  `void` return where retail fills the epilogue delay slot; deleting the
  `-fno-schedule-insns` markers of CollisionQueries_2 and BurstEffect_2.
- GTE macro operands are the objects themselves, not pinned pointers:
  `gte_ldrotmatrix(D_800BCFA4.value)`, `gte_ldv3(D_800F3310, &D_800F3318,
  &D_800F3320)`, `gte_ldv0(&D_800F3328)`, `gte_stsxy3(&p->x0, &p->x1, &p->x2)`,
  `gte_stszotz(&depth)`. A constant or memory operand of an `"r"` input is
  loaded by reload, and reload takes its spill registers round robin
  (`last_spill_reg`); the number of spill registers is the largest number one
  insn needs at once, so the three-operand `gte_ldv3` is what gives the
  t1, t2, t3 rotation retail has in the sprite renderers. Separate
  `gte_lwc2_*` statements or one-operand loads leave a single spill register.
- The composed glow layers are two (`FieldEng_GlowLayers.inc`) PSY-Q
  `gte_CompMatrix(&matrix, &rotation, &matrix)` calls; the floor-triangle
  tests are the PSY-Q outer product (`gte_ldopv1_psyq`, `gte_ldopv2`, OP,
  `gte_stlvnl`) with the first operand read through `D_800E2844` and one
  `FieldTriScratch *` local per later product.
- A byte field incremented as `age++` is a QImode subreg set, which sched1
  does not give launch priority, so the increment is hoisted to the top of the
  block; `age = p->age; p->age = age + 1;` through an `int` makes it a
  single-set pseudo and sched1 places it where retail does (the four glow
  particle updaters). The same holds in reverse for `func_800C2FF0`: `u8`
  cell-size temporaries are not launched, so width-1/height-1 are computed
  first as in retail.
- The particle updaters' `move a0,a2` is a second pointer to the particle
  (`FieldAnimMovingParticle *particle = anim`) used only for the lifetime
  countdown after the bounce test.
- `func_800C5EB0` walks the strip nodes beside a `FieldStripNodeTail` view
  (the node seen from its brightness field, `pe1/field_textured_chain_node.h`):
  `body[-1].nextCellStep`, `body[1].edgeA` replace the raw byte offsets.
- The field engine object passes (`FieldEng_ObjectScript.c`) keep the slot
  table and script state in typed globals (`FieldEngSlot *`,
  `FieldEngState *`); the slot address the two dispatch loops add as an
  integer, index first, is a `FieldEngSlotAddress` pointer/word union.
- `FieldEng_GlowEffect.c` .. `FieldEng_BurstEffectC.c` are the eight effect
  module classes of `pe1/field_effect_modules.h`; their scripts, handler lists,
  slot sizes and tables are declared there under module names instead of the
  per-unit extern headers.

- A copy of a parameter that retail keeps in its own register survives cse
  when the parameter itself is reassigned afterwards: `func_800CE610` reads
  the list header through two copies taken before the `list` parameter
  advances to the entries (`list += 0xC`), which gives retail's `v0`/`t0`
  copies without pins or barriers. The `collision_step` post-increment
  (`step++ == 0x64`) loads the halfword once with `lhu` and sign-extends it
  for the test, as retail does, without a volatile view.

What is left (best crutch-free lev against retail):

- `FieldEng_AngleMath.c` (5 pins): `FieldEng_TurnToward` lev 15 and
  `func_800E0024` lev 6. Retail keeps the step and the turn delta in separate
  registers (`move v1,a2` then `negu v1,a2`): with `delta = step` followed by
  `delta = -step` cse makes the longer-lived delta the class head and rewrites
  `-step` as `-delta`, so step dies at the copy and global alloc gives delta
  step's register (a s16 delta, if/else and ternary forms tried). The same
  copy preference ties `func_800E0024`'s distance to the difference in `a0`.
- `FieldEng_CollisionQueries.c` `func_800C5EB0` (1 pin on the node pointer):
  lev 9 without the pin. Global alloc ranks the loop index (9 refs over 45
  insns) above the node pointer (11 over 59); retail has node, index, strip in
  s1, s2, s3. Dropping the index's reuse as the return value lets the strip
  outrank the index (lev 13); starting the index inside the `if` gives the
  node s1 but swaps strip and index (lev 17-21).
- `FieldEng_EffectPrimitives.c`: `func_800D2370` keeps one empty constraint on
  `rotation` and `length` (lev 71 without it: `rotation` is spilled to the
  stack instead of living in s8 across the packet setup calls).
  `func_800D3BC8` keeps the entry memory barrier, the `$6` clobber and the
  volatile stack parameters (lev 14 without them: sched2 interleaves the
  stack-argument loads with the register saves, retail loads them after the
  last save).
- `FieldEng_FourLayerGlowEffect.c` `func_800CAE0C` (5 pins, 1 barrier): lev 50
  as four `gte_CompMatrix` layers. Local alloc ranks the composed translation
  source (`&rotation.t`) above the second-column addresses and leaves the
  matrix second column without a register; retail spills the translation
  source instead (stack slot 192).
- `FieldEng_GeometryAndShapes.c` is excluded from this sweep (owned
  elsewhere); its three `_externs` headers stay with it.

### Open declaration conflicts

`python3 tools/scripts/decl_conflicts.py --count` lists symbols declared more
than once with different types (629 at the start of the header unification,
232 after it, 99 after the battle pass, 65 after the small-data pass, 12
after the actor and overlay view pass, 1 after the scalar view pass, 0 after
`GetClut` took its header declaration).

How the earlier groups were closed, for the next symbols of the same kind:

- Small data versus absolute access is solved per unit, not per symbol: a
  `-G8` unit that reads a small word absolutely while its own small data is
  `$gp`-relative takes `/* MASPSX_FORCE_G0: 1 */` with
  `--use-comm-section`, and tentative (COMMON) definitions of the words it
  does reach through `$gp`. MASPSX then emits `$gp` loads only for that
  COMMON metadata and every extern stays absolute, whatever its declared
  size. A unit whose `$gp` words are records read at a field offset also
  takes `/* ASPSX_VERSION: 2.70 */`, which lets MASPSX keep those offset
  accesses `$gp`-relative (`Geo_ActorFloorResolution`, `Menu_TextboxDraw`).
  A unit that also needs cc1 to treat a small word as expensive (address
  kept in a register) gets `-G0` for cc1 instead; that is why
  `Gpu_InitPipeline` is its own unit, ahead of the `-G8` frame loop. This
  retired the array, record and aliased views of the battle, task, boot,
  menu and field words, the actor list head (`D_8009D20C`,
  `g_FieldActorListHead`), the player (`D_8009D254` as a `BattleEntity`
  pointer, `g_PlayerEntity` as a `FieldActor` pointer), `g_ActiveActor`,
  `g_ActiveDrawSlot`/`D_8009CDDC`, `g_GameStateFlags`/`D_8009D1A0`,
  `D_8009D1F4`, `D_8009D2E8` and the task script cursor `D_8009CE00`.
  The symbol alias pass applied it to `Battle_EscapeAndPhaseInitFlow`,
  `Battle_DrawStatusPanel`, `Battle_InitEntityColors`,
  `Inv_InitMaxLevelInventory`, the three `-G8` capacity units and the `-G4`
  `Inv_CheckFreeSlotCapacity`, which retired the `battle_turn.h`,
  `battle_status.h` and `inventory_slots.h` views and the per-byte HUD color
  views. Where retail reloads a word after a field store, the plain scalar is
  not enough: GCC 2.7 lets a store to a structure field skip a fixed scalar,
  so `D_8009D29C` stays a one-element array and the `Combatant` flag store in
  `Battle_InitEntityColors` goes through a pointer to the field (an
  indirect store is not a structure field store). A field store that
  `g_GameState` and `D_800B0CEC` already describe is written as that field.
- Pointee views of one pointer (the room and scene records of the player,
  the actor list, the matrix and projection slots) became casts at the use
  to the view each unit reads, or the `FieldActor` field when it has the
  same offset and type; the generated code does not change.
- Element views of one table became a union element type
  (`GteSineWord` for the sine table `D_800966EC`) or the record the data is
  (`FieldGlowSprite` for the scene_e08 sprites, `DISPENV`/`DRAWENV` for
  `D_800BCE80`/`D_800BCDC8`, `AkaoTickTrack` for `D_800B8AC0`,
  `FontGlyphLoadState` for `D_800B0DD8`). Where a function needs the old
  array arithmetic, it casts the table at the use.
- Per-overlay data declared differently by two headers of one overlay got
  one owner header in the overlay directory (`room_m350_data.h`,
  `room_m273_boss_state.h`, `scene_e22_ember_data.h`,
  `menu_memcard_offset.h`).
- `BattleCmd_CommitAmmoAndUpdate` is called with the queued action and
  without it, as retail does, so `battle_cmd.h` declares it without a
  prototype.
- Scalar versus record access of one word: cc1 reads a scalar `extern`
  straight from its symbol, but any record, union, array or cast view goes
  through `memory_address`, which puts the symbol's address in a register
  when it is not small data, and CSE keeps it there for a read-modify-write
  or a reload. A record declaration and a record view cast from the scalar
  (`(*(T *)&sym)`) produce the same code, so the word is declared as the
  scalar and the record users read it through a view: `D_800BCF88` is the
  scroll state's flags word (`volatile int`, the grab effects' absolute
  read-modify-write) and `g_GeomScroll` the record view; `D_800B0CD8` is
  `g_GameState.flags` as a `u32` (the frame loop, process manager and CD
  boot read), while every record and array user reads `g_GameState`;
  `D_800B0CE6` is `g_GameState.story_day_flags` as a `u8`. The opposite
  case, a unit that needs the scalar to read like a register-held record,
  takes cc1 `-G0` (`Battle_EncounterInitializationFlow`); a unit that needs
  a record or volatile cast of a small word to read absolutely takes cc1
  `-G8`, which makes the small symbol a cheap address (`FxCommon_*`
  with the projection slot record `D_800BCFA8`,
  `RoomEffect_ConfiguredPulseCallback` with `*(volatile u16 *)&D_800E11E8`).
  The cc1 `-G8` trick needs a unit without small data definitions of its
  own: in `RoomFx_GrabAttackSet` its `.sdata` is discarded by the overlay
  link.
- The texture slot words `D_800E11E8`/`D_800E11EA` are declared once as
  `u16` entries of the slot table (`field_texture_tables.h`); the one-field
  record views became `TEXTURE_SLOT_RECORD`, the array views index
  `D_800E11E4`, and the volatile reads were not needed except in the one
  above.
- `g_RoomScriptRecord` is declared once (`room_script_record.h`) by the
  leading halfword every room's record starts with: room_m123 reads it as
  that scalar, room_m005 and room_m023 take its address through their own
  view macros, and `RoomLib_SetTaskVar0` returns its address.
- A unit in a shared overlay directory (room_lib) now counts towards the
  rooms whose splat config links it, so `RoomLib_CaptureFrameSample`
  (m156/m291/m380) and `RoomFx_EffectSequence` (other rooms) no longer
  collide at `D_80192C34`/`D_80192C38`/`D_80192C3C`.

Still open:

- Translation units that cannot merge: `Entity_CollisionDatabase` with
  `Entity_UpdateList` (`-G8` against default) and
  `Render_EntityAndTexturedQuadFlow` with `Render_DrawObjectAndClut` (GPREL
  relocation overflow).
- Symbol aliases that `MASPSX_FORCE_G0` cannot remove, because cc1 decides
  them before MASPSX runs:
  - `CD_StepReadState` reads the sound table pointer `D_800B0E08` four
    times with no store or call between the check and the use; one symbol
    lets CSE merge those loads, so its three views stay.
  - `Menu_MemCardProgress` (`D_800A19C0`, two views), `Save_StartWriteSlot`
    (`D_8009EE70` in `memcard.h`) and the scene_e20 hover orb
    (`D_800E2850`) rebuild a buffer or table address after a call or a
    store. With one symbol GCC keeps the address in a register (or, in the
    hover orb, an 8-byte spill slot). The text buffer matches only when it
    is declared at 8 bytes or less, which would misstate its 0x40 bytes.
  - `Battle_StepEscapeOrDeath` keeps the read and write views of
    `D_800B0D88` (`D_800B0CEC.flags_9C`): the player's flags load must stay
    after that store, which needs a non-small store address, and writing
    the field through `D_800B0CEC` makes the loop keep its address in `s1`.
- Crutches left in `src/main/field` and `src/main/anim` (2026-10-11 pass).
  Each was removed and rebuilt; the reason is what changed in the bytes:
  - `Anim_DecodeBoneRotationsShort` keeps the `$s2` pin on the angle
    pointer. The bone counter and the pointer both have 10 references, but
    the counter is dead between the translation loop and the bone loop, so
    its live range is two insns shorter. Global allocation then ranks it
    first and gives it `$s2`. Starting the counter first fixes the
    allocation, but sched2 then moves `move s3,zero` above the `lui`. No
    statement order of the first four assignments matches without the pin.
    Storing the Y sample through `angle->y` instead of `angleZ[-1]` gives
    the pointer the extra reference that fixes the allocation, but then the
    store is addressed from `$s2` instead of `$s1` (lev 1); splitting the
    two loop counters moves other registers (lev 26). Assigning the angle
    pointer only after the translation loop fixes the allocation but leaves
    the `lui` and the `angleZ` address (`lui`/`ori`) after the loop (lev 17).
    The frame byte offset is added through the `AnimSampleAddress`
    pointer/word union. Indexing the halfword samples by frame hoists the
    `<<1` below the `angleZ` setup, and `(u8 *)` arithmetic swaps the
    `addu` operands.
  - `Entity_SlideOnWall` keeps the `$s3` pin and both empty asm statements.
    Retail saves `s3` first and copies the entity from `$a0` in the
    `bgtz` delay slot, after both vertex loads. Without the barriers, sched1
    and sched2 put the two `lh` loads last for their load hazard. The
    parameter copy is a fixed head insn before reload, so neither a
    separate `entity = input` copy nor per-branch copies move it.
  - `Geo_LoadMeshEntry` adds 479 to the palette start in place before
    copying it to the running maximum. Retail loads the palette byte before
    the frame is set up; computing the maximum as `paletteStart + 479` makes
    the load a single-set pseudo that sched1 sinks next to its use (lev 6).
    The page pointer and `disp_x` are stored through pointers to the fields
    (`pageSlot = &entry->u34.pagePackets`, `dispX = &entry->disp_x`). GCC 2.7
    lets a load of a fixed scalar (the `D_800B1624` reload) pass a structure
    field store, but not an indirect store, so the reload stays after both
    stores as in retail; this replaced the `disp_x` memory barrier and the
    byte-storage view of `u34`.
    `Geo_TransformPoint` keeps its three pins and the barrier on the x sum.
    Without the pins, local allocation swaps `$t0`/`$a0` for the entry and
    y. Without the barrier, the unclamped x copy merges with the sum
    (lev 36 with neither; reusing `x` for the clamp bound or comparing
    before the bound copy changes nothing or is worse, lev 36-38). The
    `D_800B1624` pointer stays volatile: a plain pointer lets CSE reuse it
    across the `disp_x` store (lev 21). The
    four bound barriers are gone; loading the bound before the y add gives
    the retail order.
  - `Geo_ActorFloorResolution`: `Entity_FindFloor` adds the region slot
    offset to the table address as integers, through the
    `CollisionRegionTable` address/entries union (no casts). Indexing
    `D_8009CE08.entries` or starting the sum from the table pointer swaps
    `$v0`/`$v1` in both floor modes.
    `Entity_ResolvePosition` keeps the `$v1` pin on its index copy: retail
    copies the index to `$v1` before the plane-table test and masks it in
    each branch. Unpinned, CSE folds the copy into the parameter and both
    branches mask `$a1` (lev 5); reusing the copy for the Z argument in the
    sloped branch keeps it in the flat branch only (lev 1), and a `u16`
    parameter changes nothing. Masking the copy in place keeps the copy but
    in `$a2` (lev 18); reusing the parameter for the X argument still lets
    CSE fold the copy (lev 10).
    `Geo_PointInPoly` keeps the `$t3` pin on the parity flag: unpinned, the
    flag (10 references over 52 insns) ranks below the current Z (11 over
    42) and the two swap `$t3`/`$t4` (lev 10). Only a duplicated toggle adds
    the references that flip them.
    The ramp `goto slide` enters the slide code past the first floor search;
    a flag variable instead keeps a register and a test (lev 61).
- Crutches left in `src/main/memcard`, `src/main/save`, `src/main/entity`
  and `src/main/cdrom` (2026-10-11 pass). `src/main/save` is clean. Each
  site below was removed and rebuilt; lev is the Levenshtein distance over
  instruction words of the best crutch-free form tried:
  - CD and stream retry loops keep one `goto` each
    (`Overlay_LoadInitialImage`, `CD_FindNextDataSector`, `CD_StepReadState`).
    Retail hoists no invariant out of these loops: the range pointer, the
    `0xFEFFBFFF` mask and the `-1`/`33`/`42` constants are rebuilt on every
    pass. GCC's loop optimizer only runs on syntactic loops, so a `do`/`for`
    form hoists them into `$s` registers (lev 27 and 63).
  - `CD_FindNextDataSector` adds the range index to the range table as
    integers, index first, through the `CdRangeAddress` pointer/word union
    (`&ranges[i]` and `i + ranges` put the table first in the `addu`).
  - `CD_StepReadState` needs no slot macro or pin: the load-delay `nop`
    between `lw D_8009CDA4` and the gp-relative `sw` to `g_CdFadeStartTime`
    is lost only because MASPSX does not know an extern `.sdata` symbol is
    gp-relative. A tentative definition of `g_CdFadeStartTime` with
    `--use-comm-section` gives MASPSX its size, and it keeps the `nop`.
    The same recipe fixes any other `PE1_NOP` before a `$gp` store of a
    just-loaded value.
  - `CD_ReadSectors` (pins on `state`, `finished`, `scaledIndex`, `start`,
    barriers on `blocking`/`kind`, the volatile sector-directory reads)
    rebuilds at lev 47 without them. `CD_SeekToTrack` keeps the `$s1` pin
    and the count barrier: without the barrier combine knows the count is a
    `>> 22` and turns retail's `blez` into `beqz`, and without the pin
    `slot` and `finished` swap `$s1`/`$s2` (lev 49).
  - `MemCard_PollTransferDelay` keeps the `$a0` pin and the barrier on the
    pending-flag pointer. Without the barrier CSE folds `pending + 1` into
    an absolute address (`lui`/`lw 4`) where retail indexes `$a1` (lev 4);
    `&pending[count - 2]` and `pending -= 2` change the allocation (lev 10
    and 17). The pin is only needed with the barrier.
  - `MemCard_AbortActiveOperation` keeps the barrier that separates
    `name_buf` from `D_8009EE70`: with one value CSE keeps the buffer
    address in `$s0` across `sprintf` instead of rebuilding it (lev 31), the
    same effect as the `Save_StartWriteSlot` alias above.
    `Save_StartWriteSlot` keeps its `$s0` pin and both barriers (lev 23
    without; lev 1 with only the pin).
  - `Entity_AllocBlock` keeps its m2c pins and two `goto`s. The structured
    loop (u8 `blocks`/`slot`, the scratch base read inside the loop) is
    lev 1: reorg fills the not-free branch's delay slot from the
    fall-through instead of stealing the loop test's `andi` from the target,
    so the branch lands one insn early. A separate `next` index fixes the
    target but swaps `$a2`/`$a3` (lev 10 to 19 over all declaration orders).
    `Scene_UpdateEntityPositions` keeps its `goto link`: duplicating the two
    inlined `Scene_LinkContactTask` calls is not cross-jumped (lev 128).
  - `Entity_TriggerAnimEvent` keeps its three pins (lev 81 without: retail
    copies `entity` to `$a2` first). `Task_RunEntityCommand` keeps its two
    argument-register pins: every statement order allocates the `1` and the
    stack value to `$a0`/`$a1` the other way round (lev 6).
    `Entity_RelocateSceneData` keeps the `$v0` pin on the loaded block
    (lev 15) and adds region offsets to the database as integers: an
    indexed `regions[i]` walk reorders the header loads (lev 9 to 19).
  - `Entity_ResetStateGlobals` clears both ramp edge points by byte offset
    (`D_8009CE0C`/`D_8009CE0E`); a `FloorEdgePoint` index keeps a separate
    counter and `addiu` base (lev 11). `Entity_ResetAllPools` clears the
    allocation slots by byte offset under `-fno-strength-reduce`.

- Battle (`src/main/battle`, 2026-10-12 pass). The local extern lists, the
  asm symbol aliases (except the one pair below) and the byte-offset field
  macros are gone; the recipes that replaced crutches without changing a
  byte, for the next battle or HUD function:
  - GCC 2.7 CSE drops a cached global after a store through a pointer that
    is not a structure field (`u32 *flags = &core->coreFlags; *flags = ...`)
    and after a QImode store, but not after a structure field store. Retail
    reloads that look like volatile or need a memory barrier usually come
    from one of those two (`Battle_SubActionStep`, `Battle_ActionLifecycle`,
    `Battle_InitEntityColors`, `Battle_ResolveHitOnTimer`).
  - Field-by-field copies of packed words are bitfield assignments
    (`BattleAttackBits`, `BattleTurnBits`, `BattleEffectBits` in
    `battle.h`): a 1-bit field at bit 16 is read through its halfword, which
    is the `lhu` retail has (`Battle_PadStateSyncFlow`,
    `Inv_BuildWeaponList`).
  - Units that read small words absolutely take `-G8` with
    `MASPSX_FORCE_G0` and COMMON definitions of the `$gp` words, plus
    `ASPSX_VERSION: 2.70` when a `$gp` word is a record read at an offset
    (`g_HudOrigin`) or a byte array stored at nonzero offsets
    (`Battle_SetupEntityTarget`, three `PE1_NOP_DEP` barriers retired).
  - `(u16)(s8)byte` at the call reproduces the enemy action-mode argument;
    `(s16)(word >> 16)` and `(u16)(word >> 16)` read the integer half of a
    16.16 field with one `lh`/`lhu`.
  - A base pointer local (`BattleTileSlot *tiles = D_8009E070`) keeps a
    packet base in a register where retail loads it early.
  - The hit-state, category and reaction inserts into an enemy's core flags
    are `EnemyCoreFlagBits` bitfield assignments; each insert then reuses
    the previous value's register (retail's `$3` chain). The reaction byte
    is read through the `BattleEvasionReaction` record view, which keeps the
    load after the flag stores and the middle store alive
    (`Battle_ResolveHitOnTimer`, one `ApplyEvasionOutcome` helper for all
    four targets; the steal path reaches it by cross-jumping).
  - A narrower copy keeps a register copy alive: `s16 centered = bound`
    stays a `move` where an `s32` copy is folded into its source.
  - Retail fields of the partner render object that splat split into their
    own symbols (`D_800B0D8A`, `D_800B0D88`) are `D_800B0CEC` fields; GCC
    derives the object address from the field address as retail does, and
    only the relocation addend changes. The same holds for the end-of-battle
    panel color bytes (`g_BattleEndPanels[slot].color`).
  - The case-into-if switch shape (`case 0: if (!(rand() & 1)) { case 2:
    ... }`) replaces a goto into a shared resistance body, and a separate
    `if (damage <= 0)` after the scaling block replaces a goto into the
    minimum-damage branch (`Battle_StepEnemyMovement`).
  - An if/else with two calls keeps the branch that `x = 1; if (c) x = 0;
    f(x)` folds to `sltiu` (`Battle_Update` item menu context).
  Crutches left, each removed and rebuilt (lev without it):
  - `Battle_DrawActiveStatus`: the background pointer and the index of the
    last `AddPrim` are both born in `v0`; as pseudos they take `a0`/`v1`
    and the index load moves above the background store (lev 33 and 13).
  - `Battle_DrawEnemyHP` (14 pins, 9 barriers): retail adds the frame and
    digit offsets before the sprite base, which neither typed indexing nor
    a sprite-slot view reproduces (lev 136, byte arithmetic lev 160).
  - `Battle_ProcessActionSlot`: the room argument is set in the delay slot
    of the actor test (lev 1) and the kind is compared against a fresh 1
    where GCC reuses the result register (lev 21). `Battle_UpdateEnemy`:
    a plain copy of the core is folded into `enemy`, swapping the saved
    registers of actor and enemy (lev 129); the shared death step is laid
    out as its own block when duplicated (goto, lev 19).
  - `Battle_StepEscapeOrDeath`: see the `D_800B0D88` views above (lev 42).
  - `Battle_SetupPlayerPalette`: `c2`/`c7` are single-set constants, so
    sched1 launches them late (lev 35).
  - `Battle_AdvanceTurnSlot`: retail shares the queue-index store between
    the loop entry and the loop body; a `while` loop rotates the other way
    (goto and barriers, lev 27).
  - `Battle_DrawStatusPanel` (typed digit packets lev 294) and the four
    phase steps of `Battle_EscapeAndPhaseInitFlow` (62 pins) were not
    reduced further; `Battle_DrawHPBar` is crutch-free (recipes below).
  - Second half of the directory (`Battle_HaltOnPositiveX` to
    `Math_IntSqrt`), measured again after the recipes above:
    - `Battle_StepEnemyMovement` (1 pin): in global allocation the
      resistance mode (7 refs over 137 insns) outranks `enemy` (9 refs
      over 285 insns), so they swap `s4`/`s5` (lev 22). Naming `enemy`
      instead of its copy in one more read makes it win (lev 1, but that
      load then uses `s4` where retail uses the copy in `s2`); moving the
      mode's initialisation or retyping it does not help (lev 18 to 71).
    - Constant hoisting (`Battle_Update` gauge recolor: 6 pins and 7
      barriers; `Battle_StepPostBattle` cases 2 and 4: 6 pins, 3 barriers
      and the volatile panel view, measured one at a time lev 2 to 38;
      the `Battle_StepVictory` stat reset and `Battle_StepPostBattle` case
      3 are solved, see the recipes after this list): retail loads
      every color or edge constant at the top of its block and leaves the
      later load-delay slots empty. Without the pins sched1 places each
      `li` in the first load stall after its use, whether the constant is
      a block local, a function-scope variable set in two cases (no longer
      LAUNCH priority, still a stall filler) or a `u8` (lev 75, 60 and 103
      for the three blocks). The `asm("" : : "i"(n))` barriers in
      `Battle_Update` also keep cross-jumping to the last store only;
      without them blocks 1 and 3 merge (lev 299).
    - `Battle_StepVictory` (`result`, `object`, `brightness` pins):
      retail jumps from case 1 to the byte truncation in case 5 before
      register allocation, so the brightness temporaries share `s0`;
      written as a goto or as two tails the temporaries take `v0` (lev 25).
      Without the `result` pin the `result = 1` (set twice, so not
      launched) fills an early load stall instead of the delay slot before
      the death-animation call (lev 3).
    - `Battle_StepAyaAction` (6 pins, 3 barriers left; cases 0 and 9 are
      plain C, see the recipes after this list): case 11 restores the pose
      through the position and rotation records at lev 41, all of it one
      `lh` of `posZ` that sched1 hoists into an earlier load stall where
      retail reads it after the rotation stores (a `z` local, a reload
      through `D_8009D254` or a rotation record copy do not keep it there).
      Case 6 without the product barrier is lev 8 (`mflo` lands in `t1`, not
      `v1`; inline products, two locals, `<< 4` and operand order do not
      change it). Case 15 written as `flags &= mask` with `mask` assigned
      twice keeps the two `and`s (lev 11; the one-statement mask folds them,
      lev 29), but the flags load moves above the visibility store and the
      registers rotate.
    - Stack reserves: `Battle_Update` keeps 0x1A0 and
      `Battle_StepPostBattle` 24 unused frame bytes; no local of the
      function accounts for them.
  - Recipes found in the 2026-10-12 battle debt pass (each replaced pins,
    barriers, gotos, casts or volatile with the same bytes):
    - Packet arrays are typed (`RenderPagedSpritePacket g_BattleHpDigits[2][4]`
      and the rest of `battle_status.h`) and indexed naturally. When retail
      keeps the sprite base (array + 8) in its own register and adds the
      offset to it, the stores go through `BATTLE_SPRITE_SLOTS`, a cast of
      `&array[0][0].sprite` to rows of `BattleDigitSlot`: as a memory
      reference GCC adds the shared index offset to the hoisted `array + 8`
      while the packet argument and the clut store keep `array` itself
      (`Battle_DrawHPBar`, six `goto` loops, five pins, two barriers and
      eight pointer casts retired). An assignment as the stored value
      (`tile.r0 = shade = shades[i]`) expands the store address before the
      load, which is retail's order.
    - A statement macro instead of a `static inline` sound helper:
      inlining gives each `g_AkaoBgmHandle[0]` read its own `lui`, the
      direct code takes the table address into a register once, as retail
      does (`PLAY_TABLE_SOUND` in `Battle_TargetAndEnemyTurnFlow`, which
      retired the four volatile handle pointers, two pins and two barriers).
    - Globals that retail reaches as one object are records:
      `g_AyaSavedPos` (three words) and `g_AyaSavedRotation` (three
      halfwords) keep the player loads and the saved-pose stores
      interleaved, because GCC 2.7 lets a structure store alias any other
      structure access but never a scalar global. The evasion outcome is
      `g_EvasionOutcome` (`BattleEvasionOutcome`, at `g_EvasionStatus`):
      its reaction is read back after the core flag bitfield stores. Under
      cc1 `-G0` the record address is CSE'd into a register (lev 57), so
      the unit moved to cc1 `-G8` with `MASPSX_FORCE_G0`, which leaves the
      other words absolute (`Battle_StepAyaAction`). The two byte names
      remain for `Battle_ResolveHitOnTimer`: its ASPSX 2.56 addresses
      `g_EvasionOutcome+1` absolutely, ASPSX 2.70 or later through `$gp`.
    - Constant gradients and colors are `BATTLE_SET_GAUGE_GRADIENT` and
      `BATTLE_SET_SPRITE_COLOR` on the typed arrays: GCC sorts the byte
      stores by value itself (`Battle_StepVictory`, 6 pins and the interior
      byte symbols). Quad corners are `BATTLE_SET_QUAD_XY` on a pointer
      formed from a local copy of the array base (`panels = g_BattleEndPanels;
      panel = &panels[slot]`), which loads the base where retail does
      (`Battle_StepPostBattle` case 3, the volatile view, two barriers and
      five pins).
    - An `s16` parameter on an inline helper keeps CSE from reusing a
      caller's `index - 1` for the helper's own `initial - 1`: the
      conversion gives the helper a separate value, and it folds away
      afterwards (`RewindTurn` in `Battle_AdvanceTurnSlot`, which retired
      the `savedIndex` barrier; `int` and `u8` parameters keep lev 26).

### Small-data recipes that replaced crutches (2026-10-12 pass)

These apply to any unit, not only the ones they were found in:

- A `PE1_NOP`/`PE1_NOP_IO2_DEP` slot between a load and a `$gp` store of
  the loaded value stands in for the load-delay `nop` ASPSX emitted. A
  tentative definition of the stored word in the unit plus
  `--use-comm-section` gives MASPSX its size and it keeps the `nop`
  (`CD_StepReadState`, `Scene_LoadMap`, `Battle_StepCharacterAction`).
- 16-byte `struct { char _[16]; } X __asm__("sym")` views in a `-G8` unit
  only make cc1 treat a small word as absolute. `MASPSX_FORCE_G0: 1` with
  `--use-comm-section` and tentative definitions of the words retail reaches
  through `$gp` makes every other word absolute, so plain declarations give
  the same code (`Battle_BuildTargetList`, `Battle_Init`,
  `Battle_PlayerTurnFlow`, `Battle_FinalizeAttackResult`,
  `Battle_AdvancePhase`, the forced-G0 task units).
- Views that only separate reads of one word for CSE give way to one-field
  record views (`pe1/battle_word_records.h`, `pe1/draw_slot_record.h`,
  `g_AyaBonusPoolRecord`, compare `room_sound_slot.h`). A record read is a
  structure access: GCC reloads it after a store through a structure
  pointer, where a scalar at a constant address is assumed not to alias and
  is reused. At one word the record is small data, so a -G8 unit addresses
  it directly; under `-G0` the unit switches to cc1 `-G8` with
  `MASPSX_FORCE_G0` so the record does not get its address CSE'd into a
  register (`Battle_DrawSpellName`). The linker map names the record at the
  word's address. `Battle_ApplySpellEffect` lost 33 views this way.
- Scalar views do the opposite job: `g_EquippedWeaponSlot`/`ArmorSlot`
  (`pe1/aya.h`) are byte scalars, so in a forced-G0 -G8 unit every slot
  access is a direct `lui`/`lb` or `lui at`/`sb` instead of an address held
  in a register (`Inv_TransferItem`, `Inv_RemoveActiveListItem`).
- A masked read-modify-write chain with hoisted `lui`/`ori` masks, or a word
  stored and immediately loaded again, is a bit-field store in the original:
  `g_BattleResultFlags` (Scan, `BattleResultFlagsRecord.bits`),
  `Combatant.stateFlags` (`COMBATANT_STATE_BITS`, Haste) and the
  `BattleTurnBits.actions` read (a bit-field read loads the word and masks
  it, where `turnWord & 0xF` is narrowed to `lbu`) replace volatile casts.
- A read after an outgoing stack argument store is reloaded only when it is
  a structure access: `g_AkaoBgmHandle[0]` (array element) replaces the
  `void * volatile *` pointers of the menu sound helpers and
  `Menu_MainUpdate`.
- An inline helper keeps an index scaled before the table base is added:
  `&D_800A1E64[kind - 5]` written directly folds the -5 into the address
  (lev 16); `AmmoPoolRecord(kind - 5)` does not (`Inv_ArmorCapacity.c`).
- Under `-G0` every extern is absolute anyway; the views in
  `Scene_UpdateBgDraw` and `Scene_LoadSceneData` were unnecessary.
- An array bound changes the small-data decision: `g_TaskNodeSeqCounter[1]`
  is small, `[]` is not. Declaring a symbol as an incomplete array before
  its tentative definition in the same unit also makes it large, so
  `Boot_FrameLoop` includes `pe1/boot.h` after its definitions.
- `CdRom_PollReady` is an `extern inline` in `pe1/cd_poll.h`: the boot disc
  loaders inline it as retail does (the copy into the return register is
  what the old `$v0` pins and status barriers imitated), and
  `CdRom_BootReadFlow.c` keeps the out-of-line definition.
- A loop that walks a table by index rather than an incremented pointer
  leaves the table pointer single-set, and sched1 then places its address
  load late (`MemCard_InitManager`, no pins).
- Pointer/word unions (`CdRangeAddress`, `PeSectorTableAddress`,
  `SceneCopyCursor`) replace `(u32)` casts where retail adds an index to a
  table address with the index as the first `addu` operand.
- Overlays link against their own symbol maps, so a header symbol whose
  `__asm__` label is the name overlays use (`g_ActiveDrawSlot` in
  `boot_display.h`) keeps the label until the overlays are renamed too.

### Crutches left in scene, task, gpu, item, boot and main (2026-10-12 pass)

Each site was removed on its own and rebuilt; the lev given is for that
single removal (none was byte-identical):

- scene: `Scene_LoadSceneData` keeps the `$t0` `copyEnd` pin (lev 5) and the
  destination-word barrier (lev 2). `Scene_LoadEntityTexture` keeps the
  `$s0` loop pin (lev 9), the record barrier (lev 3), the packed-word
  barrier (lev 9) and its stack reserve. `Scene_TickTimers` keeps the `$a1`
  entry pin (lev 30). `Scene_UpdatePlayerEntity`/`Scene_UpdateEntityFacing`/
  `Scene_UpdateEntityFacingFromPad` keep their angle launders (lev 1 each:
  CSE folds the masked angle into the previous one) and the pad memory
  barrier (lev 7). Retry `goto`s in the room loaders stay for the loop.c
  reason given for the CD loops.
- task and gpu: see "Task and GPU units (src/main/{task,gpu})" below.
- item: `Inv_SetActiveList` (lev 9 to 36), `Inv_GetSlotItemData`/
  `Inv_GetActiveSlotCount`/`Inv_IsSlotEquipped`/`Inv_IsSlotSelectable` empty
  asm statements (lev 18 to 61: they stop the shared slot lookup being
  cross-jumped), `Inv_ClearDisplaySlots` (lev 12),
  `Inv_CompareItemsForSort` (two memory barriers, lev 22 and 4; moving the
  function-address load among the three reads does not help, lev 22),
  `Inv_CheckItemEquippable` (lev 5 to 40), `Inv_FindFirstEmptySlot`
  (lev 16, 49), `Inv_RebuildSelectionBitset` (lev 17 to 36),
  `Stat_QueryLevelAndSubLevel` (lev 65). `Inv_EquipItemById` (formerly
  `Inv_FindItem`) and `Inv_InitMaxLevelInventory` are crutch-free.
  `Inv_TransferItem` keeps three `goto`s: the refusal path stores 0 into the
  result after the success path's return, so `return result` must sit at a
  join (`move s4,zero; move v0,s4`); `return 0` and `result = 0; return`
  give `move v0,zero` (lev 23), a `do`/`while (0)` with `break` lev 48 to 65,
  a refusal flag lev 53. `Inv_SetActiveList` case 5 keeps its pins: the
  natural form allocates the action pointer to a3 and the selection to a1
  (lev 37). `Inv_RemoveActiveListItem` keeps one m2c integer that holds
  first the list pointer and then the removed item (typed forms swap
  v0/v1, lev 2). The six local item
  record lookups became `Inv_LookupItemRecord`/`Inv_LookupItemRecordScaled`
  in `pe1/inventory_slots.h`.
- boot: `Field_HandleStateTransition` (lev 13 to 48), `Gpu_InitPipeline`
  (pins lev 11 to 13, barrier lev 37: the constant clamps are not folded
  without it). `Boot_DiscTableLoad` keeps its retry `goto`s (loop.c).
- main: `Battle_ApplySpellEffect` is crutch-free. `Battle_BuildTargetList`
  `finalCount` pin and barrier (lev 5), `Battle_DrawStatusOverlay` (lev 2
  to 7), `Battle_StepCharacterAction` (lev 2 to 47; the queue offset pin
  alone lev 3, a structured rewrite with record views lev 216 because the
  retail block order follows its `goto`s),
  `Battle_UpdatePlayerTurn`/`Battle_ApplyPlayerHit` (lev 11 to 20),
  `Battle_DrawTargetHighlight` (lev 2 to 24), `Battle_DrawSpellName` (lev 2
  to 15; a typed `BattleGaugePrim [2][10]` rewrite lev 224: retail keeps the
  packet base in a register). `Battle_AdvancePhase` keeps the copy of the
  sound table as m2c wrote GCC's block move (the struct assignment matches,
  but the target search's found block then moves out of the copy, where
  loop.c placed it in retail), its `goto`s, and the scaled queue reads in
  the two finish tails (array indexing gives the index another register,
  lev 6). `Battle_DrawStatusValue` keeps two byte offsets (typed line and
  marker views lev 13, ordering index lev 2). `Battle_ApplyEnemyAttack` keeps
  two volatile `stateFlags` reloads: bit-field forms drop the store and
  reload pair (lev 52 with branch offsets, 2 instructions). `Battle_UpdateEntityFacing` keeps its two `goto`s: any
  conditional form of the facing mode becomes an `slti` store-flag or the
  folded unsigned range test (lev 16 to 21). `Battle_DrawStatusOverlay`
  case 3 keeps its packet base pin (typed sprite forms lev 7 to 23).

### Battle HUD recipes and crutches left (src/main/battle, first half, 2026-10-12)

Recipes that removed crutches here and apply to other HUD code:

- PSY-Q macro shape. Retail indexes the draw-buffer packet arrays through
  `(&array[D_8009CDDC][i])->field`, the shape the `setXY0`/`setRGB0` macros
  expand to. GCC turns that into pointer arithmetic, `(f*stride + i*size) +
  base`, and keeps the base in a register across a loop. A direct
  `array[f][i].field` gives `(i*size + f*stride)` with the symbol folded into
  the store (`MAX_HP_DIGIT`, `PANEL_DIGIT`). When retail adds the base before
  the store offset of a sprite, declare the sprite-first view
  (`BattleDigitSlot`) so the symbol is the sprite, not the packet.
- Branches that store the same tail (`.b = 128` in both colour branches,
  the four background tiles) should repeat it in each branch: retail's
  shared block is GCC cross-jumping, not source.
- HUD colour resets that use constant indices (`[0]`, `[1]`) are plain
  `BATTLE_SET_GAUGE_GRADIENT`/`BATTLE_SET_SPRITE_COLOR` calls in the natural
  per-quad order. The scheduler reorders the stores and groups the constants
  the way retail does. The macros are comma expressions: a `do { } while (0)`
  body or an inline function changes the result (lev 96 and 62).
- A test of the form `if (D == 0 || ...) return; do { D--; } while (...)`
  is the inverted loop that retail's entry jump comes from (`RewindTurn`).
- A copy register that retail keeps for one pointer (`statusCore`) comes
  from reading the field twice, before any store.
- A temporary that only feeds two stores and must not share a register with
  an earlier variable is block-local (`finisher` in
  `Battle_PhaseInitEnemyTurn`).
- A result flag that must not be merged with the constant 1 by CSE is an
  `s8` (`Battle_ProcessActionSlot`).

Crutches left; each was removed on its own and rebuilt, lev for that removal
or for the best crutch-free form tried:

- `Battle_EscapeAndPhaseInitFlow.c`: the per-frame gauge colour cases in
  `Battle_PhaseInitEnemyTurn`, `Battle_PhaseHitReaction` and
  `Battle_PhaseEndTurn` (all register pins and empty barriers of those
  functions). Retail loads all five gradient constants before the third
  store of each case; every crutch-free form tried (struct fields, the
  gradient macros, shared or block-local variables, `u8` constants, an
  inline helper, a volatile draw slot, even `-fno-schedule-insns`) loads each
  constant right before its first use (lev 20 to 60 per case, 260 for the
  four HP cases together). The `asm("" : : "i"(n))` statements stop
  cross-jumping from merging more than the last store of each case.
  Measured again on a one-function copy of a case (2026-10-12): plain code
  already gets retail's registers (the last-born constant in `a0`), and a
  loop note after the first store (`do { ... } while (0)`, which sched1
  treats as a full barrier) gets retail's schedule, but the loop weights
  the references inside it and the constants then take `a0` upwards in
  birth order (lev 18 for the best of 288 barrier placements, types,
  declaration orders, `register` and inline-parameter forms). No single C
  form gave both.
  `moveLock` in `Battle_PhaseInitEnemyTurn` (lev 12: the flag store sinks
  below the player motion stores). The memory barriers in
  `Battle_StepEscapeOrDeath` and `Battle_StepEntityDeath` (lev 41: retail
  reloads `entityFlags` after storing it). The three stack reserves are
  retail's unused frames.
- `Battle_DrawStatusPanel`: the MISS branch keeps seven pins and two
  barriers. Its five byte stores need the folded symbol and `f*0x578 +
  s*0x8C` order; the macro form wastes 8 to 48 stack bytes because combine
  leaves `(use)` insns for the deleted base registers, the direct array form
  computes the slot offset first (lev 73 to 128).
- `Battle_ActionSlotAndEnemy.c`: `room` (`$4`, lev 27: one delay slot,
  the rest is branch offsets), the `actor` barrier (lev 129: global
  allocation swaps s0/s1), the `stepDeathAnimation` goto (lev 184 for an
  inline search helper).
- `Battle_DrawStatusPanel`: the MISS branch with the panel as
  `(g_BattlePanelDigits[frame] + slot)` computes the frame offset first as
  retail does, but the five byte stores then leave five `(use)` insns
  after combine and 40 unused frame bytes (lev 77; the `(use)` comes from
  combine's REG_DEAD note placement when the base pseudo of a three-insn
  combination disappears).

### Task and GPU units (src/main/{task,gpu})

Recipes that removed crutches here (2026-10-12, second pass). Each one was
found by rewriting the whole function from the retail listing rather than
deleting one pin at a time:

- A dispatch `switch` whose decision tree splits at the wrong case is
  missing a case. `Task_DispatchScriptCmd` has a no-op opcode 2100
  (`case 2100: break;`): with it the tree pivots at 2500 as retail does and
  the 23-label `goto` tree became a plain switch. Count the case nodes
  `balance_case_nodes` sees before blaming the compiler.
- Two branches that end in the same store sequence are cross-jumped into
  one tail when each branch does the whole update itself
  (`Task_SetObjEntryFlag80`/`Task_ClearObjEntryFlag80`: each table branch
  computes its triangle as `index * size + table` through the
  `CollisionTriangleTable` word and sets the flag; the shared `addu`/`lbu`
  tail is the compiler's, not the source's).
- A copy of a value into a second variable, or reuse of one variable for
  two roles, shows up as retail's register reuse:
  `Task_TurnTowardPointStep` turns its target coordinates into the deltas
  passed to `ratan2` (`x = (pos_x - x) >> 16`), and its wrap tests use a
  fresh block-local `wrap`.
- Store order of several fields follows the natural statement order once
  the value has one name (`Task_SnapPosToSpawnPos` stores x, y, z).
- Block-local single-set temporaries (an inline `AdvanceCursor(dx)` that
  reads x and y and writes both back) replace function-scope `x`/`y`
  reused across calls; `Draw_PrintTimeValue` lost both memory barriers.
- Inline helpers with their own parameter copies reproduce retail's
  copies: `Draw_PrintCenteredTextInWidth` is `PushCursor`, the inline
  `Draw_MeasureTextWidth`, a `SetCursor`, a print loop that saves the
  known cursor (`SaveCursor`) and two `PopCursor`s.
- PSY-Q GTE macros cover the hand-pinned GTE sequences:
  `gte_SetBackColor`, `gte_SetColorMatrix` (`Gte_SetBackColor`,
  `Gte_SetLightColor`) and `gte_MulMatrix0` (`Task_SetGteMatrix`).
- Address arithmetic on a pointer as an integer goes through a union
  (`RelocAddress`, `TaskNodeAddress`, `DrawPrimLink`, `TimRelativeTable`).
  Declare the union in the innermost block that uses it: a union declared
  at function scope gets the older pseudo, CSE then rewrites the packet
  pointer's later uses through the copy and an extra `move` appears.
- Plain argument use beats a laundered copy: `Task_RunInvCommand` and
  `Task_OpenMemCardDialog` match with the argument block and the shared
  `return 1` written directly.
- `sll`/`sra 16` before every use of a value, plus plain `move`s between
  copies of it, is a `short` local: GCC 2.7.2 on MIPS does not promote
  HImode variables, so each use as `int` re-extends it, while assigning a
  masked `int` to it is a bare copy. `Camera_StepAngleFade` lost 8 pins,
  4 barriers and 7 `goto`s this way (`short current, target, step`, with an
  `int` copy of the angle and target for the first two reach tests and the
  wrap test written as `(sum & 0xFFF) <= target`).
- GCC 2.7.2's alias rule says a scalar at a fixed address never conflicts
  with a structure member reached through a pointer. A load of such a scalar
  is hoisted out of loops and scheduled across member stores; a record view
  of the scalar (`g_PmTables`, a one-element array like
  `g_TaskNodeSeqCounter[1]`) keeps it in place without volatile.
- A stored packet field read straight back is reloaded by itself: any store
  through a pointer invalidates every remembered memory value in CSE, so
  `Draw_EmitGlyph` and `Draw_EmitDigitSprite` need no volatile views for
  their vertex reloads. A reload of a value stored in both arms of an `if`
  comes from cross-jumping the two stores into the join
  (`Draw_AllocColorTri`'s pulse shade).
- PSY-Q `addPrim` is the 24-bit tag bit-field (`RenderPacketTag.link`):
  `p->tag.link.address = ot->link.address; link.packet = p;
  ot->link.address = link.word;` reproduces the mask-and-merge sequence, and
  for `Draw_AllocSprite`, `Draw_EmitGlyph`, `Draw_AllocTexturedQuad` and the
  wipe triangles it replaced the tag pins and barriers. The `setRGB0`
  colour store with the code byte kept is `RenderPacketColor.fields.rgb`.
- A `case` that ends with the same `return` as the next label falls into
  it: `Boot_MemCardStepInit` state 0x3B shares `default: return 1` instead
  of an empty barrier.
- Allocations written in their own blocks (`u8 *old`, `u8 *next` declared
  inside) give retail's registers where function-scope temporaries reused
  for the second allocation do not (`Draw_AllocSprite`).
- A variable reused for a second role is retail's register reuse:
  `Camera_TrackEntityAngle` keeps the remaining distance in
  `angle_difference`.

Crutches still in these units, with the best crutch-free lev found:

- `Task_InitGpuHwRegs` (7 pins, 2 barriers) and `Task_GpuFlushPrimQueue`
  (10 pins, 4 barriers) are the additive generator around the
  80-byte `.text` state block at 0x80070E04, the same original-assembler
  module as `Task_GpuPackPrimColor` (see ASM_AND_GTE_POLICY.md): absolute
  addresses built with `lui`/`ori`, constants loaded with `ori`, `or` moves,
  no frame, hand-filled delay slots. GCC emits none of these (crutch-free
  lev 22 and 24). They should become `GAME_ASM_FUNCTION` units with
  evidence entries in a reviewed change; this pass adds no assembler. The
  queue offsets are now added through the `TaskGeneratorAddress` union
  instead of integer casts.
- `Boot_MemCardStepInit`: 3 re-dispatch `goto`s (a `for`/`continue` loop
  lets loop.c hoist `0x400000` and `1`, lev 39; tail recursion re-runs the
  `&g_GameState` load after the re-entry label, lev 24). Its empty barrier
  is gone: state 0x3B falls into the `default: return 1`, which shares the
  `li v0,1` jump as retail does.
- `Task_LoadSceneData`: 2 CD retry `goto`s (a loop lets loop.c hoist the
  `-1` comparands into saved registers, lev 32).
- `Akao_ProcessCommand`: 1 `goto archive_match`; retail places the match
  block after case 0x199, a structured `break`/flag places it in the loop.
- `Task_SpawnChildNode` (6 pins, 2 barriers): crutch-free lev 19. Viewing
  the free-list head as a one-element array (the `g_TaskNodeSeqCounter[1]`
  recipe) fixes the store order the memory barrier holds and the node
  register (lev 12 with no else-branch crutch, 8 with the `$4` state pin);
  what remains is the `g_CurrentEntity` load, which retail schedules right
  after the argument load in both branches.
- `Task_SetEntityActionAndWait` (1 pin): lev 2 to 3; retail loads the
  current frame first into `v0`, every crutch-free form either loads it
  second or allocates it to `v1`.
- `Task_WaitAnimRange` (1 pin): instruction sequence exact, lev 19 from the
  global allocation of `next`/`target`/`count` (retail `a0`/`a2`/`v0`).
- `Camera_TrackEntityAngle` keeps its 8-byte `stack_pad[2]` frame reserve
  (lev 18 without it).
- `Draw_AllocColorQuad` (13 pins, 5 barriers): with every crutch removed
  the instruction sequence is identical and only the register assignment
  differs (lev 138: the first quad in `s1` with `height` in `s0`, the
  packet copy in `t2`, the masks in `a0`/`a2`); declaration order, block
  scope for the first quad and dropping the `w`/`h` copies do not change
  the global allocation order (lev 138 to 153).
- `Draw_TextPrimitives.c`: `Draw_EmitGlyph` keeps the `$4` vertex pointer
  pin and the descriptor barrier (lev 44 without them: retail copies the
  packet into `a0` for the vertex stores, an inline vertex helper or a
  block-local copy is merged back into the packet register);
  `Draw_AllocTexturedQuad` keeps the `$4` GetTPage argument pin (lev 2:
  `move a0,zero` is scheduled later), the `$5` half and `$6` x pins and
  three barriers (lev 5 to 15 each; `spacing >>= 1` in place of `half` lev
  16); the `DecodeGlyph` `$4` pin stays (a `u8` copy of the code lev 10, a
  `u8` parameter lev 14: retail masks the code byte again for every use but
  keeps the masked glyph in `v1`).
- `Draw_WipeAndGradient.c`: `Draw_AllocColorTri` keeps the tag tail of its
  tile, one pin and two barriers (lev 27 without them: the code byte load
  is scheduled above the width store and the cursor loads move); its pulse
  shade and colour clear are plain C now.
- `Gpu_InitDisplay` (3 pins, 1 barrier) repeats `Boot_InitMemoryLayout`
  (sys, also pinned) on the `volatile s32` buffer words of `pe1/sys.h`;
  both should change together with that header.
- `Gpu_SetupSprites.c`: the status-icon packets (`D_8009E960[2][13]`,
  `D_8009EC38[2]`, 28 bytes: draw mode then sprite) are addressed from the
  sprite part at +8 (`D_8009E968`, `D_8009EC40`) with byte offsets (7 byte
  additions, 2 raw offsets) and `Gpu_DrawStatusIcons` keeps 8 pins. A typed
  `{ tag; mode; RenderSpritePacket sprite; }` array reproduces every
  instruction except the register base, which retail points at the sprite
  part (lev 8 from the immediates). `battle_status.h` now types
  `D_8009E960` as `RenderPagedSpritePacket[2][13]` for `Battle_DrawHPBar`;
  `D_8009E976` and `D_8009EC38` are still byte arrays for this unit.
- `Gpu_SetupSprites.c`: the status icons are typed now
  (`g_StatusIconPackets[2][13]`, `g_StatusMarkerPackets[2]` in
  `pe1/draw_state.h`, linker names beside the battle byte arrays), and
  `Gpu_SetupSprites`/`Gpu_QueuePrimitive` are plain field accesses.
  `Gpu_DrawStatusIcons` keeps 8 pins and the integer address sums in its
  three emit macros (3 byte additions): retail adds the slot and icon
  offsets to the sprite base and derives the packet base as sprite base
  minus 8, which typed indexing does not produce (lev 48 with a texture-page
  step back from a `RenderSpritePageSlot` view, 76 with paired array
  references, 188 to 237 for a single typed macro with `short` or `int`
  cursors).

## Link ownership

Splat's `undefined_syms_auto` and `undefined_funcs_auto` files are address/name
catalogues, not linker inputs. A linker-script assignment overrides a genuine
definition from an object and can make promoted C appear to link while its
symbol remains pinned. The build generates `undefined_addr_aliases.main.txt`
from the linked objects and emits only symbols that remain unresolved.

Tentative COMMON globals remain pinned until their BSS/SBSS placement is
represented by the manifest and linker script. This is explicit layout debt;
ordinary text/data definitions become object-owned automatically and cannot be
silently shadowed by the complete splat catalogues.

## Review contract

Each change must state which claim it makes: binary match, semantic recovery,
typing, naming, organization, or crutch removal. A green byte check proves the
binary claim only. It does not by itself prove the other claims.

## Shiftability

A goal of the reconstruction is that the code can change size without
breaking anything: adding, removing or growing a function must not invalidate
another address. A matching build proves the bytes; shiftability proves the
source does not depend on where those bytes sit.

Rules for source that aims at shiftability:

- Code and data refer to each other by symbol. A fixed address in C is allowed
  only for hardware (I/O registers, the scratchpad) and for the contract with
  the disc loader, such as the address an overlay is loaded at.
- Data that the program owns is defined in a translation unit and placed by
  the linker. Absolute symbol assignments in linker or symbol files are
  migration state, not a design.
- No constant that happens to equal a code or data address of this program:
  pointer-to-integer casts, integer-to-pointer casts of program addresses,
  address-named alias declarations and table entries typed as plain integers
  all count as hidden fixed pointers.
- Overlays link against the built main executable's symbols, not against
  addresses copied from a retail disassembly.
- Data produced outside the build (disc assets) that embeds program addresses
  is a documented ABI. Each such place is listed with its owner, and the
  addresses it needs stay pinned on purpose.

### Measuring it

`tools/scripts/shift_audit.py` (read-only, needs `make build` and, for the
overlay table, `make overlay-build-all`) prints the inventory below;
`--json` gives the same data and `--check BASELINE` fails when a count grows.
`tools/scripts/shift_test.py` is the acceptance test. It relinks main from
the built objects into a temporary directory, with `--emit-relocs`, once as
is and once with `--pad` bytes (default 0x10) inserted at the start of
`.main`, and compares every word of the two images. A relocated word that
moved by the pad is an explained address field. A relocated word that did not
move refers to an absolute linker pin, a gp-relative offset that changed
refers to a pinned small-data symbol, and an unrelocated word shaped like a
program address (data word, `lui` upper half, `j`/`jal` target) is a literal
address. The repository is never written; the base relink must equal
`build/USA/main.exe`.

### Inventory (2026-10-05, after removing the shadowing pins)

Linked absolute assignments for main, by what the address is:

| category | manual | generated aliases | remedy |
| --- | ---: | ---: | --- |
| unreferenced by any main object | 1546 | 0 | catalogue only; move to a symbol file or drop |
| bss inside a zero-filled asm blob (295 of them already labelled there) | 298 | 1 | own the variable in a C unit's `.bss`/`.sbss` |
| tentative COMMON pinned (no BSS placement yet) | 111 | 0 | place BSS/SBSS in the manifest |
| initialised data inside an asm blob, labelled there | 108 | 1 | name the blob label, or move the data into C |
| function address that a linked object already names | 48 | 9 | reference the defining name |
| data inside a C unit's section | 15 | 26 | point at the owning symbol plus offset, or split the definition |
| hardware (I/O registers) | 10 | 0 | unavoidable |
| kernel/BIOS RAM (0x80000000, 0xA000xxxx, 0xDFFC) | 4 | 1 | unavoidable (BIOS contract) |
| symbolic aliases (`a = b;`) | 5 | 0 | not fixed; rename callers eventually |

`undefined_syms_auto.main.txt` (517) and `undefined_funcs_auto.main.txt`
(empty) are splat catalogues and are not linked.

Fixed addresses in C (comments stripped): 446 alias declarations
(`asm("name")`) whose target is an absolute pin (350 in `src/main`, 49 in
`include`, 47 in overlays), 33 integer-to-pointer casts of literal program
addresses (18 main, 15 overlays) and 5 other literal program addresses. A
further 390 address-named aliases point at symbols an object defines and do
not pin anything. The crutch ratchet's `pointer_integer_casts` (547 main, 82
overlays) is a plain `(s32)`/`(u32)` cast regex; almost all of those are
integer casts and do not bind an address.

Built main executable: 7147 relocations resolve to 618 absolute
program-range symbols (5655 address words and 1491 gp-relative offsets in the
shift test), 21 data words and 17 `lui` halves hold a program address with no
relocation at all. The literals are LIBCD BIOS state pointers
(`g_CdBiosHardwareState`), a `0x800E0xxx` function table in
`dtail_gp_pre_s016`, two string-directory buffer pointers, the
`Task_InitGpuHwRegs`/`Task_GpuFlushPrimQueue` register-block pointers at
0x80070E0x, and `0x800Axxxx` upper halves in a few CD, memory card, menu
and GPU units. A few of the 21 data words (`0x80017FFF`) are probably two
halfwords rather than an address.

Overlays never see the built main executable. Each overlay links three
generated scripts (`undefined_funcs_auto`, `undefined_syms_auto` from splat,
`undefined_extra` from `overlay_extra_undefineds.py` using
`configs/USA/sym.main.txt`), all holding retail addresses. Across the 190
overlays 8073 referenced assignments point into main (399 distinct
addresses): 6086 already match a symbol of the same name and address in the
built main ELF, 1672 match an address main names differently and 314 have
no main symbol. Overlays also pin 7539 references to their own definitions
(the same shadowing as main's, through address-named `func_`/`D_` labels) and
988 own-window addresses no object defines; 518 point into overlay windows
used as runtime buffers. Since 2026-10-06 `overlay_extra_undefineds.py` reads
only the objects the overlay's linker script links and skips every name one
of them defines: `undefined_extra` dropped from 12915 entries (4622 hard
assignments) to 1369 (471). The two overlays with copied foreign-VMA code
(`OVERLAY_VMA_OVERLAP`, `boot_display` and `menu_memcard`) keep those pins
(`--pin-defined`). The splat `undefined_*_auto` scripts still pin.

Fixed layout: `.main` at 0x80010000 (the PS-X EXE load address, a loader
contract), `.field_engine` at 0x800C1CA0, `_gp = 0x8009CD70`, and the
header literals in `asm/USA/main/header.s` (initial PC 0x80072534, text size
0x1EE000, stack 0x801FFFF0). Main's BSS, heap and every overlay load window
live inside the zero-filled `dtail_*` data blobs, so the executable covers
0x80010000 to 0x801FE000 with no layout of its own for those regions.

### Shift test result

With the script unchanged a shift does not link: `.field_engine` at its fixed
VMA overlaps the grown `.main`. With `.field_engine` and `_gp` rebased in the
scratch copy, a 0x10 shift gives 20930 explained address fields, 5655 words
still pointing at pinned symbols (464 symbols; most used `g_CurrentEntity`,
`g_AkaoCmd`, `g_AkaoCurTrack`, `g_GameState`, `_spu_RXX`), 1491 gp-relative
offsets to pinned small data, 21 literal data words, 17 literal `lui`
halves, 28 hardware references and no unexplained difference. The header
fields do not follow the shift. Before the shadowing pins were removed the
same run reported 15105 pinned words and 1947 gp-relative offsets.

### Removal order

1. Done: 931 manual assignments that duplicated a definition at the same
   address in a linked object. Byte-identical; they only hid the object's
   ownership.
2. Unreferenced manual entries (1546): no object needs them; their names
   feed `gen_expected.py`'s label table, so move the useful names into
   `configs/USA/sym.main.txt` (or drop generic ones) and check the expected
   split. Mechanical, about one commit.
3. Overlays link against the built main symbols (`ld --just-symbols` on the
   main ELF, or generated assignments taken from `build/USA/main.map`), and
   `overlay_extra_undefineds.py` stops pinning names the overlay defines
   itself (done for `undefined_extra` outside the VMA-overlap overlays).
   Clears the 8073 main references and 7539 self-shadowing pins; needs a
   check for names that exist in both main and an overlay.
4. References that a linked object already names under another label (57
   function pins, about 400 data pins into asm blobs): give the blob label the
   C name through `sym.main.txt`, or change the C reference. Mechanical per
   symbol, byte-identical.
5. Alias declarations onto pins (446) and integer-to-pointer literals (33):
   replace by the owning symbol once steps 4 and 6 give it one.
6. BSS ownership: the zero blobs and the 111 COMMON pins become `.bss`/`.sbss`
   definitions in their units with a manifest placement. This is the large
   step (several hundred variables) and is what lets `_gp` and
   `.field_engine` be derived (`ADDR(.sdata) + 0x8000`, end of `.main`)
   instead of fixed.
7. Literal words and `lui` halves (38): rewrite the C as symbol references,
   relabel the asm data words.
8. Header: initial PC from the entry symbol, text size from the section end.

Unavoidable, by design: hardware registers and the scratchpad, the kernel and
BIOS addresses (LIBCARD patch, `0x80000000` vectors), the `.main` load address
and stack top, and the overlay load addresses (0x8018EFE8 for rooms and the
other windows): the disc loader and the data on the disc depend on them.
