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
