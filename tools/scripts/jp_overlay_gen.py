#!/usr/bin/env python3
"""Generate configs/JP/overlays/<name>.yaml (+ sym file, ported sources) from the USA ones.

Only overlays whose JP slice has the USA size and layout are handled (masked word ratio in
docs/jp_overlay_map.csv). Window vrams move by the JP window deltas; sources are copied to
src/jp/overlays with main addresses retargeted.
Usage: jp_overlay_gen.py <usa exe> <jp exe> <name>... | --all
"""
import csv, hashlib, os, re, subprocess, sys

US, JP = sys.argv[1], sys.argv[2]
names = sys.argv[3:]
PE = "/Users/hasik/Projects/parasite-pc/disc/jp/extracted/PE.IMG"
rows = {r["name"]: r for r in csv.DictReader(open("docs/jp_overlay_map.csv"))}
if names == ["--all"]:
    names = [n for n, r in rows.items() if float(r["masked_ratio"]) >= 0.95 and os.path.exists(f"configs/USA/overlays/{n}.yaml")]
# JP window deltas (USA window start, delta): see linkers/JP/overlay_windows.txt
DELTAS = [(0x8010BCF8, 0x80120D00, -0x540), (0x80120D00, 0x80122FA4, -0x540), (0x80122FA4, 0x80170000, -0x400)]
def shift(v):
    for lo, hi, d in DELTAS:
        if lo <= v < hi: return v + d
    return v

import struct
from collections import Counter, defaultdict
MEM = set(range(0x20, 0x2F)) | {0x31, 0x39, 0x32, 0x3A}
votes = defaultdict(Counter)


