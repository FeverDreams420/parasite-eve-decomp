#!/usr/bin/env python3
"""Build and check JP overlays in parallel. Usage: jp_overlay_check.py [-j N] name... | --all"""
import glob, os, subprocess, sys
from concurrent.futures import ThreadPoolExecutor
args = sys.argv[1:]; jobs = 4
if args[:1] == ["-j"]: jobs = int(args[1]); args = args[2:]
names = sorted(os.path.basename(p)[:-5] for p in glob.glob("configs/JP/overlays/*.yaml")) if args == ["--all"] else args
def run(n):
    r = subprocess.run(["make", "VERSION=JP", f"OVERLAY={n}", "overlay-check"], capture_output=True, text=True, errors="replace")
    ok = f"{n}: OK" in r.stdout
    return n, ok, r.stdout + r.stderr
res = {}
with ThreadPoolExecutor(jobs) as ex:
    for n, ok, out in ex.map(run, names):
        res[n] = ok; os.makedirs("build/JP/logs", exist_ok=True); open(f"build/JP/logs/{n}.log", "w").write(out)
print(sum(res.values()), "of", len(res), "overlays match")
open("build/JP/overlay_status.txt", "w").write("\n".join(f"{n} {'OK' if ok else 'FAIL'}" for n, ok in res.items()) + "\n")
