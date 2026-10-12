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
  to the constant and builds it twice (lev 3-15).
- `Seq_TrackPitchSetup` (2 pins): retail computes the pitch delta in place in
  a2 (sll, subu, div, mflo), which gives it more weight than `track`; without
  the pins the subtraction goes through v0 and the registers swap (lev 7).
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
- `Asset_LoadTimTextures` (2 gotos, 2 volatile bank-pointer views): the
  state machine restarts its `switch` after states 0 and 0x35; a `for (;;)`
  loop around the switch lets loop.c move code out of the cases (lev 81).
  Retail stores each process bank pointer through its address held in a
  register and reloads it every iteration; plain stores use absolute
  addressing (lev 47), a `PmPrimarySlot **` local gives the register address
  but not retail's extra copy (lev 44).
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
  one on the stored slot (lev 14). `Render_LoadFontGlyphAlt` keeps its byte
  view of the table (typed lookup lev 10, shared slot search lev 24).
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
- `Aya_SetTotalExp` (1 barrier, local alias header): retail keeps the save
  base in s2 for the total and the cleared pool, and reads/writes the bonus
  pool through three absolute accesses that one symbol lets CSE merge
  (`Menu_LevelUpFlow_externs.h`).
- `Menu_MainUpdate` (3 pointer-integer casts, 3 field macros, local alias
  header): the m2c variables that carry the battle entity pointers also
  carry colours, and splitting them reallocates registers (lev 28-248); the
  halfword position reads must not be structure members (the scheduler
  would hoist the `D_8009D28C` store); `draw_slot_status0..2` keep retail's
  reloads of the draw slot (`Menu_MainUpdate_externs.h`).
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

What is left (best crutch-free lev against retail):

- `FieldEng_AngleMath.c` (5 pins): `FieldEng_TurnToward` lev 15 and
  `func_800E0024` lev 6. Retail keeps the step and the turn delta in separate
  registers (`move v1,a2` then `negu v1,a2`): with `delta = step` followed by
  `delta = -step` cse makes the longer-lived delta the class head and rewrites
  `-step` as `-delta`, so step dies at the copy and global alloc gives delta
  step's register (a s16 delta, if/else and ternary forms tried). The same
  copy preference ties `func_800E0024`'s distance to the difference in `a0`.
- `FieldAnim_TaskList.c` `func_800CE610` (2 pins, 2 barriers): lev 14 with a
  `for` loop over the parameter itself (`list = header->entries`), the stride
  read before the loop and `if (i >= header->count) return 0`. Retail has two
  more copies of the list pointer (`v0` for the header loads, `t0` after the
  loop) and a second register for the count inside the loop; cse merges both
  in every plain form.
