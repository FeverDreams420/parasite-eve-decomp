#!/usr/bin/env python3
"""Copy a USA C source to src/jp/main and retarget its address-named symbols.

`func_XXXXXXXX` and `D_XXXXXXXX` names encode USA addresses; the JP build has other
objects at those addresses. Each token is rewritten to the JP address of the same
function (docs/us_jp_function_map.csv) or datum (jp_data_map.py). Unmapped tokens are
reported and left alone.
Usage: jp_port_c.py [--overlay] <us exe> <jp exe> <src/main/rel.c|src/overlays/...>...\n--overlay: sources of an overlay; only addresses of main code and data are retargeted.
"""
import csv, importlib.util, re, shutil, sys, os
OVERLAY = "--overlay" in sys.argv
if OVERLAY: sys.argv.remove("--overlay")

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
HEADERS = [open(os.path.join(r, f), encoding='latin-1').read().splitlines() for d0 in ('include', 'src/main', 'src/overlays') for r, _, fs in os.walk(d0) for f in fs if f.endswith(('.h', '.inc'))]
import json
AMAP = {int(k): v for k, v in json.load(open(os.environ['JP_AMAP'])).items()} if os.environ.get('JP_AMAP') else {}
PORTED = set()
tok = re.compile(r"\b(func|D|jtbl|jlabel)_([0-9A-F]{8})\b")

for rel in sys.argv[3:]:
    t = open(rel, encoding="latin-1").read(); miss = []
    def sub(m):
        a = int(m.group(2), 16)
        ja = OVR.get(m.group(0)) or (fmap.get(a) if m.group(1) == "func" else best.get(a))
        if ja is None and m.group(1) == "func" and not OVERLAY: ja = search(a)
        if OVERLAY and ja is None and a in AMAP: ja = AMAP[a]
        if OVERLAY and a >= 0x8010B000 and ja is None: return m.group(0)
        if ja is None: miss.append(m.group(0)); return m.group(0)
        if m.group(1) == "D" and ja < 0x8010B000: PORTED.add(f"D_{ja:08X} = 0x{ja:08X};")
        return f"{m.group(1)}_{ja:08X}"
    out = tok.sub(sub, t)
    # declarations of retargeted names live in shared headers: re-declare them under the JP name
    olds = {m.group(0) for m in tok.finditer(t)} - {m.group(0) for m in tok.finditer(out)}
    extra = []
    for old in sorted(olds):
        new = tok.sub(sub, old)
        if re.search(r"^(extern\s+)?[A-Za-z_][\w\s\*]*\b%s\b(\[[^\]]*\])*\s*(=[^;]*)?;" % old, t, re.M): continue   # declared here
        for h in HEADERS:
            for ln in h:
                if ln.startswith("extern") and re.search(r"\b%s\b" % old, ln) and re.sub(r"/\*.*?\*/", "", ln).rstrip().endswith(";"):
                    extra.append(re.sub(r"/\*.*?\*/", "", ln).replace(old, new).rstrip()); break
            else: continue
            break
    for decl in dict.fromkeys(extra):               # declare each name after the last #include before its first use
        name = tok.search(decl).group(0)
        first = out.find(name)
        inc = [m.end() for m in re.finditer(r"^#include.*\n", out, re.M) if first < 0 or m.end() <= first]
        pos = inc[-1] if inc else 0
        out = out[:pos] + decl + "\n" + out[pos:]
    dst = rel.replace("src/main/", "src/jp/main/", 1).replace("src/overlays/", "src/jp/overlays/", 1)
    os.makedirs(os.path.dirname(dst), exist_ok=True)
    open(dst, "w", encoding="latin-1").write(out)
    print(dst, "unmapped:", sorted(set(miss)) or "-")

if PORTED:                                  # main data names the ported sources now use (no asm label defines them)
    os.makedirs("build/JP", exist_ok=True)
    f = "build/JP/ported_names.txt"
    old = set(open(f).read().split("\n")) if os.path.exists(f) else set()
    open(f, "w").write("\n".join(sorted((old | PORTED) - {""})) + "\n")
