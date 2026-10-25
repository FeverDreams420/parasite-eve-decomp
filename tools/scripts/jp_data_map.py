#!/usr/bin/env python3
"""Derive USA->JP data addresses by pairing the operands of matched functions.

For every matched function in docs/us_jp_function_map.csv the USA and JP bodies
have the same word layout; wherever a lui/%lo pair or a $gp-relative access
differs, the two computed addresses name the same object in each build. Votes are
counted per USA address and the winner is kept.
Usage: jp_data_map.py <us main.exe> <jp exe> <out sym file>
"""
import csv, re, struct, sys
from collections import Counter, defaultdict

BASE, GP_US, GP_JP = 0x80010000, 0x8009CD70, 0x8009C838
MEM = set(range(0x20, 0x2F)) | {0x31, 0x39, 0x32, 0x3A}
s16 = lambda v: v - 0x10000 if v & 0x8000 else v


def scan(d, a, n, gp):
    word = lambda addr: struct.unpack_from("<I", d, addr - BASE + 0x800)[0]
    out, hi = [], {}
    for i in range(n):
        x = word(a + 4 * i); op = x >> 26; rs = (x >> 21) & 31; rt = (x >> 16) & 31; im = x & 0xFFFF
        addr = None
        if op == 0x0F:
            hi[rt] = im << 16
        elif op in MEM or op in (0x09, 0x0D):
            if rs == 28 and op in MEM: addr = (gp + s16(im)) & 0xFFFFFFFF
            elif rs in hi: addr = (hi[rs] + (im if op == 0x0D else s16(im))) & 0xFFFFFFFF
        out.append(addr)
        rd = (x >> 11) & 31
        if op == 0 and (x & 0x3F) == 0x21 and (rs in hi or rt in hi):           # addu base, idx: keeps the hi part
            hi[rd] = hi[rs] if rs in hi else hi[rt]
        elif op == 0 and rd in hi: hi.pop(rd)                                   # R-type rd overwritten
        elif op in (0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E) or 0x20 <= op <= 0x26:
            if op != 0x0F and rt in hi and not (rs == rt and op in (0x09, 0x0D, 0x08)): hi.pop(rt)
        if op in (2, 3, 4, 5, 6, 7) or x == 0x03E00008: hi = {} if op in (2, 3) or x == 0x03E00008 else hi
    return out


def compute(usexe, jpexe):
    us, jp = open(usexe, "rb").read(), open(jpexe, "rb").read()
    votes = defaultdict(Counter)
    for r in csv.DictReader(open("docs/us_jp_function_map.csv")):
        if r["class"] == "different" or not r["jp_address"]: continue
        a, ja, n = int(r["address"], 16), int(r["jp_address"], 16), int(r["size"]) // 4
        try: A, B = scan(us, a, n, GP_US), scan(jp, ja, n, GP_JP)
        except struct.error: continue
        for p, q in zip(A, B):
            if p is not None and q is not None and p >= BASE: votes[p][q] += 1
    return {p: c.most_common(1)[0][0] for p, c in votes.items()}


def main():
    best = compute(sys.argv[1], sys.argv[2])
    out, seen = ["// JP data symbols derived from operand pairs of matched functions."], set()
    for f in ("configs/USA/sym.main.txt", "configs/USA/sym.main_only.txt"):
        for l in open(f):
            m = re.match(r"\s*(\w+)\s*=\s*0x([0-9A-Fa-f]+);", l)
            if m and int(m.group(2), 16) in best and m.group(1) not in seen:
                seen.add(m.group(1)); out.append(f"{m.group(1)} = 0x{best[int(m.group(2), 16)]:08X};")
    open(sys.argv[3], "w").write("\n".join(out) + "\n")
    print(len(out) - 1, "symbols")


if __name__ == "__main__":
    main()
