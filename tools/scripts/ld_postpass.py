#!/usr/bin/env python3
"""Checked post-pass over splat's main linker script (run by `make split`).

Splat 0.50 writes main's BSS as a separate NOLOAD output section and places
no small data or COMMON that the manifest does not list. Retail's BSS is zero
bytes inside the PS-X EXE image and the field engine follows main in both the
file and memory, so this pass rewrites `linkers/USA/main.ld`
(docs/SHIFTABILITY_PLAN.md 3.5 and 3.6):

- in-image BSS: the `.bss` inputs of `.main_bss (NOLOAD)` move into `.main`
  after main's data, so `__romPos` (SIZEOF(.main)) covers them and the field
  engine's load and run addresses cannot diverge;
- catch-alls for data that no manifest row or data label places, all empty
  in the retail layout: `*(.sdata)` at the end of .sdata (the manifest's
  `main_sbss` linker_offset row), `*(.sbss)` and `*(.scommon)` at the end of
  the small COMMON pool (`main_scommon`), so that new small data stays in
  the positive $gp window, and `*(COMMON)` after main's BSS;
- startup symbols: `_edata` and `__bss_start` at `main_sbss` (end of .sdata,
  where the startup's BSS clear starts) and `_end` at the end of main, where
  the clear stops and the heap begins;
- an assertion that the field engine has no NOLOAD BSS: its BSS still lives
  in the `dtail_0D0860` blob together with the overlay windows (plan 3.4).

The catch-alls come after every explicit input of their kind, because ld
gives an input section to the first statement in the script that matches it.
Every rewrite checks the exact shape it expects and fails otherwise, so a
splat upgrade or a manifest change cannot silently produce a different link.
The pass is idempotent.

Usage: ld_postpass.py [linker_script]
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
DEFAULT = ROOT / "linkers/USA/main.ld"
MARKER = "/* ld_postpass: in-image BSS */"

BSS_BLOCK = re.compile(
    r"    main_bss_VRAM = ADDR\(\.main_bss\);\n"
    r"    \.main_bss \(NOLOAD\) : SUBALIGN\(4\)\n"
    r"    \{\n"
    r"        FILL\(0x00000000\);\n"
    r"        main_BSS_START = \.;\n"
    r"(?P<inputs>(?:        \S+\(\.bss\*\);\n)*)"
    r"        \. = ALIGN\(\., 4\);\n"
    r"        main_BSS_END = \.;\n"
    r"        main_BSS_SIZE = ABSOLUTE\(main_BSS_END - main_BSS_START\);\n"
    r"    \}\n")
DATA_END = re.compile(
    r"(        main_DATA_SIZE = ABSOLUTE\(main_DATA_END - main_DATA_START\);\n)"
    r"(    \}\n)")
ROM_ADVANCE = "    __romPos += SIZEOF(.main);\n"
SDATA_END = "        main_main_sbss_OFFSET = .;\n"
SCOMMON_END = "        main_main_scommon_OFFSET = .;\n"
ENGINE_BSS_END = ("        field_engine_BSS_SIZE = ABSOLUTE(field_engine_BSS_END - field_engine_BSS_START);\n"
                  "    }\n")


class PostPassError(Exception):
    pass


def rewrite(text: str) -> str:
    if MARKER in text:
        return text
    blocks = list(BSS_BLOCK.finditer(text))
    if len(blocks) != 1:
        raise PostPassError(f"expected one .main_bss (NOLOAD) block of .bss inputs, found {len(blocks)}")
    block = blocks[0]
    if not text[block.end():].startswith(ROM_ADVANCE):
        raise PostPassError(".main_bss is not followed by the __romPos advance over .main")
    ends = list(DATA_END.finditer(text, 0, block.start()))
    if not ends or ends[-1].end() != block.start():
        raise PostPassError("main_DATA_SIZE does not close .main right before .main_bss")
    for mark, row in ((SDATA_END, "main_sbss"), (SCOMMON_END, "main_scommon")):
        if text.count(mark) != 1:
            raise PostPassError(f"expected one `{row}` linker_offset row in main (configs/USA/main.yaml)")
    if not text.index(SDATA_END) < text.index(SCOMMON_END) < ends[-1].start():
        raise PostPassError("`main_sbss` and `main_scommon` must be in that order inside main's data")
    if text.count(ENGINE_BSS_END) != 1:
        raise PostPassError("expected one .field_engine_bss section")

    in_image = (
        f"        {MARKER}\n"
        "        main_BSS_START = .;\n"
        f"{block.group('inputs')}"
        "        *(COMMON)\n"
        "        . = ALIGN(., 4);\n"
        "        main_BSS_END = .;\n"
        "        main_BSS_SIZE = ABSOLUTE(main_BSS_END - main_BSS_START);\n"
        "        _end = .;\n")
    data_end = ends[-1]
    text = (text[:data_end.start()] + data_end.group(1) + in_image + data_end.group(2)
            + "    main_bss_VRAM = main_BSS_START;\n" + text[block.end():])
    text = text.replace(SDATA_END, "        *(.sdata .sdata.*)\n" + SDATA_END
                        + "        _edata = .;\n        __bss_start = .;\n")
    text = text.replace(SCOMMON_END, "        *(.sbss .sbss.*)\n        *(.scommon)\n" + SCOMMON_END)
    text = text.replace(
        ENGINE_BSS_END,
        ENGINE_BSS_END + "    /* No semicolon: ASSERT is a statement inside SECTIONS. */\n"
        "    ASSERT(SIZEOF(.field_engine_bss) == 0,\n"
        "        \"field engine BSS is not in the image yet (docs/SHIFTABILITY_PLAN.md 3.4)\")\n")
    return text


def main(argv: list[str]) -> int:
    path = Path(argv[1]) if len(argv) > 1 else DEFAULT
    try:
        new = rewrite(path.read_text())
    except PostPassError as exc:
        print(f"ld_postpass: {path}: {exc}", file=sys.stderr)
        return 1
    path.write_text(new)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
