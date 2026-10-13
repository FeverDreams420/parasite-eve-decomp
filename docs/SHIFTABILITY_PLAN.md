# Shiftability plan, phase 3: main's data

Goal: no fixed program addresses in the main executable, so that code and
data can grow, shrink or move without breaking anything, while the retail
layout keeps building byte-identical. Phases 1 and 2 (code addresses,
shadowing pins, overlay `undefined_extra`) are described in
[CODE_ORGANIZATION.md](CODE_ORGANIZATION.md#shiftability). This document is
the analysis and the work plan for phase 3: main's data.

Measured on 2026-10-13 with `make build shift-audit shift-test` (and the
`--json` outputs), after the pilot and layout commits described in sections 3
and 7.

## Key conclusions

1. **Retail's data layout is the PsyQ link model, and it is recoverable from
   evidence.** The startup (`__SN_ENTRY_POINT`) clears BSS from `0x8009CDF8`
   to `0x800C20C8` and sets `$gp` to `0x8009CD70`, the first word of `.sdata`.
   The referencing units of the labels in between follow link order in
   runs: `.sdata` by object, `.sbss` by object, a small COMMON pool, `.bss`
   by object, a large COMMON pool. That tells, for every variable, which unit
   owned it and whether it was file-static (object runs) or a tentative
   global (pools). Section 1.
2. **Most pins can become linker-placed without touching code generation.**
   Every referenced data pin except the PsyQ CD state block (and one word in
   `field/actor_collision`'s `.data`) points into an asm blob. Naming the address in `configs/USA/sym.main.txt` turns the pin into
   a blob label, which the linker moves with its blob (stage A). Owning the
   variable in a C unit (stage B) is a second, per-unit step.
3. **`$gp` and the field engine no longer have fixed addresses** (done in this
   branch): `_gp = _ramsize` and `.field_engine` follows `main_VRAM_END`. A
   padded relink now links with no fixed placement left. The gp window is
   positive-only and the whole small-data block is 0x59C bytes, so it has
   32 KiB of room; an overflow fails the link (`R_MIPS_GPREL16`), it cannot
   silently mislink.
4. **Main cannot grow its globals today**: a tentative definition that no pin
   or label resolves is discarded with `.scommon`/`COMMON` and the link fails.
   The manifest needs a COMMON catch-all and in-image BSS before new data can
   be added. Section 3.5.
5. **The disc embeds no main data address.** A scan of `PE.IMG` for 32-bit
   words equal to a main symbol address finds 5 isolated (random) hits and
   none inside the 190 overlay slices. What is fixed by the disc is the load
   address, the overlay windows and the overlays' own code, which is rebuilt
   from source but shipped prelinked in `PE.IMG`. Section 6.
6. **Pilot** (`script/sfx_queue`, 3 pins): done end to end, byte-identical,
   all checks green. Pinned address words 6498 to 6443, gp-relative words to
   pinned symbols 1265 to 1243, no fixed placement left in the linker script.

## 1. Retail data layout

Section `.main` runs from 0x80010000 to the end of main's BSS; the field
engine group follows; the image (text size 0x1EE000) then covers the heap and
the overlay windows up to 0x801FE000.

| range | group | what decides the owner | evidence |
| --- | --- | --- | --- |
| 0x80010000..0x8001220C | `.rodata`, by object | link order | manifest, already linker-placed |
| 0x8001220C..0x800910A0 | `.text`, by object | link order | all C, linker-placed |
| 0x800910A0..0x8009CD70 | `.data`, by object (game, then PsyQ) | link order; several tables are shared | blobs `dtail_gp_pre_s016`, `_post_sprintf`, `_post_bios`, `_post_dsfile` and C `.data` |
| 0x8009CD70..0x8009CDF8 | `.sdata`, by object | link order (`_ramsize`, `_stacksize` first) | `_gp`; startup reads `_ramsize`/`_stacksize`; mostly initialised words |
| 0x8009CDF8..0x8009D1A0 | `.sbss`, by object | link order, monotonic from `node_pool` (1) to `widget` (63) | startup BSS clear starts here (`D_8009CDF8`) |
| 0x8009D1A0..0x8009D30C | small COMMON pool | none: order is not link order | users jump back and forth between unit 0 and unit 322 |
| 0x8009D30C..~0x800A5C00 | `.bss`, by object | link order, monotonic from `node_pool` (1) to `waitrc2` (291) | e.g. `g_FieldSfxQueue` sits between `widget` and LIBGPU state |
| ~0x800A5C00..0x800C20C8 | large COMMON pool | none | `g_GameState`, `g_AkaoCmd`, `g_SaveRuntimeState`, PsyQ `StRingSize` |
| 0x800C20C8 | `_end` | startup BSS clear stops here and `InitHeap(_end + 4, ...)` | `__SN_ENTRY_POINT` |
| 0x800C20C8..0x800E21A4 | field engine rodata, text, data | its own link order | engine strings start exactly at `_end` |
| 0x800E21A4..~0x800F3500 | field engine BSS (referenced by main) | | `fieldfx/burst` statics, `D_800F3428` ... |
| ~0x800F3500..0x8010BCF8 | zero, no label referenced by main | unknown: possibly runtime buffers | needs a RAM trace before it is used as growth room |
| 0x8010BCF8..0x801FE000 | overlay windows and buffers at fixed addresses | disc ABI | section 6 |

Two corrections to the current manifest follow from this:

- The `field_engine` segment starts at 0x800C1CA0, but 0x800C1CA0..0x800C20C8
  (0x428 zero bytes: `g_WayneStorageItems`, `g_InvItemArray`,
  `g_CursorRenderDataBlock`, `StRingSize`) is the tail of main's large COMMON
  pool. The segment boundary should move to file offset 0xB28C8 (vram
  0x800C20C8) and those bytes become a main data blob. Byte-identical; it
  makes `main_VRAM_END` equal retail's `_end`.
- The object runs are where "file-static" globals live. A variable in an
  object run that several of our units reference is evidence that those units
  were one retail object (or that the variable was a defined global); the
  object-run order is a TU-boundary oracle in the same way text order is.

The run boundaries above come from the referencing units of every label
(scratch script over the relocations of a `--emit-relocs` relink). The end of
the `.bss` object run lies between 0x800A5B70 and 0x800A5D50; the first agent
working that region pins it down.

## 2. Taxonomy of pinned main symbols

`relocations resolving to absolute symbols` (shift audit (c)): 7686 address
relocations to 499 distinct program-range symbols, plus 28 hardware/kernel.
The shift test sees 6443 pinned address words and 1243 gp-relative offsets to
pinned symbols. By retail group and by what the pin is today:

| group | symbols | address words | gp-relative | pin kinds today |
| --- | ---: | ---: | ---: | --- |
| function | 18 | 21 | 0 | 18 function named elsewhere |
| rodata | 2 | 8 | 0 | `jtbl_80010000` (constructor table base in `__main`), 1 blob |
| `.data` | 136 | 1518 | 0 | 94 in asm blobs, 40 inside `psyq/libcd/bios_4.c`'s data, 2 shadowing |
| `.sdata` | 6 | 204 | 11 | 3 COMMON pins, 3 blob |
| `.sbss` object run | 102 | 40 | 928 | 99 COMMON pins, 3 blob |
| small COMMON pool | 67 | 1566 | 304 | 34 COMMON pins, 32 blob, 1 C unit |
| `.bss` object run | 60 | 1016 | 0 | 58 blob (named elsewhere), 2 unlabelled |
| large COMMON pool | 104 | 1984 | 0 | 101 blob, 3 inside the field engine blob |
| field engine | 4 | 86 | 0 | 3 unlabelled (`fieldfx/burst`), 1 blob |

"Blob" means the address lies inside a splat asm data blob that already has a
label there (usually `D_xxxxxxxx`, 41 times a descriptive name that differs
from the pinned name); "COMMON pin" means a C unit holds a tentative
definition and the linker script pins it. 26 addresses carry more than one
pinned name (struct base and first field, or deliberate record views such as
`g_ActiveActor`/`g_ActiveActorRecord`).

What each kind needs:

| kind | count | stage A (shiftable) | stage B (owned) |
| --- | ---: | --- | --- |
| function named elsewhere | 18 | name the function in `sym.main.txt` (the blob word then says `Script_GetCurrentEntityFieldD`), or rename the C reference; drop the pin | done by stage A |
| shadowing pin (`g_RoomFloorY`, `g_DsReadStatusBlock`) | 2 | delete the pin, the object already defines the name at the same address | |
| initialised `.data` in an asm blob | 94 | `sym.main.txt` name for the blob label, drop the pin | carve the blob at the unit's `.data` boundaries, define the table in C (`.data` subsegment); needs the table's type, risk is in initialisers and alignment |
| data inside a C unit (`bios_4.c` CD state block) | 40 | relative linker alias `D_8009AFB8 = g_CdBiosState + 4;` (moves with the owner), or a member access where codegen does not change | define the aliases in `bios_4.c`, or members |
| `.sdata` | 6 | blob label | `.sdata` subsegment of the owner; needs the unit to compile with cc1 `-G8` (see the pilot: a forced-G0 unit emits the definition into `.data`) |
| `.sbss` object run | 102 | blob label; a COMMON then resolves to it (as `g_InvActiveListItems` already does) | owner's `.sbss` subsegment; needs MASPSX to emit a global `.sbss` label for a `.comm` (today it emits a local one, which is why units use `--use-comm-section`) |
| small COMMON pool | 67 | blob label | one pool unit defining the variables in retail order, or keep the labelled blob (see 3.5) |
| `.bss` object run | 60 | blob label | file-static in the owner, `.bss` subsegment with `linker_section_order: .data` (the pilot) |
| large COMMON pool | 104 | blob label | as the small pool |
| field engine BSS | 4 | blob label (`sym.main.txt`; three have no label at all yet) | engine units' `.bss` |
| hardware I/O, scratchpad | 10 + 6 | unavoidable | |
| kernel/BIOS RAM (`0x80000000` vectors, `0xA000xxxx`, `0xDFFC`) | 5 | unavoidable (BIOS contract) | |
| unreferenced assignments | 1549 | no object uses them, they do not affect the shift; move useful names to a symbol catalogue (many are overlay-window and cheat names, e.g. `g_HSFuse2Tx`) | |

Not pins but still fixed, from the shift test:

- 15 literal data words: two overlay-window pointers in `rodata_001E0C`
  (`0x8018EFF0` string directory buffer, `0x801ED7F8` task program buffer; ABI
  values, should be named ABI constants), the LIBCD hardware state block in
  `bios_4.c` (6 words pointing into itself and at `g_CdLastCom`), and 7 words
  in the `dtail` blobs (`0x80010EA0`, `0x80010F58`, `0x8001170C`,
  `0x800117AC`, `0x80010804` are rodata pointers splat did not relocate,
  `0x80017FFF` twice is probably two halfwords). Fix: relabel the blob words
  as symbol references (`rel.main.txt` or a symbol at the target).
- 8 literal `lui` halves: `Draw_InitGpuHwRegs` and `Draw_FlushPrimQueue`
  (the register block at 0x80070Dxx, 6), `Render_LoadFontGlyphAlt` (1),
  `DsReadCallback` (1). Fix in C: refer to the symbol.
- 13 integer-to-pointer casts of program addresses in `src/main` (`bios_4.c`
  6, `gpu_regs.c` 3, `prim_queue.c` 3, `dsread_2.c` 1), the same items seen
  from the source side.
- Header literals: initial PC `0x80072534` and text size `0x1EE000`.

## 3. Mechanisms

### 3.1 Stage A: a pin becomes a blob label

A splat data blob is one input section. The linker places it after its
predecessor, so every label inside it moves with it. Naming the address in
`configs/USA/sym.main.txt` makes splat emit `dlabel <name>` in the blob at
the next `make split`; references then relocate against that label and the
absolute assignment is deleted. This is already the project's practice
(`e5291685f`, `57b48b333`). A tentative COMMON definition in a C unit
resolves to the blob's definition; ld prints an alignment note when the
COMMON wants more alignment than the label declares, as it already does for
`g_InvActiveListItems` and nine others.

What stage A does not give: the blob's internal layout is still fixed, so
data cannot be added inside a blob, and the owner is still asm. It is enough
for code to grow and for whole data regions to move.

One address, one name. Renaming a blob label `D_8009D20C` to
`g_FieldActorListHead` removes the old name, and any C that still says
`D_8009D20C` is then silently pinned again by
`gen_undefined_addr_aliases.py` (it pins address-form names). The recipe
checks for that. When two names are deliberate (record views used to steer
GCC), renaming the C may change code; then the second name becomes a relative
alias in the linker symbol file (`g_ActiveActorRecord = g_ActiveActor;`),
which moves with its target and is not a pin. The audit counts these as
`symbolic_alias`; they are allowed migration state, not a goal.

### 3.2 Stage B: a C unit owns the variable

The unit defines the variable and the manifest places the unit's section at
the retail position inside the data stream:

```yaml
      - [0x8DA58, data, main/dtail_gp_post_collision_actor]
      - {start: 0x93980, type: .bss, name: script/sfx_queue, linker_section_order: .data}
      - [0x93AD0, data, main/dtail_gp_post_sfx_queue]
```

`linker_section_order: .data` keeps the unit's `.bss` in the `.data` list, in
the PROGBITS `.main` output section, so ld zero-fills it in the file exactly
where retail has zeros. `gen_expected.py` counts such in-image `.bss` rows
toward coverage (`inline_bss_bytes`, added with the pilot).

Which form of definition reproduces the retail section:

| retail group | C form | MASPSX/cc1 path |
| --- | --- | --- |
| `.bss` object run | `static T x[N];` | cc1 `.lcomm`, MASPSX emits a `.bss` label (global) in declaration order |
| `.sbss` object run | `static T x;` in a `-G8` unit | cc1 `.lcomm`, MASPSX `.sbss` label, local; a global needs the MASPSX change in section 8 |
| `.data` object run | `T x[] = {...};` | `.data` |
| `.sdata` object run | `T x = v;` with cc1 `-G8` | cc1 emits `.sdata`; under `MASPSX_FORCE_G0` cc1 runs at `-G0` and emits `.data` instead |
| COMMON pools | not reproducible by section order | keep the labelled blob, or one pool unit (section 3.5) |

A unit owning several variables in one group must declare them in retail
address order and match retail's padding (`SUBALIGN(4)` aligns each input
section to 4; MASPSX aligns `.sbss` items by size).

### 3.3 `$gp` (done)

`_gp` is set by splat's `ld_gp_expression: _ramsize`. Retail's `$gp` is the
start of the `.sdata` group and its first word is the startup's `_ramsize`
(0x00200000), followed by `_stacksize` (0x2000); both are now named and
`snmain.c` reads them by name. `gp_value` stays for the retail disassembly.

All gp-relative targets lie in 0x8009CD70..0x8009D30C, positive offsets only
(0x0..0x59C). When data before `.sdata` grows, `_gp` and its targets move
together and every `%gp_rel` offset is unchanged (the shift test reports
2314 unchanged gp-relative words). When small data is added inside the block,
the offsets after it change and are relocated. The window has 32 KiB of
positive room (64 KiB if `_gp` were ever re-centred, which would change every
offset and is not needed). Out-of-range offsets are a link error. The 1243
gp-relative words that still go to pinned symbols disappear with stage A of
the `.sbss`, small-pool and `.sdata` symbols; until then a shift that moves
`_gp` breaks exactly those words, which the shift test lists.

### 3.4 `.field_engine` (done) and the main/engine boundary

The `field_engine` segment has `vram_symbol: main_VRAM_END`, so the linker
script places it after main. Next step: move its start to 0x800C20C8
(section 1), so that `main_VRAM_END` is retail's `_end`.

The field engine is linked in the same program (main calls `FieldFx_*`
directly and the engine reads main's data), so it shifts with main. Its BSS
and whatever lies above it are in the `dtail_0D0860` blob up to 0x801FE000,
which also holds the overlay windows. That blob must be split into
engine data/BSS (moves), a free gap, and the overlay window block anchored at
its ABI address (`. = 0x8010BCF8` after an `ASSERT(. <= 0x8010BCF8, ...)`).
The gap between the last main-referenced engine label (~0x800F3500) and
0x8010BCF8 is about 98 KiB of zeros, but before it is used as growth room a
RAM trace has to show that nothing uses it at run time.

### 3.5 COMMON, BSS in the image, and growth

Today every tentative definition is resolved by a pin or a blob label. A new
one is not: ld discards `.scommon`/`COMMON` through `/DISCARD/ : { *(*) }`
and the link fails (`g_MenuWidgetSavedNode ... defined in discarded section
.scommon`, tried in a scratch relink). For growth, the manifest needs:

- a catch-all `*(.scommon) *(COMMON)` after the last main data blob (empty
  for the retail layout, so byte-identical);
- BSS inside the file image. Retail's BSS is zero bytes inside the PS-X EXE.
  If `.main_bss` (NOLOAD) ever becomes non-empty, `__romPos` does not advance
  and the field engine's LMA and VMA diverge. Either keep every BSS input
  in `.main` (as `linker_section_order: .data` does) or advance `__romPos` by
  `SIZEOF(.main_bss)`. Splat 0.50's `ld_bss_contains_common` only writes
  `COMMON` for NOLOAD BSS and its `ld_bss_is_noload: False` drops it, so this
  needs either a splat option or a small checked post-pass over `main.ld`.

The COMMON pools are not in link order, and GNU ld allocates COMMON in hash
order, so a pool cannot be reproduced by placing each unit's `COMMON`. Two
ways to own a pool: keep it as a labelled blob (shiftable as a block; new
globals go to the catch-all), or one pool unit that defines the pool's
variables as real definitions in retail order, with the other units keeping
their tentative definitions. The second needs the MASPSX global-`.sbss`
behaviour for the small pool and is a later decision (section 8).

### 3.6 Startup and header symbols

`snmain.c` (asm kept in the PsyQ macro) refers to the BSS bounds through blob
labels (`D_8009CDF8`, `D_800C20C8`) and to the heap words `D_8009453C`,
`D_80094540`. They move with their blobs today. Once `.sbss` is owned and the
engine boundary moves, they become linker symbols: BSS start = first `.sbss`
input of main, `_end` = `main_VRAM_END`. The PS-X EXE header's initial PC
should be `__SN_ENTRY_POINT`; the text size can stay 0x1EE000 while the image
end is anchored at 0x801FE000 by the overlay-window block.

### 3.7 Overlays

Overlays link against retail addresses (`undefined_*_auto` and
`undefined_extra` from splat and `sym.main.txt`): 389 distinct main
addresses. A shifted main therefore needs the step in the existing removal
order: link each overlay against the built main ELF (`--just-symbols` or
assignments generated from `build/USA/main.map`), and repack the rebuilt
overlays into `PE.IMG` (same sizes, patched in place at their sectors; the
ISO target currently reuses the retail `PE.IMG`). Until then a shifted main
is shiftable source but not a playable disc.

## 4. Byte identity and the recipe

Every step keeps the retail layout byte-identical: the manifest reproduces
retail addresses, the source no longer states them. A step is accepted only
if all of `make check`, `make overlay-build-all overlay-check-all`,
`make organization-check source-policy-check debt-check` and `make report`
pass, no new pin, barrier, `volatile`, asm or `goto` appears, and the
Levenshtein distance of every touched function stays 0. Run `make split`
after any symbol-file or manifest change.

### Recipe: one symbol, end to end

Worked on `g_FieldSfxQueue` (bss object run), `g_FieldSfxQueueCount`
(`.sdata`) and `g_FieldActorListHead` (small COMMON pool).

1. Find the symbol and its group: `shift_test.py -v 400` lists pinned words
   per object and symbol; the address gives the group (section 1). Look up
   the label splat already has there (`build/USA/main.map`) and every C name
   used for the address (`grep -rn D_8009D20C src/main include`).
2. Stage A. Append `name = 0xADDR; // size:0xN` to `configs/USA/sym.main.txt`
   and delete the line from `linkers/USA/undefined_syms_manual.txt`.
3. Unify names. Replace the other spellings in `src/main` and the headers
   main includes (here `D_8009D20C` in 8 units and `battle_runtime.h`).
   Overlay sources may keep address-form names; they resolve by address.
4. `make split && make build && make check`. Then check that
   `linkers/USA/undefined_addr_aliases.main.txt` did not gain the old name; if
   it did, a reference was missed (the first try of the pilot did exactly
   that: 18 more pinned words until the battle units were renamed).
5. If a rename changes code (lev > 0), revert it and add a relative alias
   `old = name;` to `undefined_syms_manual.txt` instead.
6. Stage B, when the group allows it (object runs only). Define the variable
   in the owner (`static FieldSfxQueueEntry g_FieldSfxQueue[28];`), drop the
   header `extern` if nothing else uses it, and split the blob in
   `configs/USA/main.yaml` at the variable's file offsets
   (`vram - 0x80010000 + 0x800`) with a `.bss`/`.sbss`/`.data` row for the
   unit (`linker_section_order: .data`) and a new blob row after it. Check
   that the unit's section has exactly the gap's size (`objdump -h`).
7. `make split build check`, all overlays, the checks and `make report`;
   `make shift-test` shows the symbol gone from the pinned lists.

What the pilot learned: `g_FieldSfxQueueCount` could not be owned. Its unit
runs cc1 at `-G0` (`MASPSX_FORCE_G0`), so `= 0` puts it in `.data` at the
unit's `.data` position and everything after it moved by 4 bytes. It stays a
blob label until the unit runs cc1 with `-G8` or the forced-G0 path learns
`.sdata`.

## 5. Work list

### Order

Stage A is independent of code generation and is done by region packages
whose symbol sets are disjoint, so agents can run in parallel; each package
owns its symbols' lines in `sym.main.txt` (append in one block per package to
keep rebases trivial) and every C rename those symbols need.

| wave | package | symbols | words + gp | risk | notes |
| --- | --- | ---: | ---: | --- | --- |
| 0 | layout: engine boundary to 0x800C20C8, COMMON catch-all, `_end`/BSS-start symbols | 1 | 4 | low | done: engine at 0x800C20C8; `tools/scripts/ld_postpass.py` (run by `make split`) puts main's BSS in `.main`, adds `*(.sdata)` at `__bss_start`, `*(.sbss) *(.scommon)` at the small pool end 0x8009D310 (the gp window: new small data at the end of main overflows `R_MIPS_GPREL16`) and `*(COMMON)` before `_end`; `snmain` uses `__bss_start`/`_end`; a new global and function link and main grows by their size (`test_ld_postpass.py`) |
| 1 | functions named elsewhere, shadowing pins, field engine labels | 24 | 131 | low | names only |
| 2 | `.bss` object run | 60 | 1016 | low | stage A done (2a): 58 blob labels in `sym.main.txt`, 18 address-form C names unified, no alias; pinned words 6253 to 5237; stage B open |
| 2 | `.sbss` object run + `.sdata` | 108 | 1183 | medium | 99 COMMON pins; names used by many menu units. Stage A done: 107 pins removed (blob labels; 5 second views are relative aliases); left `g_ScriptCameraBytes` (label shared with the BSS start in `snmain`, wave 0) |
| 2 | large COMMON pool | 104 | 1984 | medium | `g_GameState`, AKAO state; record-view aliases |
| 3 | small COMMON pool | 67 | 1870 | high | `g_CurrentEntity`, `g_PlayerEntity`, `g_ActiveActor`: most aliases and record views, most units |
| 3 | `.data` and `.rodata` in blobs (game, PsyQ SPU/GPU/pad/DS) | 96 | 1114 | medium | descriptive blob names differ from pinned names in 41 places |
| 3 | `psyq/libcd` state block in `bios_4.c` | 40 | 388 | high | struct base vs field names across 12 LIBCD units; relative aliases first |
| 4 | literals: 15 data words, 8 `lui`, 13 casts, header PC | | 23 | medium | |
| 5 | stage B by unit, object runs first (`.bss`, then `.data`, then `.sbss` once MASPSX allows) | | | per unit | uses the per-module table below |
| 6 | overlays link against built main, `PE.IMG` repack | | | high | needed before a shifted build can boot |

Wave 1 status: done, byte-identical; 18 function pins, `g_DsReadStatusBlock`, 3 `fieldfx/burst` labels and 4 `FieldFx_*` symbolic aliases gone (pinned words 6253 to 6140); `D_800C20C8` (`_end`) is left to wave 0, `func_800119CC` (rodata) to the data package.

Within a package, work from the least shared symbols to the most shared. A
symbol referenced by several modules belongs to the package of its group,
not to a module; the module table below is the order for stage B and for
assigning agents, and a module's stage A is complete when its package is.

### Per module

Pinned symbols referenced (own = referenced by no other main unit), words
(address and gp-relative relocations to pinned symbols), the groups they lie
in, the most used, and the risk (high: many shared or aliased names, or more
than 300 words; aliased = addresses with more than one name). Ordered low to
high risk, then by size. `script/sfx_queue` (pilot) is done.

| # | module | pins | words | groups | most used | risk |
| --- | --- | --- | ---: | --- | --- | --- |
| 1 | `asm:dtail_gp_post_dsfile` | 1 (1 own) | 1 | func 1 | `func_80090F68` | low (1 aliased) |
| 2 | `psyq/libgpu/sys_3` | 1 (1 own) | 2 | data 1 | `g_GpuActiveDispEnv` | low |
| 3 | `psyq/libcd/c_003` | 1 (0 own) | 2 | data 1 | `D_8009AFD8` | low |
| 4 | `psyq/libspu/s_snc` | 1 (0 own) | 2 | data 1 | `_spu_RXX` | low |
| 5 | `psyq/libspu/s_gvex` | 1 (0 own) | 2 | data 1 | `_spu_RXX` | low |
| 6 | `psyq/libspu/s_srmd` | 1 (0 own) | 2 | data 1 | `_spu_RXX` | low |
| 7 | `psyq/libspu/s_r` | 1 (0 own) | 2 | data 1 | `_spu_transferCallback` | low |
| 8 | `psyq/libspu/s_w` | 1 (0 own) | 2 | data 1 | `_spu_transferCallback` | low |
| 9 | `draw/color_quad` | 1 (0 own) | 2 | sbss 1 | `D_8009D10C` | low (1 aliased) |
| 10 | `psyq/libcd/c_005` | 1 (0 own) | 2 | common 1 | `g_StrDataReadyCallback` | low |
| 11 | `psyq/libcd/c_008` | 1 (0 own) | 2 | common 1 | `StRingAddr` | low (1 aliased) |
| 12 | `menu/item_detail` | 1 (0 own) | 3 | sbss 1 | `g_MenuItemPrimListHandles` | low |
| 13 | `psyq/libds/dssys_3` | 1 (0 own) | 4 | data 1 | `CD_debug` | low (1 aliased) |
| 14 | `psyq/libspu/s_svva` | 1 (0 own) | 4 | data 1 | `_spu_RXX` | low |
| 15 | `psyq/libspu/s_stc` | 1 (0 own) | 4 | data 1 | `_spu_transferCallback` | low |
| 16 | `psyq/libspu/s_sic` | 1 (0 own) | 4 | data 1 | `_spu_IRQCallback` | low |
| 17 | `psyq/libds/dsready` | 1 (0 own) | 4 | data 1 | `g_DsReadBusy` | low |
| 18 | `battle/enemy_slots` | 1 (0 own) | 4 | scommon 1 | `D_8009D2A0` | low |
| 19 | `psyq/libds/dsread2` | 1 (1 own) | 4 | common 1 | `g_DsStreamNoLocFlag` | low |
| 20 | `psyq/libcd/c_011` | 1 (0 own) | 4 | common 1 | `g_CdStreamEndState` | low |
| 21 | `psyq/libetc/vmode` | 1 (1 own) | 6 | data 1 | `g_VideoMode` | low |
| 22 | `battle/player_turn` | 1 (0 own) | 7 | scommon 1 | `D_8009D1D4` | low (1 aliased) |
| 23 | `psyq/libspu/s_si` | 1 (0 own) | 8 | data 1 | `_spu_RXX` | low |
| 24 | `psyq/libds/dsread_2` | 1 (1 own) | 8 | data 1 | `g_CdReadCompleteCallback` | low |
| 25 | `psyq/libetc/vsync` | 1 (0 own) | 10 | data 1 | `g_VSyncCount` | low |
| 26 | `psyq/libapi/pad` | 1 (1 own) | 10 | data 1 | `g_InitPadFlag` | low |
| 27 | `psyq/libds/dstype` | 1 (1 own) | 10 | common 1 | `g_DsDiskType` | low |
| 28 | `psyq/libds/dsfile` | 1 (0 own) | 12 | data 1 | `D_8009AFC0` | low (1 aliased) |
| 29 | `fieldfx/emitters` | 1 (0 own) | 12 | scommon 1 | `g_PlayerEntity` | low (1 aliased) |
| 30 | `render/frame_update` | 1 (0 own) | 16 | sdata 1 | `g_ActiveDrawSlot` | low (1 aliased) |
| 31 | `psyq/libds/dsfile_2` | 1 (0 own) | 20 | data 1 | `D_8009AFC0` | low (1 aliased) |
| 32 | `psyq/libspu/s_sca` | 1 (0 own) | 28 | data 1 | `_spu_RXX` | low |
| 33 | `menu/save_slot` | 1 (0 own) | 30 | common 1 | `g_TextboxEntries` | low |
| 34 | `cdrom/track_read` | 1 (0 own) | 34 | common 1 | `g_GameState` | low |
| 35 | `psyq/libspu/s_sra` | 1 (0 own) | 64 | data 1 | `_spu_RXX` | low |
| 36 | `draw/textbox` | 1 (0 own) | 72 | common 1 | `g_TextboxEntries` | low |
| 37 | `menu/equip_screen` | 2 (0 own) | 3 | sbss 2 | `g_InvAmmoSpendActiveList`, `g_MenuEquipSwapSource` | low |
| 38 | `psyq/libgpu/sys_7` | 2 (0 own) | 4 | data 2 | `g_GpuGp0Ptr`, `g_GpuGp1Ptr` | low |
| 39 | `psyq/libds/dssys_1_2` | 2 (0 own) | 4 | data 2 | `g_DsReadSysEnabled`, `g_CdSeekState` | low |
| 40 | `psyq/libcd/c_010` | 2 (1 own) | 4 | common 2 | `g_CdStreamEndState`, `g_CdStreamMask` | low |
| 41 | `psyq/libetc/intr_2` | 2 (0 own) | 6 | data 2 | `g_EventCallbackTable`, `g_IntrMaskRegPtr` | low |
| 42 | `psyq/libspu/s_stsa` | 2 (0 own) | 6 | data 2 | `g_SpuTransferAddr`, `_spu_mem_mode` | low |
| 43 | `psyq/libspu/s_m_util` | 2 (0 own) | 6 | data 2 | `_spu_memList`, `_spu_mem_mode` | low |
| 44 | `psyq/libds/dssys_1_6` | 2 (0 own) | 6 | data 1, bss 1 | `g_DsReadyCallback`, `g_CdSeekState` | low |
| 45 | `boot/frame_loop` | 2 (0 own) | 6 | scommon 1, common 1 | `g_GameState`, `g_GameStateFlags` | low (1 aliased) |
| 46 | `psyq/libcd/c_007` | 2 (0 own) | 6 | common 2 | `StRingAddr`, `StRingSize` | low (1 aliased) |
| 47 | `psyq/libsn/snmain` | 2 (2 own) | 8 | rodata 1, engine 1 | `jtbl_80010000`, `D_800C20C8` | low (2 aliased) |
| 48 | `psyq/libds/dssys_1_8` | 2 (0 own) | 8 | data 2 | `g_DsReadSysEnabled`, `g_DsReadStatusBlock` | low |
| 49 | `battle/enemy_movement` | 2 (0 own) | 8 | scommon 2 | `D_8009D2A0`, `g_ActiveActor` | low (1 aliased) |
| 50 | `fieldfx/geometry` | 2 (2 own) | 8 | common 2 | `D_800B0E18`, `D_800B0E1C` | low (2 aliased) |
| 51 | `menu/equip_select` | 2 (0 own) | 9 | sbss 2 | `g_InvAmmoSpendActiveList`, `g_MenuSpendArrowDirection` | low |
| 52 | `fieldfx/sprite_effects` | 2 (0 own) | 12 | data 1, scommon 1 | `g_RoomFloorY`, `g_PlayerEntity` | low (1 aliased) |
| 53 | `psyq/libetc/intr_vb` | 2 (1 own) | 12 | data 2 | `g_IntrVSyncCallbackTable`, `g_VSyncCount` | low |
| 54 | `entity/allocation` | 2 (0 own) | 12 | scommon 1, common 1 | `g_PlayerEntity`, `g_RenderScratchBufferBase` | low (1 aliased) |
| 55 | `psyq/libpad/waitrc2` | 2 (2 own) | 14 | common 2 | `g_TimerTimeoutStart`, `g_TimerTimeoutLimit` | low |
| 56 | `psyq/libetc/intr` | 2 (0 own) | 18 | data 2 | `g_EventCallbackTable`, `g_IntrMaskRegPtr` | low |
| 57 | `psyq/libetc/intr_dma` | 2 (2 own) | 20 | data 2 | `g_IntrDmaDispatchPtr`, `g_IntrDmaHandlerTable` | low |
| 58 | `psyq/libgpu/sys` | 3 (3 own) | 6 | data 3 | `g_GpuCallbacks`, `g_GpuDebugPrintf`, `g_GraphDebug` | low |
| 59 | `psyq/libpad/padseqd` | 3 (1 own) | 6 | data 3 | `g_MemCardStateDispatchFn`, `g_MemCardIsTransferActiveFn`, `g_MemCardResponseHandler` | low |
| 60 | `time/counters` | 3 (2 own) | 6 | common 2, scommon 1 | `g_GameStateFlags`, `g_GameTimeTable`, `g_PlayTimeSeconds` | low (1 aliased) |
| 61 | `psyq/libcd/c_002` | 3 (0 own) | 6 | common 3 | `g_CdStreamDataReadyFlag`, `g_CdStreamRingReadSlot`, `StRingSize` | low |
| 62 | `psyq/libpad/padcmd` | 3 (2 own) | 8 | func 2, data 1 | `g_MemCardIsTransferActiveFn`, `func_80083C20`, `func_80083C3C` | low (2 aliased) |
| 63 | `psyq/libspu/s_q` | 3 (0 own) | 8 | data 3 | `_spu_EVdma`, `_spu_transferCallback`, `_spu_IRQCallback` | low |
| 64 | `psyq/libcd/c_009` | 3 (0 own) | 10 | common 3 | `StRingAddr`, `g_CdStreamEndState`, `StRingSize` | low (1 aliased) |
| 65 | `psyq/libds/dsready_2` | 3 (2 own) | 11 | func 2, data 1 | `g_DsReadBusy`, `func_8008227C`, `func_8007FC44` | low (2 aliased) |
| 66 | `psyq/libds/dssys_2_2` | 3 (0 own) | 12 | bss 3 | `g_CdDsReadQueue`, `g_CdPendingReadCount`, `g_DsReadCallbackCursor` | low |
| 67 | `psyq/libspu/s_crwa` | 3 (0 own) | 14 | data 3 | `_spu_transferCallback`, `_spu_mem_mode`, `g_SpuReverbWorkAreaTable` | low |
| 68 | `psyq/libpad/padentry` | 3 (0 own) | 18 | data 3 | `g_MemCardObjLookupFn`, `g_MemCardObjArray`, `g_MemCardCommandByte` | low |
| 69 | `psyq/libspu/spu_3` | 3 (0 own) | 26 | data 3 | `_spu_mem_mode`, `_spu_RXX`, `_spu_sys_pcr` | low |
| 70 | `script/value_ops` | 3 (0 own) | 62 | scommon 2, common 1 | `g_CurrentEntity`, `g_PlayerEntity`, `g_RenderStateFlags` | low (1 aliased) |
| 71 | `fieldfx/burst` | 3 (3 own) | 82 | engine 3 | `D_800F3430`, `D_800E2260`, `D_800E2818` | low |
| 72 | `psyq/libspu/s_m_init` | 4 (2 own) | 8 | data 4 | `_spu_mem_mode`, `_spu_AllocBlockNum`, `_spu_AllocLastNum` | low |
| 73 | `psyq/libds/dssys_1_10` | 4 (0 own) | 8 | bss 3, data 1 | `g_DsReadStatusBlock`, `g_DsPollCallback`, `g_DsSyncCallback` | low |
| 74 | `psyq/libcd/c_004` | 4 (1 own) | 10 | common 4 | `g_CdStreamRingReadSlot`, `g_StrDataReadyCallback`, `g_CdStreamDataReadyFlag` | low (1 aliased) |
| 75 | `psyq/libspu/s_srmt` | 4 (1 own) | 14 | data 4 | `_spu_RXX`, `_spu_rev_offsetaddr`, `g_SpuReverbWorkAreaTable` | low |
| 76 | `menu/skills` | 4 (1 own) | 14 | sbss 3, common 1 | `g_MenuInventoryViewMode`, `g_DiscChangeFlags`, `g_MenuEquipSwapSource` | low (1 aliased) |
| 77 | `boot/main` | 4 (0 own) | 14 | scommon 2, common 2 | `g_SceneDispatchToken`, `g_SceneDispatchCur`, `g_PlayTimeFrameCounter` | low |
| 78 | `psyq/libspu/s_sr` | 4 (0 own) | 18 | data 4 | `_spu_rev_flag`, `_spu_RXX`, `_spu_rev_reserve_wa` | low |
| 79 | `psyq/libgpu/sys_10` | 4 (0 own) | 40 | data 4 | `g_GpuDmaChcrPtr`, `g_GpuGp1Ptr`, `g_GpuDmaTimeoutDeadline` | low |
| 80 | `render/object` | 4 (2 own) | 66 | common 2, data 1, sdata 1 | `g_IdentityMatrixSource`, `g_ActiveDrawSlot`, `g_RenderClutLookupTable` | low (1 aliased) |
| 81 | `psyq/libspu/s_ini` | 5 (1 own) | 10 | data 5 | `_spu_EVdma`, `_spu_rev_flag`, `_spu_rev_reserve_wa` | low |
| 82 | `psyq/libds/dssys_2` | 5 (0 own) | 14 | bss 5 | `g_CdDsReadQueue`, `g_CdDsReadQueueState`, `g_CdDsReadIndex` | low |
| 83 | `psyq/libgpu/sys_8` | 5 (0 own) | 20 | data 5 | `g_GpuGp1Ptr`, `g_GpuDmaChcrPtr`, `g_GpuDmaControlRegPtr` | low |
| 84 | `psyq/libpad/padif` | 5 (2 own) | 24 | data 5 | `g_MemCardStateDispatchFn`, `g_MemCardDispatchResult`, `g_MemCardWriteBlockCount` | low |
| 85 | `psyq/libds/dssys_1_4` | 5 (1 own) | 34 | data 4, bss 1 | `g_CdRomCmdTimeout`, `g_CdRomCmdLongTimeoutTable`, `g_DsPollCallback` | low |
| 86 | `psyq/libds/dssys_2_3` | 6 (1 own) | 22 | bss 5, common 1 | `g_DsReadCallbackCursor`, `g_CdPendingReadCount`, `g_CdDsReadQueue` | low |
| 87 | `psyq/libgpu/sys_6` | 6 (2 own) | 42 | data 5, bss 1 | `g_GpuGp0Ptr`, `g_GpuGp1Ptr`, `g_GpuDmaMadrPtr` | low |
| 88 | `psyq/libspu/spu_2` | 7 (3 own) | 56 | data 7 | `_spu_RXX`, `g_SpuDmaDirection`, `g_SpuTransferAddr` | low |
| 89 | `cdrom/scene_assets` | 8 (3 own) | 38 | common 6, data 2 | `g_GameState`, `g_Base32CharTable`, `g_DiscChangeFlags` | low |
| 90 | `psyq/libpad/padmain` | 8 (3 own) | 44 | data 8 | `g_MemCardSioRegs`, `g_MemCardPort1Present`, `g_MemCardPort2Present` | low |
| 91 | `render/font` | 1 (1 own) | 110 | data 1 | `g_FontSelectionState` | medium |
| 92 | `menu/main_update` | 2 (0 own) | 16 | sdata 1, scommon 1 | `g_ActiveDrawSlotRecord`, `D_8009D1D4` | medium (2 aliased) |
| 93 | `battle/context` | 3 (0 own) | 12 | scommon 2, data 1 | `g_ActiveActor`, `g_PlayerEntity`, `g_RoomFloorY` | medium (2 aliased) |
| 94 | `asset/unload_table` | 3 (0 own) | 16 | data 1, scommon 1, common 1 | `g_PmCmdHandlerTable`, `g_GameStateFlags`, `g_GameState` | medium (2 aliased) |
| 95 | `battle/commands` | 3 (0 own) | 18 | common 2, scommon 1 | `g_PlayerEntity`, `g_AyaEquippedWeaponSlot`, `g_AyaParasiteSpellFlags` | medium (2 aliased) |
| 96 | `asm:dtail_gp_pre_s016` | 4 (4 own) | 4 | func 4 | `func_80017928`, `func_80017948`, `func_80017968` | medium (4 aliased) |
| 97 | `draw/frame` | 4 (0 own) | 15 | common 3, sdata 1 | `g_ActiveDrawSlot`, `g_GameState`, `g_RenderDrawEnvArray` | medium (1 aliased) |
| 98 | `battle/phase` | 5 (0 own) | 22 | scommon 3, data 1, sbss 1 | `D_8009D1D4`, `g_PmSlotTable`, `D_8009D258` | medium (1 aliased) |
| 99 | `menu/inventory` | 5 (1 own) | 28 | sbss 5 | `g_MenuActiveWidget`, `g_MenuSelectedItemData`, `g_MenuEquipSwapSource` | medium (1 aliased) |
| 100 | `asset/tim_textures` | 5 (1 own) | 32 | data 3, scommon 1, common 1 | `g_GameStateFlags`, `g_PmCmdHandlerTable`, `g_PmTables` | medium (3 aliased) |
| 101 | `item/swap` | 5 (1 own) | 32 | common 3, sbss 2 | `g_EquippedWeaponSlot`, `g_EquippedArmorSlot`, `g_EquipItemDataTable` | medium (2 aliased) |
| 102 | `pm/process` | 5 (1 own) | 122 | data 3, scommon 1, common 1 | `g_PmSlotTable`, `g_PmSlotTable2`, `g_GameState` | medium (2 aliased) |
| 103 | `psyq/libpad/padportd` | 6 (0 own) | 14 | data 6 | `g_MemCardServiceReady`, `g_MemCardObjResetFn`, `g_MemCardObjLookupFn` | medium |
| 104 | `aya/stats` | 6 (3 own) | 17 | scommon 2, common 2, sbss 1, bss 1 | `g_AkaoBgmHandle`, `g_GameStateFlags`, `g_PlayerEntity` | medium (2 aliased) |
| 105 | `psyq/libds/dssys_1_5` | 6 (0 own) | 34 | data 3, bss 3 | `g_DsSyncCallback`, `g_DsDispatchCallback`, `g_DsReadSysEnabled` | medium |
| 106 | `entity/combat` | 6 (4 own) | 38 | scommon 6 | `g_PlayerEntity`, `g_ActiveActor`, `g_BattleHitActionMode` | medium (2 aliased) |
| 107 | `battle/encounter` | 6 (2 own) | 42 | scommon 3, func 1, sbss 1, common 1 | `g_BattleEndTimer`, `g_GameState`, `g_BattleEquipStateBlock` | medium (1 aliased) |
| 108 | `battle/enemy_actions` | 6 (2 own) | 58 | scommon 6 | `g_ActiveActor`, `g_ActorEffectFlag100Timer`, `g_FieldMoveLock` | medium (3 aliased) |
| 109 | `script/node_actions` | 6 (0 own) | 121 | scommon 4, sbss 2 | `g_CurrentEntity`, `g_ScriptNodePool`, `g_SceneDataTable0` | medium (2 aliased) |
| 110 | `draw/pipeline` | 7 (1 own) | 14 | scommon 3, common 2, data 1, sdata 1 | `g_GameState`, `g_RoomFloorY`, `g_GameStateFlags` | medium (1 aliased) |
| 111 | `psyq/libds/dssys_1` | 7 (0 own) | 16 | data 4, bss 3 | `g_DsReadyCallback`, `g_CdSyncCallback`, `g_CdReadyCallback` | medium (2 aliased) |
| 112 | `script/node_pool` | 7 (2 own) | 21 | sbss 3, scommon 2, bss 1, common 1 | `g_ScriptNodeFreeListHead`, `g_SceneDataTable1`, `g_ScriptNodeSeqCounter` | medium (1 aliased) |
| 113 | `psyq/libspu/spu` | 7 (0 own) | 60 | data 7 | `_spu_RXX`, `_spu_mem_mode`, `g_SpuTransferAddr` | medium |
| 114 | `entity/pool` | 8 (0 own) | 14 | scommon 5, common 3 | `g_PlayerEntity`, `g_EntityFreePoolCount`, `g_EntityFreeListHead` | medium (2 aliased) |
| 115 | `psyq/libgpu/sys_9` | 9 (0 own) | 42 | data 9 | `g_GpuGp1Ptr`, `g_GpuDmaQueueTail`, `g_GpuGp0Ptr` | medium |
| 116 | `menu/item_actions` | 9 (4 own) | 66 | sbss 6, common 2, bss 1 | `g_BonusPointSpendStat`, `g_BonusPointSpendWorkingValue`, `g_BonusPointStatDeltas` | medium (1 aliased) |
| 117 | `render/mesh_list` | 9 (3 own) | 92 | common 6, data 1, sdata 1, scommon 1 | `g_GeomState`, `g_RenderStateFlags`, `g_ActiveDrawSlot` | medium (2 aliased) |
| 118 | `draw/wipe` | 9 (3 own) | 116 | sbss 7, bss 1, common 1 | `g_TextCursorStackTop`, `g_DrawVertexWritePtr`, `g_TextCursorX` | medium (1 aliased) |
| 119 | `script/flow_opcodes` | 9 (1 own) | 131 | scommon 6, sbss 2, common 1 | `g_CurrentEntity`, `g_ScriptNodePool`, `g_SceneDataTable0` | medium (5 aliased) |
| 120 | `menu/save_screens` | 10 (3 own) | 39 | sbss 6, bss 2, common 2 | `D_800A19C0`, `g_MenuRenameTargetRecord`, `g_SaveSelectedSlot` | medium (1 aliased) |
| 121 | `menu/lists` | 11 (0 own) | 29 | sbss 7, common 3, bss 1 | `g_MenuActiveWidget`, `g_BonusPointStatDeltas`, `g_SaveSelectedSlot` | medium (2 aliased) |
| 122 | `field/actor_collision` | 11 (5 own) | 35 | scommon 6, sbss 5 | `D_8009CE0C`, `g_CollisionPlaneTable`, `g_CollisionDb` | medium (5 aliased) |
| 123 | `render/camera` | 11 (5 own) | 176 | common 10, scommon 1 | `g_GeomState`, `g_RenderStateFlags`, `g_GeomVramPacketDst` | medium (1 aliased) |
| 124 | `scene/room_load` | 12 (4 own) | 128 | common 7, scommon 4, data 1 | `g_GameState`, `g_PlayerEntity`, `g_GameStateFlags` | medium (4 aliased) |
| 125 | `boot/init` | 13 (6 own) | 95 | scommon 7, common 5, sdata 1 | `g_RenderDrawEnvArray`, `g_GameStateFlags`, `g_RenderDispEnvArray` | medium (2 aliased) |
| 126 | `menu/main_menu` | 15 (7 own) | 85 | sbss 9, bss 4, common 2 | `g_MenuPendingTotalExp`, `g_MenuLevelDisplayValue`, `g_BonusPointStatDeltas` | medium |
| 127 | `psyq/libds/dssys_1_3` | 16 (6 own) | 34 | data 12, bss 4 | `g_CdRomCmdTimeout`, `g_CdSyncCallback`, `g_CdReadyCallback` | medium (2 aliased) |
| 128 | `draw/text` | 16 (5 own) | 83 | sbss 12, bss 2, sdata 1, common 1 | `g_TextCursorStackBottom`, `g_TextCursorStackTop`, `D_8009D10C` | medium (2 aliased) |
| 129 | `item/armor_capacity` | 16 (10 own) | 103 | sbss 9, data 3, common 3, bss 1 | `g_InvStorageSelectionBits`, `g_InvSortListBase`, `g_InvSortListCount` | medium (2 aliased) |
| 130 | `akao/spu_transfer` | 17 (2 own) | 78 | scommon 11, common 5, sdata 1 | `g_AkaoSpuCommonAttr`, `g_SpuTransferStatus`, `g_AkaoTimerEventDesc` | medium |
| 131 | `menu/input_queue` | 18 (8 own) | 67 | sbss 11, common 4, sdata 1, scommon 1, bss 1 | `g_MenuEventQueueFreeList`, `g_MenuEventQueueHead`, `g_MenuEventQueueTail` | medium (1 aliased) |
| 132 | `menu/status_panels` | 18 (7 own) | 71 | sbss 9, common 5, bss 4 | `g_MenuSaveBgFadeState`, `g_MenuSaveBgFadeHeight`, `g_MenuSaveBgFadeStep` | medium |
| 133 | `menu/widget` | 18 (8 own) | 202 | sbss 12, bss 5, sdata 1 | `g_MenuWidgetColumnLayoutTable`, `g_MenuWidgetActiveListHead`, `g_TextCursorStackPtr` | medium (2 aliased) |
| 134 | `pm/disc_tables` | 19 (5 own) | 74 | common 14, data 3, sdata 1, scommon 1 | `g_GameState`, `g_DrawEnabled`, `g_SeqElapsed` | medium (1 aliased) |
| 135 | `akao/sequencer` | 22 (5 own) | 188 | scommon 9, common 9, data 4 | `g_AkaoCurTrack`, `g_AkaoVoiceUpdateFlags`, `g_AkaoInstrumentTable` | medium |
| 136 | `psyq/libcd/sys` | 4 (1 own) | 8 | data 4 | `D_8009AFC4`, `g_CdLastPos`, `g_CdMode` | high (1 aliased) |
| 137 | `psyq/libcd/bios_6` | 4 (0 own) | 8 | data 4 | `g_CdLastCom`, `D_8009AFDC`, `D_8009B05C` | high (2 aliased) |
| 138 | `psyq/libcd/bios_2` | 7 (0 own) | 32 | data 7 | `D_8009B27C`, `D_8009AFB4`, `D_8009AFB8` | high (5 aliased) |
| 139 | `psyq/libcd/sys_2` | 7 (3 own) | 50 | data 6, rodata 1 | `D_8009AFB4`, `D_8009AFC4`, `func_800119CC` | high (5 aliased) |
| 140 | `psyq/libcd/bios_4` | 9 (1 own) | 26 | data 9 | `g_CdRegIndexBase`, `g_CdRegDataWrite`, `g_CdRegResponse` | high (6 aliased) |
| 141 | `psyq/libcd/bios_5` | 12 (1 own) | 36 | data 12 | `D_8009B288`, `D_8009B27C`, `D_8009AFC4` | high (6 aliased) |
| 142 | `entity/frame_update` | 12 (5 own) | 44 | scommon 6, func 4, common 2 | `g_GameState`, `g_PlayerEntity`, `g_EntityFreeListHead` | high (7 aliased) |
| 143 | `psyq/libcd/bios` | 13 (3 own) | 62 | data 13 | `D_8009B294`, `D_8009B27C`, `D_8009AFC4` | high (8 aliased) |
| 144 | `psyq/libcd/bios_7` | 14 (6 own) | 46 | data 14 | `D_8009B27C`, `g_CdRegIndexBase`, `D_8009B28C` | high (8 aliased) |
| 145 | `item/actions` | 14 (2 own) | 133 | common 7, bss 4, sbss 3 | `g_InvCompareSlotLeft`, `g_InvCompareSlotRight`, `g_InvStorageSelectionBits` | high (4 aliased) |
| 146 | `script/actor_opcodes` | 15 (4 own) | 192 | scommon 8, data 3, common 2, func 1, sbss 1 | `g_CurrentEntity`, `g_GameStateFlags`, `g_ScriptNodePool` | high (6 aliased) |
| 147 | `battle/setup` | 16 (2 own) | 70 | scommon 12, sbss 3, bss 1 | `g_ActiveActor`, `g_BattleTargetList`, `g_GameStateFlags` | high (9 aliased) |
| 148 | `asset/game_state` | 17 (1 own) | 110 | common 14, scommon 2, sdata 1 | `g_GameState`, `g_PeImageMountFlags`, `g_ActiveDrawSlot` | high (2 aliased) |
| 149 | `battle/status_hud` | 17 (9 own) | 243 | scommon 7, sbss 3, bss 3, sdata 2, common 2 | `g_ActiveDrawSlot`, `D_8009CE84`, `g_OtBufferTable` | high (6 aliased) |
| 150 | `battle/turn` | 18 (4 own) | 183 | scommon 11, sbss 5, bss 1, common 1 | `g_BattleTargetList`, `D_8009D2D8`, `g_AkaoBgmHandle` | high (7 aliased) |
| 151 | `akao/voice_driver` | 18 (1 own) | 518 | common 13, scommon 4, sdata 1 | `g_AkaoCmd`, `g_AkaoCurTrack`, `g_AkaoVoiceUpdateFlags` | high |
| 152 | `psyq/libcd/bios_3` | 19 (3 own) | 80 | data 19 | `g_CdRegResponse`, `g_CdRegIndexBase`, `D_8009B294` | high (16 aliased) |
| 153 | `script/system_opcodes` | 20 (2 own) | 294 | scommon 11, common 8, sbss 1 | `g_CurrentEntity`, `g_FieldMoveLock`, `g_GameStateFlags` | high (6 aliased) |
| 154 | `item/active_list` | 21 (5 own) | 76 | common 10, sbss 7, bss 4 | `g_EquipItemDataTable`, `g_ItemDropRngXorKeyHigh`, `g_ItemDropRngXorKeyLow` | high (4 aliased) |
| 155 | `save/game_data` | 21 (6 own) | 134 | common 11, sbss 7, scommon 2, bss 1 | `g_SaveRuntimeState`, `g_SaveMetadataCursor`, `g_SaveMetadataWindowLength` | high (4 aliased) |
| 156 | `battle/attack` | 21 (14 own) | 204 | scommon 13, sbss 7, bss 1 | `g_ActiveActorRecord`, `g_BattleResultFlagsRecord`, `D_8009D25C` | high (8 aliased) |
| 157 | `akao/voice_bank` | 30 (9 own) | 429 | scommon 13, common 12, func 3, data 1, sdata 1 | `g_AkaoCurTrack`, `g_AkaoCmd`, `g_SpuActiveVoiceMask` | high (3 aliased) |
| 158 | `memcard/save_flow` | 37 (29 own) | 544 | bss 26, common 6, scommon 3, data 2 | `g_SaveIoCursor`, `g_MemCardPortStates`, `g_GameState` | high (1 aliased) |

## 6. What cannot shift: disc and external ABI

These addresses are fixed on purpose. Each needs a named constant in one
documented place (an ABI symbol file), not literals spread through the code.

| item | address(es) | owner in the code | why fixed |
| --- | --- | --- | --- |
| PS-X EXE load address | `.main` at 0x80010000 | `asm/USA/main/header.s`, manifest | BIOS loader contract |
| Initial stack | 0x801FFFF0 (header), `_ramsize` 2 MiB | header, `snmain.c` | BIOS/loader convention; `_ramsize` and `_stacksize` are data, not addresses |
| Image end | 0x801FE000 (text size 0x1EE000) | header | the image covers the overlay windows below the stack |
| Room, scene and field effect overlay window | 0x8018EFE8 (`fx_common` at 0x8018EFF0) | `Scene_LoadRoom` and the asset loaders; `g_StrFileDirBuffer` literal | 185 overlays in `PE.IMG` are linked there |
| Other overlay windows | `mdec_diag` 0x8010BCF8, `menu_memcard` 0x80120D00, `boot_display` 0x80122FA4, `fx_common_sector17` 0x80170000 | their loaders | linked for those addresses |
| Runtime buffers inside the windows | e.g. 0x801ED7F8 task program buffer (`g_PmTaskProgramBuffer` literal), 0x801F1xxx process slots, 0x801235DC/0x801909B4/0x8019234C used by `boot/main` | `pm/process`, `pm/disc_tables`, `boot/main`, `script/actor_opcodes`, `boot/frame_loop` | share the window space with the overlays |
| Overlay code in `PE.IMG` | every main address an overlay calls or reads (389 distinct) | overlays | the shipped binaries are prelinked against retail main; ABI until overlays link against the built main and `PE.IMG` is repacked (section 3.7) |
| `PE.IMG` sector layout | sector numbers in main's tables (e.g. the special-scene table at 0x80094048, `g_SceneSectorDirectory`) | `Asset_OpenPeImage`, `Scene_LoadRoom` | disc positions, not program addresses; unaffected by a shift |
| Save files | card file: 0x100-byte header + 0x12E4-byte copy of `g_SaveRuntimeState` | `memcard/save_flow`, `save/game_data` | the layout is the ABI; no typed field holds a program address, the untyped `inventory` (0x1064) and `remainder` (0x20C) parts still need checking |
| Kernel and BIOS | 0x80000000 vectors, 0xA000xxxx, LIBCARD's kernel patch at 0xDFFC | PsyQ LIBAPI/LIBCARD | BIOS contract |
| Hardware and scratchpad | 0x1F80xxxx | GPU, SPU, CD, pad drivers | hardware |
| External cheat devices | e.g. JP debug flag 0x8009CD3C | none | not ours; a shifted build breaks GameShark codes, accepted |

Not in the list because the scan found none: data in `PE.IMG` (room
directories, scripts, TIM, AKAO) holding a 32-bit pointer into main. The room
directory is relocated into `g_GameState` by offsets, not by absolute
addresses.

## 7. Pilot: `script/sfx_queue`

Three pinned symbols referenced, the smallest game unit with pins in three
different groups.

| symbol | group | result |
| --- | --- | --- |
| `g_FieldSfxQueue` (0x150 bytes) | `.bss` object run | stage B: `static` in `sfx_queue.c`, `.bss` row at 0x93980, blob split (`dtail_gp_post_sfx_queue`) |
| `g_FieldSfxQueueCount` | `.sdata` | stage A: blob label; stage B blocked by forced-G0 (section 4) |
| `g_FieldActorListHead` | small COMMON pool | stage A: blob label; `D_8009D20C` renamed in `battle/{attack,encounter,phase,setup,turn}`, `script/{actor_opcodes,flow_opcodes}`, `menu/main_update` and `battle_runtime.h` |

Commits on the branch:

1. `Link the field sfx queue state by name instead of absolute pins` (pilot,
   plus `gen_expected.py` counting in-image `.bss` and its tests).
2. `Derive $gp and the field engine address from the layout` (`_ramsize`,
   `_stacksize`, `ld_gp_expression`, `vram_symbol`).

Before and after (shift test, pad 0x10):

| measure | before | after |
| --- | ---: | ---: |
| fixed placements in the linker script | `.field_engine`, `_gp` | none |
| padded relink without rebasing | fails (overlap) | links |
| pinned address words | 6498 | 6443 |
| gp-relative words to pinned symbols | 1265 | 1243 |
| distinct pinned program symbols used | 503 | 499 |
| referenced assignments in `undefined_syms_manual.txt` | 483 | 480 |
| `script/sfx_queue` relocations to pinned symbols | 17 | 0 |

`make check`, all 190 overlays, `organization-check`, `source-policy-check`,
`debt-check` and `make report` pass. No function changed (lev 0 everywhere,
byte-identical image).

## 8. Decisions needed

- **MASPSX and `.comm` in `.sbss`.** MASPSX's default path turns a small
  `.comm` into a local `.sbss` label (only `.bss` labels get `.globl`), which
  is why 52 main units use `--use-comm-section` and keep real COMMON symbols.
  Owning the 102 `.sbss` object-run variables needs a global label there. If
  ASPSX did emit it global (to be checked against a PsyQ object with a small
  global `.comm`), this is a fidelity fix in MASPSX, not a crutch.
  *Checked 2026-10-13: ASPSX did not.* The raw PsyQ 4.0 `LIBGTE.LIB`
  `CLIP_INI.OBJ` declares `CLIPNearz`, `CLIPHdkx` and four more 4-byte
  globals as XBSS records (SN's common block, allocated by psylink), not as
  labels in the object's `.sbss`/`.bss`; the 4.0 signatures list XBSS for
  every small tentative global of LIBCD (`StRingSize`, `CChannel`, 20 in
  `CDROM.OBJ`), LIBDS, LIBETC and LIBSPU. No PsyQ library uses small data
  (all are -G0), so the `.sbss` case is seen in this game: the small COMMON
  pool after the `.sbss` object run is exactly psylink allocating small
  XBSS, and `StRingSize` (4 bytes, -G0 LIBCD) sits in the large pool.
  `--use-comm-section` is therefore the faithful mode and a global `.sbss`
  label would not be a fidelity fix. A `.sbss` object-run variable that
  several of our units reference is a file-static (`.lcomm`) of one retail
  object, i.e. TU-merge evidence, or stays a blob label.
- **COMMON pools.** Keep them as labelled blobs (shiftable, simplest), or
  model them as pool units in retail order. Recommendation: blobs now, pool
  units only if the PsyQ COMMON allocation order can be explained.
- **Manifest post-pass.** In-image BSS with a COMMON catch-all needs either a
  splat change or a small checked rewrite of `main.ld` after `make split`.
- **Relative linker aliases** (`old = new;`) as the fallback when unifying
  names would change code: allowed as migration state?
