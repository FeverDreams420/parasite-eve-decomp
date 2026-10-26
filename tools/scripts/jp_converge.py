#!/usr/bin/env python3
"""Convert USA C translation units to the JP target and keep only those that match.

1. every USA `c` text subsegment without a function that changed in JP is a candidate;
2. its source is copied to src-jp/main with address-named symbols retargeted
   (jp_port_c.py), and a JP configs/JP/main.yaml is generated around the candidates;
3. the JP exe is built and compared with SLPS_012.30; TUs that contain a differing
   word, or that emit their own .data/.bss, go back to asm; repeat until stable.

Usage: jp_converge.py <us exe> <jp exe> [--report out.csv]
State: configs/JP/converted.txt (the TUs currently built from C).
"""
import csv, os, re, struct, subprocess, sys

US, JP = sys.argv[1], sys.argv[2]
REPORT = sys.argv[sys.argv.index("--report") + 1] if "--report" in sys.argv else None
BASE, TEXT0, DATA0, END = 0x80010000, 0x2A0C, 0x84D28, 0x1EE800
env = dict(os.environ)

rows = list(csv.DictReader(open("docs/us_jp_function_map.csv")))
mapped = sorted((int(r["address"], 16), int(r["jp_address"], 16)) for r in rows if r["class"] != "different" and r["jp_address"])
different = [int(r["address"], 16) for r in rows if r["class"] == "different"]

ent = []                                   # (offset, type, name) from the USA yaml
for l in open("configs/USA/main.yaml"):
    m = re.match(r"\s*- \[(0x[0-9A-Fa-f]+),?\s*([^,\]]*),?\s*([^\]]*)\]", l)
    if m: ent.append((int(m.group(1), 16), m.group(2).strip(), m.group(3).strip()))
data_names = {n for o, t, n in ent if o >= DATA0 and n}
ends = [e[0] for e in ent[1:]] + [END]

def vram(o): return BASE + o - 0x800
tus = []                                   # candidate C TUs
for (o, t, n), e in zip(ent, ends):
    if t != "c" or not (TEXT0 <= o < DATA0 or 0xB2AF8 <= o < 0xD0860): continue
    a, b = vram(o), vram(e)
    patched = os.path.exists(f"configs/JP/patches/{n}.patch")      # hand-edited for JP: the changed functions are covered
    if (any(a <= d < b for d in different) and not patched) or n in data_names: continue
    fm = [(f, j) for f, j in mapped if a <= f < b]
    if not fm: continue
    f, j = fm[0]
    tus.append({"name": n, "usa_off": o, "len": e - o, "jp_off": (j - (f - a)) - BASE + 0x800})

conv_file = "configs/JP/converted.txt"
if os.path.exists(conv_file): conv = {l.strip() for l in open(conv_file) if l.strip()}
else: conv = {t["name"] for t in tus}
by_name = {t["name"]: t for t in tus}

def gen_yaml(conv):
    head = open("configs/JP/main.yaml").read().split("    subsegments:")[0]
    lines = []
    for (o, t, n), e in zip(ent, ends):
        if o >= TEXT0: break
        if t == ".rodata" and n in conv: lines.append(f"      - [0x{o:X}, .rodata, {n}]")
        elif t in (".rodata", "rodata"): lines.append(f"      - [0x{o:X}, rodata, jp/ro_{o:X}]")
        elif t == "pad": lines.append(f"      - [0x{o:X}, pad]")
    pos = TEXT0
    pieces = sorted((by_name[n] for n in conv), key=lambda t: t["jp_off"])
    for t in pieces:
        if t["jp_off"] > pos: lines.append(f"      - [0x{pos:X}, asm, jp/text_{pos:X}]")
        lines.append(f"      - [0x{t['jp_off']:X}, c, {t['name']}]")
        pos = t["jp_off"] + t["len"]
    lines.append(f"      - [0x{pos:X}, asm, jp/text_{pos:X}]")
    open("configs/JP/main.yaml", "w").write(head + "    subsegments:\n" + "\n".join(lines) + f"\n  - [0x{END:X}]\n")
    return pieces

