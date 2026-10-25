#!/usr/bin/env python3
"""Locate every USA overlay slice (configs/USA/overlays/*.yaml dd comment) in the JP PE.IMG.

The images are flat and addressed by hard-coded offsets, so each slice moved by a small
delta. Probes of the slice that do not hold relocations vote for the JP offset; the vote
is verified by comparing the masked words of the whole slice.
Usage: jp_overlay_map.py <usa PE.IMG> <jp PE.IMG> <out.csv>
"""
import csv, glob, mmap, os, re, struct, sys
from collections import Counter

U = open(sys.argv[1], "rb"); J = open(sys.argv[2], "rb")
um = mmap.mmap(U.fileno(), 0, access=mmap.ACCESS_READ); jm = mmap.mmap(J.fileno(), 0, access=mmap.ACCESS_READ)
DD = re.compile(r"dd if=\S+ of=\S+/(\S+)\.bin bs=(\d+) skip=(\d+) count=(\d+)")

def mask(w):
    op = w >> 26
    return w & 0xFC000000 if op in (2, 3) else (w & 0xFFFF0000 if 8 <= op <= 15 or 0x20 <= op <= 0x2E else w)

def mwords(b): b = b[:len(b) & ~3]; return [mask(w) for w in struct.unpack("<%dI" % (len(b) // 4), b)]

rows = []
for y in sorted(glob.glob("configs/USA/overlays/*.yaml")):
    m = DD.search(open(y).read())
    if not m: continue
    name, bs, skip, cnt = os.path.basename(y)[:-5], int(m.group(2)), int(m.group(3)), int(m.group(4))
    off, size = bs * skip, bs * cnt
    data = um[off:off + size]
    votes = Counter()
    step = max(64, size // 24)
    for p in range(0, max(size - 48, 1), step):
        probe = data[p:p + 48]
        if len(probe) < 48 or probe.count(0) > 24 or len(set(probe)) < 8: continue
        lo = max(0, off + p - 0x100000); hi = off + p + 0x100000 + 48
        pos = jm.find(probe, lo, hi)
        while pos >= 0 and pos < hi:
            votes[pos - p] += 1
            pos = jm.find(probe, pos + 1, hi)
    best = votes.most_common(1)[0] if votes else (None, 0)
    ratio = 0.0
    if best[0] is not None and best[0] >= 0:
        a, b = mwords(data), mwords(jm[best[0]:best[0] + size])
        ratio = sum(x == y for x, y in zip(a, b)) / max(len(a), 1)
    rows.append((name, off, size, best[0] if best[0] is not None else "", best[1], f"{ratio:.3f}"))
with open(sys.argv[3], "w", newline="") as f:
    w = csv.writer(f); w.writerow(["name", "usa_off", "size", "jp_off", "votes", "masked_ratio"]); w.writerows(rows)
print(len(rows), "slices;", sum(1 for r in rows if r[3] != "" and float(r[5]) > 0.95), "with masked ratio > 0.95;", sum(1 for r in rows if r[3] == ""), "not located")
