#!/usr/bin/env python3
"""Rewrite instructions that GAS expands to several words back into raw words.

splat mis-decodes some data inside .text as 64-bit/COP2 doubleword accesses
(sdc2, ldc2, ...); GAS expands those, which shifts every later address.
Usage: jp_asm_fixup.py <asm.s> [mnemonic ...]
"""
import re, sys

path = sys.argv[1]
mnems = set(sys.argv[2:]) or {"sdc2", "ldc2"}
pat = re.compile(r"^(\s*/\* [0-9A-F]+ [0-9A-F]{8} )([0-9A-F]{8})( \*/)\s+(\w+)\b.*$")
out, n = [], 0
for line in open(path):
    m = pat.match(line.rstrip("\n"))
    if m and m.group(4) in mnems:
        b = bytes.fromhex(m.group(2))
        out.append(f"{m.group(1)}{m.group(2)}{m.group(3)} .word 0x{int.from_bytes(b, 'little'):08X}\n")
        n += 1
    else:
        out.append(line)
open(path, "w").write("".join(out))
print(f"{path}: {n} instructions rewritten as .word")