- `FieldEng_CollisionQueries.c` `func_800C5EB0` (1 pin on the node pointer,
  one volatile read of `collision_step`): lev 8 without the pin. Global alloc
  ranks the loop index (9 refs over 45 insns) above the node pointer (11 over
  59); retail has node, index, strip in s1, s2, s3. Dropping the index's
  reuse as the return value lets the strip outrank the index (lev 13). The
  volatile read keeps `lhu` plus `sll/sra` for the `(s16)` test; a plain read
  is combined into `lh` (lev 4).
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
    two loop counters moves other registers (lev 26).
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
    It keeps the `disp_x` memory barrier: GCC 2.7 lets the scalar reload of
    the volatile `D_800B1624` pointer pass the `disp_x` field store, which
    retail keeps in order (lev 7 without it; the pointer's declaration
    belongs to the render headers). The byte-storage store to `u34` stays
    for the same reason (12 instructions differ).
    `Geo_TransformPoint` keeps its three pins and the barrier on the x sum.
    Without the pins, local allocation swaps `$t0`/`$a0` for the entry and
    y. Without the barrier, the unclamped x copy merges with the sum. The
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
    parameter changes nothing.
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
    panel color bytes (`D_800BE9F0[slot].color`).
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
  - `Battle_DrawStatusPanel` (typed digit packets lev 294),
    `Battle_DrawHPBar` and the four phase steps of
    `Battle_EscapeAndPhaseInitFlow` (70 pins) were not reduced further.
  - Second half of the directory (`Battle_HaltOnPositiveX` to
    `Math_IntSqrt`), measured again after the recipes above:
    - `Battle_StepEnemyMovement` (1 pin): in global allocation the
      resistance mode (7 refs over 137 insns) outranks `enemy` (9 refs
      over 285 insns), so they swap `s4`/`s5` (lev 22). Naming `enemy`
      instead of its copy in one more read makes it win (lev 1, but that
      load then uses `s4` where retail uses the copy in `s2`); moving the
      mode's initialisation or retyping it does not help (lev 18 to 71).
    - Constant hoisting (`Battle_Update` gauge recolor: 6 pins and 7
      barriers; `Battle_StepVictory` stat reset: 5 pins;
      `Battle_StepPostBattle`: 13 pins and 4 barriers, most on the panel
      quads, measured one at a time lev 2 to 38): retail loads
      every color or edge constant at the top of its block and leaves the
      later load-delay slots empty. Without the pins sched1 places each
      `li` in the first load stall after its use, whether the constant is
      a block local, a function-scope variable set in two cases (no longer
      LAUNCH priority, still a stall filler) or a `u8` (lev 75, 60 and 103
      for the three blocks). The `asm("" : : "i"(n))` barriers in
      `Battle_Update` also keep cross-jumping to the last store only;
      without them blocks 1 and 3 merge (lev 299).
    - `Battle_StepVictory` (also `result`, `object`, `brightness`, `delay`
      pins): retail jumps from case 1 to the byte truncation in case 5
      before register allocation, so the brightness temporaries share
      `s0`; written as a goto or as two tails the temporaries take `v0`
      (lev 25). The `delay` pin only moves the `249` constant (lev 2).
    - `Battle_StepAyaAction` (9 pins, 7 barriers): case 9 written with a
      word pointer and named inserts reaches lev 16 (the outcome stores to
      `g_EvasionStatus` and `g_EvasionReaction` sink below the actor loads,
      which do not alias them); with the record views the store and load of the
      reaction share one address register under cc1 `-G0` (lev 57). The
      pose save/restore cases keep `$2`/`$3`/`$4`/`$5` transfer pins
      (lev 2 to 55 each, measured one at a time).
    - `Battle_TargetAndEnemyTurnFlow` (2 pins, 2 barriers): the first
      target-confirm sound loads the AKAO table before the `128` argument
      and the cancel path keeps `mode = 0` in the call delay slot; every
      plain form (inline sound helper, local table, reordered statements)
      gives lev 11 to 16.
    - Stack reserves: `Battle_Update` keeps 0x1A0 and
      `Battle_StepPostBattle` 24 unused frame bytes; no local of the
      function accounts for them.

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
  `Battle_AdvancePhase`, the forced-G0 task units). Views that only separate
  two reads of one word for CSE are a different crutch and stay
  (`Battle_ApplySpellEffect`, `Battle_DrawSpellName`, `Inv_TransferItem`,
  `Inv_ItemActions`).
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
- task: `Task_GpuFlushPrimQueue` and `Task_InitGpuHwRegs` keep their
  register-block pins and barriers (lev 2 to 9 each; retail hand-allocates
  `$t0`..`$t6` for the GPU register words). `Camera_StepAngleFade` (m2c
  pins, lev 4 to 57), `Task_SpawnChildNode` (lev 2 to 12),
  `Task_SetObjEntryFlag80`/`Task_ClearObjEntryFlag80` (index/base `$v0`/`$v1`
  swap, lev 4 to 15), `Task_WaitAnimRange` (lev 28),
  `Task_TurnTowardPointStep` (lev 3, 14), `Task_SnapPosToSpawnPos` (lev 12),
  `Task_SetGteMatrix` (lev 21), `Task_OpenMemCardDialog` (`$v0` clobber,
  lev 13), `Task_RunInvCommand` (lev 16), `Boot_MemCardStepInit` (lev 15).
  `Task_DispatchScriptCmd` keeps its `goto`s: the structured switch puts the
  case-table pivot elsewhere (lev 80).
