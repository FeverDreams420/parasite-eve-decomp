# US and JP executable comparison

`tools/scripts/us_jp_diff.py <us main.exe> <jp SLPS_012.30> <nm -S -n output> <out.csv>`
compares every USA function against the JP executable (`disc/jp/extracted/SLPS_012.30`,
SLPS-012.30). `docs/us_jp_function_map.csv` is its output for the current build.

Both executables are 2,025,472 bytes with the same load address (0x80010000) and the
same size header, but the code is not laid out identically.

| Class | Functions | Meaning |
| --- | ---: | --- |
| identical | 415 | same bytes at the same address |
| relocated | 243 | same address, equal once jump targets and lui/addiu/load-store immediates are masked |
| shifted | 1883 | same code found at another JP address (column `jp_address`) |
| different | 579 | no masked match anywhere in JP (changed code, or the match needs a longer window) |

The first 0x11000 bytes of text (up to Pad_GetMenuPressedBitOrDisabled) are identical.
After that JP runs 4 to 1200 bytes below the USA addresses, in 383 runs of constant
offset, so the JP image is the USA code with small insertions and deletions rather
than a different program. Two single functions sit 17.7 KB away
(Menu_DrawBlendColorChannelList, Menu_DrawBlendColorOptionList).

A JP target therefore needs its own address map (symbols and link order) but can
share most C with the USA build. Next steps are in the commit that follows this one.

## JP target (`make VERSION=JP`)

JP is a separate target, not a mode of the USA build: `configs/JP/`, `linkers/JP/`,
`build/JP/`, and (later) its own sources under `src/jp/`.

Stage 1 is done: `make VERSION=JP split build check` reassembles `SLPS_012.30` byte for
byte (sha1 411bd943...) from one splat asm segment, with the USA function names carried
over to their JP addresses (`configs/JP/sym.main.txt`, 1,6xx functions; where several USA
names map to one JP address the first is kept). The target exe is copied to
`assets/JP/main.exe` (ignored, like `assets/USA`).

`tools/scripts/jp_asm_fixup.py` rewrites `sd`/`ld`/`sdc2`... lines that splat mis-decodes
inside data to `.word`, because GAS expands them and shifts every later address.

## JP overlays

`docs/jp_overlay_map.csv` locates every USA overlay slice in the JP `PE.IMG` (small offset deltas;
`tools/scripts/jp_overlay_map.py`). `tools/scripts/jp_overlay_gen.py` writes `configs/JP/overlays/*.yaml`
from the USA configs (JP offsets and sha1, window vrams moved, symbol addresses mapped through the pairs
of USA/JP instruction operands) and ports the sources to `src/jp/overlays` (`jp_port_c.py --overlay`).
`tools/scripts/jp_overlay_check.py --all` builds and checks them: 187 of 190 overlay slices match
byte for byte (two slices have no config; `menu_memcard` is JP code that differs after offset 0x6B4C).
JP moved the overlay windows: mdec_diag and menu_memcard by -0x540, boot_display by -0x400, the rest
stays (`linkers/JP/overlay_windows.txt`).
