#!/usr/bin/env python3
"""Turn the C units of a JP overlay that differ from the JP image into asm and rebuild.
Usage: jp_overlay_demote.py <name>"""
import re, subprocess, sys
n = sys.argv[1]
cfg = f"configs/JP/overlays/{n}.yaml"
for it in range(8):
    r = subprocess.run(["make", "VERSION=JP", f"OVERLAY={n}", "overlay-check"], capture_output=True, text=True, errors="replace")
    if f"{n}: OK" in r.stdout: print(n, "matches after", it, "demotions"); break
    t = open(cfg).read()
    m = re.search(r"target_path: (\S+)", t); orig = open(m.group(1), "rb").read()
    try: built = open(f"build/JP/overlays/{n}/{n}.bin", "rb").read()
    except FileNotFoundError: print(n, "did not build"); sys.exit(1)
    diff = [i for i in range(0, min(len(orig), len(built)), 4) if orig[i:i+4] != built[i:i+4]]
    segs = [(int(a, 16), k, name) for a, k, name in re.findall(r"- \[(0x[0-9A-Fa-f]+), (c|asm), ([\w./]+)\]", t)]
    hit = {s for s in segs if s[1] == "c" and any(s[0] <= d < next((x[0] for x in segs if x[0] > s[0]), len(orig)) for d in diff)}
    if not hit: print(n, "differences outside C units", [hex(d) for d in diff[:5]]); sys.exit(1)
    for off, _, name in hit: t = t.replace(f"[0x{off:X}, c, {name}]", f"[0x{off:X}, asm, {name}]")
    open(cfg, "w").write(t); print(n, "demoted", sorted(h[2] for h in hit))