- gpu: `Gte_SetBackColor`/`Gte_SetLightColor` keep `t4`..`t6`: these are
  the registers the PSY-Q GTE macros name (lev 2 to 10). `Draw_AllocColorQuad`,
  `Draw_AllocSprite`, `Draw_EmitGlyph`, `Draw_AllocTexturedQuad`,
  `Draw_MeasureTextWidth`, `Draw_PrintCenteredTextInWidth`,
  `Draw_AllocColorRect`, `Draw_AllocColorTri`, `Gpu_DrawStatusIcons` keep
  m2c register pins (lev 1 to 78). `Draw_PrintTimeValue` keeps two memory
  barriers (lev 52). `Gpu_InitDisplay` keeps the same pins as
  `Boot_InitMemoryLayout` (lev 10 to 46).
- item: `Inv_SetActiveList` (lev 9 to 36), `Inv_GetSlotItemData`/
  `Inv_GetActiveSlotCount`/`Inv_IsSlotEquipped`/`Inv_IsSlotSelectable` empty
  asm statements (lev 18 to 61: they stop the shared slot lookup being
  cross-jumped), `Inv_ClearDisplaySlots` (lev 12),
  `Inv_CompareItemsForSort` (two memory barriers, lev 22 and 4),
  `Inv_CheckItemEquippable` (lev 5 to 40), `Inv_FindFirstEmptySlot`
  (lev 16, 49), `Inv_FindItem` (lev 1 to 21), `Inv_InitMaxLevelInventory`
  (lev 9), `Inv_RebuildSelectionBitset` (lev 17 to 36), `Inv_TransferItem`
  (lev 4 to 8), `Stat_QueryLevelAndSubLevel` (lev 65). The six local item
  record lookups became `Inv_LookupItemRecord`/`Inv_LookupItemRecordScaled`
  in `pe1/inventory_slots.h`.
- boot: `Field_HandleStateTransition` (lev 13 to 48), `Gpu_InitPipeline`
  (pins lev 11 to 13, barrier lev 37: the constant clamps are not folded
  without it). `Boot_DiscTableLoad` keeps its retry `goto`s (loop.c).
- main: `Battle_ApplySpellEffect` (lev 3 to 48), `Battle_BuildTargetList`
  `finalCount` pin and barrier (lev 5), `Battle_DrawStatusOverlay` (lev 2
  to 7), `Battle_StepCharacterAction` (lev 2 to 47),
  `Battle_UpdatePlayerTurn`/`Battle_ApplyPlayerHit` (lev 11 to 20),
  `Battle_DrawTargetHighlight` (lev 2 to 24), `Battle_DrawSpellName` (lev 2
  to 15). `Battle_PhaseAndEnemyEffectFlow` keeps its `(void *)D_8009D254`
  casts: removing them changes the bytes.

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
- `Battle_DrawHPBar`: the six `goto` loops are loops GCC's loop optimizer
  must not see; as `do`/`while` loops it hoists constants and spills
  `hudPage` (lev 98). Its pins, barriers and casts stay with that structure.
- `Battle_ActionSlotAndEnemy.c`: `room` (`$4`, lev 27: one delay slot,
  the rest is branch offsets), the `actor` barrier (lev 129: global
  allocation swaps s0/s1), the `stepDeathAnimation` goto (lev 184 for an
  inline search helper).
- `Battle_AdvanceTurnSlot`: the `savedIndex` barrier (lev 26: CSE reuses the
  slot index minus one from the command lookup).

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
