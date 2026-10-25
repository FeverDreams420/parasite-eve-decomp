#!/usr/bin/env python3
"""Copy a USA C source to src/jp/main and retarget its address-named symbols.

`func_XXXXXXXX` and `D_XXXXXXXX` names encode USA addresses; the JP build has other
objects at those addresses. Each token is rewritten to the JP address of the same
function (docs/us_jp_function_map.csv) or datum (jp_data_map.py). Unmapped tokens are
reported and left alone.
Usage: jp_port_c.py <us exe> <jp exe> <src/main/rel.c>...
"""
import csv, importlib.util, re, shutil, sys, os

spec = importlib.util.spec_from_file_location("jdm", os.path.join(os.path.dirname(__file__), "jp_data_map.py"))
jdm = importlib.util.module_from_spec(spec); spec.loader.exec_module(jdm)
best = jdm.compute(sys.argv[1], sys.argv[2])
SIZE = {int(r["address"], 16): int(r["size"]) for r in csv.DictReader(open("docs/us_jp_function_map.csv"))}
fmap = {int(r["address"], 16): int(r["jp_address"], 16) for r in csv.DictReader(open("docs/us_jp_function_map.csv")) if r["jp_address"]}
US, JP = open(sys.argv[1], "rb").read(), open(sys.argv[2], "rb").read()
dspec = importlib.util.spec_from_file_location("dif", os.path.join(os.path.dirname(__file__), "us_jp_diff.py"))
src = open(os.path.join(os.path.dirname(__file__), "us_jp_diff.py")).read().split("def main")[0]
ns = {}; exec(src, ns); words, mask = ns["words"], ns["mask"]
JM = [mask(w) for w in words(JP[0x800:])]


def search(a, n=24):
    """Locate an unsized USA function by its first n masked words in JP; nearest to the USA offset wins."""
    o = a - 0x80010000
    srt = sorted(fmap.items()); import bisect
    k = bisect.bisect_left(srt, (a, 0))
    while k > 0 and srt[k - 1][0] - 0 > a: k -= 1
    um = [mask(w) for w in words(US[0x800 + o:0x800 + o + 4 * n])]
    hits = [i for i in range(max(0, o // 4 - 0x800), min(len(JM) - n, o // 4 + 0x800)) if JM[i:i + n] == um]
    big = [x for x in srt[:k] if SIZE.get(x[0], 0) >= 64]
    ref = (big[-1][1] - big[-1][0]) if big else 0       # shift of the preceding sizeable function
    return 0x80010000 + 4 * min(hits, key=lambda i: abs((i * 4 - o) - ref)) if hits else None


OVR = {l.split(',')[0]: int(l.split(',')[1], 16) for l in open('configs/JP/function_overrides.csv') if l.strip() and not l.startswith('#')}
tok = re.compile(r"\b(func|D|jtbl|jlabel)_([0-9A-F]{8})\b")

for rel in sys.argv[3:]:
    t = open(rel).read(); miss = []
    def sub(m):
        a = int(m.group(2), 16)
        ja = OVR.get(m.group(0)) or (fmap.get(a) if m.group(1) == "func" else best.get(a))
        if ja is None and m.group(1) == "func": ja = search(a)
        if ja is None: miss.append(m.group(0)); return m.group(0)
        return f"{m.group(1)}_{ja:08X}"
    out = tok.sub(sub, t)
    dst = rel.replace("src/main/", "src/jp/main/", 1)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    open(dst, "w").write(out)
    print(dst, "unmapped:", sorted(set(miss)) or "-")