def pair_scan(b, vram):
    """Linear scan of an overlay image: (address, word index) of every lui/%lo, jal and j target."""
    out = {}; hi = {}
    sg = lambda v: v - 0x10000 if v & 0x8000 else v
    for i in range(len(b) // 4):
        x = struct.unpack_from("<I", b, 4 * i)[0]; op = x >> 26; rs = (x >> 21) & 31; rt = (x >> 16) & 31; im = x & 0xFFFF
        if op in (2, 3): out[i] = (x & 0x3FFFFFF) << 2 | 0x80000000
        elif op == 0x0F: hi[rt] = im << 16
        elif op in MEM or op in (0x09, 0x0D):
            if rs in hi: out[i] = (hi[rs] + (im if op == 0x0D else sg(im))) & 0xFFFFFFFF
        if op == 0 and (x & 0x3F) == 0x21 and (rs in hi or ((x >> 16) & 31) in hi): hi[(x >> 11) & 31] = hi[rs] if rs in hi else hi[(x >> 16) & 31]
        if op in (2, 3) or x == 0x03E00008: hi = {}
    return out


def learn(u, j, vram):
    a, b = pair_scan(u, vram), pair_scan(j, vram)
    for i, p in a.items():
        if i in b and p >= 0x80010000: votes[p][b[i]] += 1


def amap(v):
    c = votes.get(v)
    return c.most_common(1)[0][0] if c else shift(v)


def shift_text(s):
    return re.sub(r"0x80[0-9A-Fa-f]{6}", lambda m: "0x%08X" % amap(int(m.group(0), 16)), s)


def shift_text_old(s): return re.sub(r"0x80[0-9A-Fa-f]{6}", lambda m: "0x%08X" % shift(int(m.group(0), 16)), s)


abi = "linkers/USA/overlay_abi.txt"
pe = open(PE, "rb")
us_pe = open("/Users/hasik/Projects/parasite-pc/disc/extracted/PE.IMG", "rb")
names = [n for n in names if os.path.exists(f"configs/USA/overlays/{n}.yaml")]
for n in names:
    r = rows[n]; us_pe.seek(int(r["usa_off"])); pe.seek(int(r["jp_off"]))
    learn(us_pe.read(int(r["size"])), pe.read(int(r["size"])), 0)
    # (only addresses seen in both builds are learnt; the rest falls back to the window delta)
# overlays call main functions that changed in JP: their jal pairs give the true entry points
ovr = {}
for l in open("configs/JP/function_overrides.csv"):
    if l.strip() and not l.startswith("#"): k, v = l.strip().split(","); ovr[k] = v
for r in csv.DictReader(open("docs/us_jp_function_map.csv")):
    a = int(r["address"], 16)
    if r["class"] == "different" and a in votes and votes[a].most_common(1)[0][1] >= 1: ovr[r["name"]] = "0x%08X" % votes[a].most_common(1)[0][0]
with open("configs/JP/function_overrides.csv", "w") as f:
    f.write("# name,jp_address: functions the masked search cannot place; entries refined from overlay call sites\n")
    f.writelines(f"{k},{v}\n" for k, v in ovr.items())
import json
os.makedirs("configs/JP/overlays", exist_ok=True)
os.makedirs("build/JP", exist_ok=True)
json.dump({str(k): c.most_common(1)[0][0] for k, c in votes.items()}, open("build/JP/overlay_amap.json", "w"))
os.environ["JP_AMAP"] = "build/JP/overlay_amap.json"
USA_WIN = {"MDEC_DIAG": 0x8010BCF8, "MENU_MEMCARD": 0x80120D00, "BOOT_DISPLAY": 0x80122FA4, "FX_SECTOR17": 0x80170000, "ROOM": 0x8018EFE8, "TASK_PROGRAM": 0x801ED7F8}
JP_WIN = {k: shift(v) for k, v in USA_WIN.items()}


def abi_text(s):
    def f(m):
        ua = USA_WIN[m.group(1)] + int(m.group(2), 16); ja = amap(ua)
        return "OVERLAY_WINDOW_%s + 0x%X" % (m.group(1), ja - JP_WIN[m.group(1)])
    return re.sub(r"OVERLAY_WINDOW_(\w+) \+ (0x[0-9A-Fa-f]+)", f, shift_text(s))


open("linkers/JP/overlay_abi.txt", "w").write(abi_text(open(abi).read()))
ported = set()
for n in names:
    r = rows[n]; jo, size = int(r["jp_off"]), int(r["size"])
    t = open(f"configs/USA/overlays/{n}.yaml").read()
    pe.seek(jo); data = pe.read(size)
    t = re.sub(r"^sha1: \w+", "sha1: " + hashlib.sha1(data).hexdigest(), t, count=1, flags=re.M)
    t = t.replace("USA", "JP")
    m = re.search(r"bs=(\d+) skip=(\d+) count", t)
    bs = int(m.group(1))
    if jo % bs: print(n, "JP offset not aligned to bs"); continue
    t = t.replace(f"skip={m.group(2)}", f"skip={jo // bs}")
    t = re.sub(r"^(\s+vram: )(0x[0-9A-Fa-f]+)", lambda x: x.group(1) + "0x%08X" % shift(int(x.group(2), 16)), t, flags=re.M)
    sp = re.search(r"src_path: (\S+)", t).group(1)
    t = t.replace(f"src_path: {sp}", "src_path: " + sp.replace("src/overlays", "src/jp/overlays"))
    open(f"configs/JP/overlays/{n}.yaml", "w").write(t)
    sym = f"configs/USA/overlays/sym.{n}.txt"
    if os.path.exists(sym): open(f"configs/JP/overlays/sym.{n}.txt", "w").write(shift_text(open(sym).read()))
    # sources: every .c the config lists (plus shared room_lib units)
    base = sp
    for c in set(re.findall(r"\[0x[0-9A-Fa-f]+, c, ([\w./]+)\]", t)):
        src = os.path.normpath(os.path.join(base, c)) + ".c"
        if src in ported or not os.path.exists(src): continue
        ported.add(src)
        subprocess.run([sys.executable, "tools/scripts/jp_port_c.py", "--overlay", US, JP, src], capture_output=True)
    # companion headers/includes of the overlay source dir
    d = base
    if os.path.isdir(d):
        for f in os.listdir(d):
            if not f.endswith(".c") and os.path.isfile(os.path.join(d, f)):
                dst = os.path.join(d.replace("src/overlays", "src/jp/overlays"), f)
                os.makedirs(os.path.dirname(dst), exist_ok=True)
                open(dst, "wb").write(open(os.path.join(d, f), "rb").read())
import glob
for s in glob.glob("configs/USA/overlays/sym.*.txt"):       # symbol files of overlays that are not generated too
    d = "configs/JP/overlays/" + os.path.basename(s)
    if not os.path.exists(d): open(d, "w").write(shift_text(open(s).read()))
# shared units (room_lib, fx_common...) are referenced from every overlay: port the lot
allc = [os.path.join(r, f) for r, _, fs in os.walk("src/overlays") for f in fs if f.endswith((".c", ".h"))]
subprocess.run([sys.executable, "tools/scripts/jp_port_c.py", "--overlay", US, JP] + allc, capture_output=True)
for r, _, fs in os.walk("src/overlays"):
    for f in fs:
        if not f.endswith((".c", ".h")):
            dst = os.path.join(r.replace("src/overlays", "src/jp/overlays"), f); os.makedirs(os.path.dirname(dst), exist_ok=True)
            open(dst, "wb").write(open(os.path.join(r, f), "rb").read())
print(len(names), "overlays generated")