def build():
    subprocess.run(["rm", "-rf", "build/JP/src", "build/JP/asm"])
    r = subprocess.run(["make", "VERSION=JP", "split"], capture_output=True, text=True, errors="replace")
    if r.returncode: print(r.stdout[-2000:], r.stderr[-2000:]); sys.exit(1)
    r = subprocess.run(["make", "-k", "-j8", "VERSION=JP", "build"], capture_output=True, text=True, errors="replace")
    return r

os.makedirs("src-jp/main", exist_ok=True)
def port(n):
    subprocess.run([sys.executable, "tools/scripts/jp_port_c.py", US, JP, f"src/main/{n}.c"], capture_output=True)
    pf = f"configs/JP/patches/{n}.patch"
    if os.path.exists(pf): subprocess.run(["patch", "-s", f"src-jp/main/{n}.c", pf], check=True)


for n in sorted(conv): port(n)

for it in range(12):
    pieces = gen_yaml(conv)
    for root, _, fs in os.walk("src-jp/main"):          # make globs every .c: keep only the converted ones
        for f in fs:
            rel = os.path.relpath(os.path.join(root, f), "src-jp/main")
            if f.endswith(".c") and rel[:-2] not in conv: os.remove(os.path.join(root, f))
    r = build()
    out = r.stdout + r.stderr
    bad = set(re.findall(r"\*\*\* \[build/JP/src-jp/main/([\w/]+)\.c\.o\]", out))
    bad |= set(re.findall(r"build/JP/src-jp/main/([\w/]+)\.c\.o: in function", out))
    cur = None
    for ln in out.splitlines():              # ld prints the owning object once per run of errors
        m = re.search(r"build/JP/src-jp/main/([\w/]+)\.c\.o: in function", ln)
        if m: cur = m.group(1)
        elif "undefined reference" in ln and cur: bad.add(cur)
    for sym in set(re.findall(r"undefined reference to `(\w+)'", out)):      # no owner printed: grep the sources
        for n in list(conv):
            if re.search(r"\b%s\b" % sym, open(f"src-jp/main/{n}.c", encoding="latin-1").read()): bad.add(n)
    if bad & conv:
        print(f"iter {it}: {len(bad & conv)} TUs fail to compile/link", sorted(bad & conv)[:5]); conv -= bad; continue
    if r.returncode:
        print("\n".join(l for l in out.splitlines() if "rror" in l or "undefined" in l)[-3000:]); break
    resized = False
    wrong = set()                          # a TU whose code size differs shifts everything after it
    for tu in pieces:
        o = f"build/JP/src-jp/main/{tu['name']}.c.o"
        if os.path.exists(o):
            sz = subprocess.run(["mipsel-none-elf-size", "-A", o], capture_output=True, text=True).stdout
            m = re.search(r"^\.text\s+(\d+)", sz, re.M)
            if m and int(m.group(1)) != tu["len"]:
                if os.path.exists(f"configs/JP/patches/{tu['name']}.patch") and not tu.get("resized"):
                    tu["len"] = int(m.group(1)); tu["resized"] = True; resized = True      # the patch changed the code size
                else: wrong.add(tu["name"])
    if resized and not wrong: print(f"iter {it}: layout resized for patched units"); continue
    if wrong:
        print(f"iter {it}: {len(wrong)} TUs have a different code size"); conv -= wrong; continue
    a, b = open("build/JP/main.exe", "rb").read(), open("assets/JP/main.exe", "rb").read()
    diff = [i for i in range(0, len(b), 4) if a[i:i + 4] != b[i:i + 4]]
    drop = {t["name"] for t in pieces for i in diff if t["jp_off"] <= i < t["jp_off"] + t["len"]}
    if not drop and len(a) == len(b) and not diff:
        print(f"iter {it}: exe identical, {len(conv)} TUs from C"); break
    print(f"iter {it}: {len(diff)} differing words, dropping {len(drop)} TUs")
    if not drop: print("unattributed diffs at", [hex(vram(i)) for i in diff[:10]]); break
    keep = set(os.environ.get("JP_KEEP", "").split(","))
    if drop & keep:
        print("kept TU differs at", [hex(vram(i)) for i in diff[:8]], "; units", sorted(drop & keep)); break
    conv -= drop
open(conv_file, "w").write("\n".join(sorted(conv)) + "\n")
if REPORT:
    with open(REPORT, "w") as f:
        f.write("tu,status,bytes\n")
        for t in tus: f.write(f"{t['name']},{'c' if t['name'] in conv else 'asm'},{t['len']}\n")
