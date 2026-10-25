#!/usr/bin/env python3
"""Show the USA and JP disassembly of one function side by side (difflib).
Usage: jp_func_diff.py <us exe> <jp exe> <usa addr> <size> [jp addr]
The JP address defaults to the best sequence-similarity match within +-0x2000 bytes.
"""
import difflib, struct, subprocess, sys, tempfile, os

us, jp = (open(p, "rb").read() for p in sys.argv[1:3])
a, n = int(sys.argv[3], 16), int(sys.argv[4], 0)

def words(d, addr, cnt): return struct.unpack_from("<%dI" % cnt, d, addr - 0x80010000 + 0x800)

def mask(w):
    op = w >> 26
    return w & 0xFC000000 if op in (2, 3) else (w & 0xFFFF0000 if 8 <= op <= 15 or 0x20 <= op <= 0x2E else w)

def dis(d, addr, cnt):
    with tempfile.NamedTemporaryFile(delete=False) as f: f.write(struct.pack("<%dI" % cnt, *words(d, addr, cnt)))
    out = subprocess.run(["mipsel-none-elf-objdump", "-D", "-b", "binary", "-m", "mips:3000", f"--adjust-vma={addr:#x}", f.name], capture_output=True, text=True).stdout
    os.unlink(f.name)
    return [l.split("\t", 2)[-1].strip() if "\t" in l else l for l in out.splitlines()[7:]]

cnt = n // 4
um = [mask(w) for w in words(us, a, cnt)]
if len(sys.argv) > 5: ja = int(sys.argv[5], 16)
else:
    best = (0, 0)
    for d in range(-0x2000, 0x2000, 4):
        try: r = difflib.SequenceMatcher(None, um, [mask(w) for w in words(jp, a + d, cnt)], autojunk=False).ratio()
        except struct.error: continue
        if r > best[0]: best = (r, a + d)
    ja = best[1]; print(f"JP match {ja:#x} ratio {best[0]:.2f}")
for l in difflib.unified_diff(dis(us, a, cnt), dis(jp, ja, cnt), "USA", "JP", lineterm="", n=2): print(l)
