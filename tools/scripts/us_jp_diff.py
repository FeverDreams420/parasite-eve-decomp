#!/usr/bin/env python3
"""Per-function comparison of the USA main.exe against the JP SLPS_012.30.

Classes: identical (raw bytes), relocated (equal once jump targets and
lui/addiu/load-store immediates are masked), shifted (same function found at
another JP offset by masked-body search), different. Writes a CSV.
Usage: us_jp_diff.py <us main.exe> <jp SLPS_012.30> <nm -S -n output> <out.csv>
"""
import struct, sys, csv, collections

def words(b):
    b = b[:len(b) & ~3]
    return struct.unpack('<%dI' % (len(b) // 4), b)

def mask(w):
    op = w >> 26
    if op in (2, 3):                      # j / jal
        return w & 0xFC000000
    if 8 <= op <= 0x0F or 0x20 <= op <= 0x2E:
        return w & 0xFFFF0000             # lui/addi*/slti*/andi/ori/xori, loads/stores
    return w

def main():
    us, jp, nm, out = sys.argv[1:5]
    U = open(us, 'rb').read(); J = open(jp, 'rb').read()
    base = 0x80010000
    jm = [mask(w) for w in words(J[0x800:])]
    index = collections.defaultdict(list)     # masked 4-word window -> offsets
    for i in range(len(jm) - 3):
        index[tuple(jm[i:i + 4])].append(i)
    rows = []; cnt = collections.Counter()
    for line in open(nm):
        a, s, name = line.split()
        addr = int(a, 16); size = int(s, 16)
        if size < 8 or addr < base: continue
        size &= ~3
        o = addr - base
        ub = U[0x800 + o:0x800 + o + size]; jb = J[0x800 + o:0x800 + o + size]
        if ub == jb: cls = 'identical'; where = addr
        else:
            um = [mask(w) for w in words(ub)]
            if [mask(w) for w in words(jb)] == um: cls = 'relocated'; where = addr
            else:
                cls = 'different'; where = 0
                if len(um) >= 8:
                    for i in index.get(tuple(um[:4]), []):
                        if jm[i:i + len(um)] == um:
                            cls = 'shifted'; where = base + i * 4; break
        cnt[cls] += 1
        rows.append((hex(addr), name, size, cls, hex(where) if where else ''))
    with open(out, 'w', newline='') as f:
        w = csv.writer(f); w.writerow(['address', 'name', 'size', 'class', 'jp_address']); w.writerows(rows)
    tot = sum(cnt.values())
    for k in ('identical', 'relocated', 'shifted', 'different'):
        print('%-10s %5d  (%.1f%%)' % (k, cnt[k], 100.0 * cnt[k] / tot))
    byte = collections.Counter()
    for r in rows: byte[r[3]] += r[2]
    print('bytes', dict(byte))
main()
