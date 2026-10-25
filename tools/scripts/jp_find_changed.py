#!/usr/bin/env python3
"""Place the functions the exact search could not (class `different`) in JP.

Masked word pairs of the USA body vote for a JP offset; the winner is accepted when it holds
a large share of the pairs. Results go to configs/JP/function_overrides.csv (existing lines kept).
Usage: jp_find_changed.py <us exe> <jp exe>
"""
import csv, struct, sys, collections

U, J = open(sys.argv[1], "rb").read(), open(sys.argv[2], "rb").read()
def mask(w):
    op = w >> 26
    return w & 0xFC000000 if op in (2, 3) else (w & 0xFFFF0000 if 8 <= op <= 15 or 0x20 <= op <= 0x2E else w)
jm = [mask(w) for w in struct.unpack("<%dI" % ((len(J) - 0x800) // 4), J[0x800:0x800 + ((len(J) - 0x800) // 4) * 4])]
idx = collections.defaultdict(list)
for i in range(len(jm) - 1): idx[(jm[i], jm[i + 1])].append(i)
have = {l.split(",")[0] for l in open("configs/JP/function_overrides.csv") if l.strip() and not l.startswith("#")}
out = []
for r in csv.DictReader(open("docs/us_jp_function_map.csv")):
    if r["class"] != "different" or r["name"] in have or int(r["size"]) < 32: continue
    a, n = int(r["address"], 16), int(r["size"]) // 4
    o = (a - 0x80010000) // 4
    um = [mask(w) for w in struct.unpack_from("<%dI" % n, U, 0x800 + 4 * o)]
    votes = collections.Counter()
    for k in range(n - 1):
        for j in idx.get((um[k], um[k + 1]), []):
            if abs(j - o) < 0x2000: votes[j - k] += 1
    if not votes: continue
    start, v = votes.most_common(1)[0]
    if v >= max(4, (n - 1) // 4): out.append((r["name"], 0x80010000 + 4 * start, v, n))
with open("configs/JP/function_overrides.csv", "a") as f:
    for name, addr, v, n in out: f.write(f"{name},0x{addr:08X}\n")
print(len(out), "functions placed")
for name, addr, v, n in out[:5]: print(name, hex(addr), v, n)
