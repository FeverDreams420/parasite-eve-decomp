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
411bd943...). `configs/JP/main.yaml` lists which units are C (243 translation units, taken
from the USA ones that are byte-identical in JP) and which are asm (`jp/text_*`);
`configs/JP/sym.main.txt` holds the JP addresses of the names. The target exe is copied to
`assets/JP/main.exe` (ignored, like `assets/USA`).

`tools/scripts/jp_asm_fixup.py` rewrites `sd`/`ld`/`sdc2`... lines that splat mis-decodes
inside data to `.word`, because GAS expands them and shifts every later address.

To convert another unit, write `src-jp/main/<unit>.c`, change its line in `configs/JP/main.yaml`
from `asm` to `c`, and run `make VERSION=JP split build check`.

## JP overlays

`configs/JP/overlays/*.yaml` are the USA overlay configs with the JP `PE.IMG` offsets and
sha1, window vrams moved and symbols retargeted. `tools/scripts/jp_overlay_check.py --all`
builds and checks them: 186 of 188 match byte for byte. `menu_memcard` is JP code that
differs after offset 0x6B4C, and `boot_display` is off by 4 in about 190 window-relative
operands (open). JP moved the overlay windows: mdec_diag and menu_memcard by -0x540,
boot_display by -0x400, the rest stay (`linkers/JP/overlay_windows.txt`).
