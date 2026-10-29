# US and JP executable comparison

`tools/scripts/us_jp_diff.py <us main.exe> <jp SLPS_012.30> <nm -S -n output> <out.csv>`
compares every USA function against the JP executable (`disc/jp/extracted/SLPS_012.30`,
SLPS-012.30) and writes a per-function CSV (class and JP address). The CSV is an analysis
artefact, not part of the build. `tools/scripts/jp_func_diff.py` diffs one function pair.

Both executables are 2,025,472 bytes with the same load address (0x80010000) and the
same size header, but the code is not laid out identically.

| Class | Functions | Meaning |
| --- | ---: | --- |
| identical | 97 | same bytes at the same address |
| relocated | 243 | same address, equal once jump targets and lui/addiu/load-store immediates are masked |
| shifted | 2083 | same code found at another JP address (column `jp_address`) |
| different | 90 | no masked match anywhere in JP (changed or JP-only code) |

The first 0x11000 bytes of text (up to Pad_GetMenuPressedBitOrDisabled) are identical.
After that JP runs 4 to 1200 bytes below the USA addresses, in 383 runs of constant
offset, so the JP image is the USA code with small insertions and deletions rather
than a different program. Two single functions sit 17.7 KB away
(Menu_DrawBlendColorChannelList, Menu_DrawBlendColorOptionList).

## JP target (`make VERSION=JP`)

JP is a separate target with its own tree, like USA: `configs/JP/`, `linkers/JP/`,
`assets/JP/`, `build/JP/` and `src-jp/` (hand-maintained, `src-jp/main`, `src-jp/overlays`).
Nothing is generated at build time and the C is not shared with `src/`; a fix in one tree
does not automatically apply to the other, because the same C has to produce different
bytes in each.

`make VERSION=JP split build check` reassembles `SLPS_012.30` byte for byte (sha1
411bd943...). `configs/JP/main.yaml` lists which units are C (291 translation units and 24 slices of translation units, taken
from the USA ones that are byte-identical in JP) and which are asm (`jp/text_*`);
`configs/JP/sym.main.txt` holds the JP addresses of the names. The target exe is copied to
`assets/JP/main.exe` (ignored, like `assets/USA`).

`tools/scripts/jp_asm_fixup.py` rewrites `sd`/`ld`/`sdc2`... lines that splat mis-decodes
inside data to `.word`, because GAS expands them and shifts every later address.

A converted unit that calls a function whose JP address differs needs that name pinned in
`linkers/JP/undefined_syms_manual.txt`, and absolute addresses in the C (`0x80070E0C`) need their JP
value by hand (`src-jp/main/draw/prim_queue.c`). Undefined data names (the PsyQ CD/DS state, SPU
registers) are pinned there too, at their JP addresses.

A USA unit that contains a function JP changed, or that JP split with new code, is built as
several files: `src-jp/main/menu/lists__s0.c`, `__s1.c`, ... each hold one run of functions that
are adjacent and unchanged in JP, with the unit's shared declarations; the changed function stays in
asm in the gap between them (`configs/JP/main.yaml` lists the slices as ordinary `c` units).
Static helpers are made external and prototyped where another slice calls them.

To convert another unit, write `src-jp/main/<unit>.c`, change its line in `configs/JP/main.yaml`
from `asm` to `c`, and run `make VERSION=JP split build check`.

## JP overlays

`configs/JP/overlays/*.yaml` are the USA overlay configs with the JP `PE.IMG` offsets and
sha1, window vrams moved and symbols retargeted. `tools/scripts/jp_overlay_check.py --all`
builds and checks them: 186 of 188 match byte for byte. `menu_memcard` is JP code that
differs after offset 0x6B4C, and `boot_display` is off by 4 in about 190 window-relative
operands (open). JP moved the overlay windows: mdec_diag and menu_memcard by -0x540,
boot_display by -0x400, the rest stay (`linkers/JP/overlay_windows.txt`).

## JP match report

The USA report pipeline runs for JP with `PE_VERSION=JP` (`gen_expected.py`, `objdiff_config.py`;
`SRC` is `src-jp` there). `menu_memcard` and `boot_display` do not reproduce their retail
binaries and their retail disassembly overlaps itself, so they are left out with
`PE_SKIP_OVERLAYS=menu_memcard,boot_display` and added back as unmatched (85 functions, 48,124 bytes,
taken from the USA report):

    PE_VERSION=JP PE_SKIP_OVERLAYS=menu_memcard,boot_display make VERSION=JP expected objdiff-config
    tools/objdiff/objdiff-cli report generate -p . -o build/JP/report.json

| | functions matched | code bytes matched |
| --- | ---: | ---: |
| Main: game code | 986 of 1797 (54.9%) | 286,128 of 1,932,572 (14.8%) |
| Main: PsyQ libraries | 523 of 611 (85.6%) | 78,612 of 81,296 (96.7%) |
| Room overlays | 7929 of 7929 | 2,390,640 of 2,390,640 |
| Battle scene overlays | 1194 of 1194 | 407,388 of 407,388 |
| System overlays | 25 of 25 | 6,916 of 6,916 |
| **Total, with the two overlays added back** | **10,657 of 11,641 (91.5%)** | **3,169,684 of 4,866,936 (65.1%)** |

USA is 100% of 11,646 functions. The code-byte figure counts the asm data blobs of main as
code, as the USA report does, so it is dominated by the unconverted main.
