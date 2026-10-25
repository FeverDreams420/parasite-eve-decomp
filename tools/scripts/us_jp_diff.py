#!/usr/bin/env python3
"""Per-function comparison of the USA main.exe against the JP SLPS_012.30.

Classes: identical (raw bytes), relocated (equal once jump targets and
lui/addiu/load-store immediates are masked), shifted (same function found at
another JP offset by masked-body search), different. Writes a CSV.
Usage: us_jp_diff.py <us main.exe> <jp SLPS_012.30> <nm -S -n output> <out.csv>
"""
import struct, sys, csv, collections

DATA_START = 0x80094528     # USA: file offset 0x84D28 (.data of psyq/libc/sprintf)


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
    index = collections.defaultdict(list)     # masked 2-word window -> offsets
    for i in range(len(jm) - 1):
        index[(jm[i], jm[i + 1])].append(i)
    rows = []; cnt = collections.Counter(); prev_end = 0   # JP code keeps the USA function order
    for line in open(nm):
        a, s, name = line.split()
        addr = int(a, 16); size = int(s, 16)
        if size < 8 or addr < base or addr >= DATA_START: continue   # nm lists data labels as T too
        size &= ~3
        o = addr - base
        ub = U[0x800 + o:0x800 + o + size]; jb = J[0x800 + o:0x800 + o + size]
        if ub == jb: cls = 'identical'; where = addr
        else:
            um = [mask(w) for w in words(ub)]
            if [mask(w) for w in words(jb)] == um: cls = 'relocated'; where = addr
            else:
                cls = 'different'; where = 0
                # several matches (small bodies): take the one nearest the USA offset
                hits = [i for i in index.get((um[0], um[1]), []) if jm[i:i + len(um)] == um]
                if hits:
                    after = [i for i in hits if base + i * 4 >= prev_end]
                    pick = min(after, key=lambda i: i) if after else min(hits, key=lambda i: abs(i * 4 - o))
                    cls = 'shifted'; where = base + pick * 4
        if where: prev_end = where + size
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
