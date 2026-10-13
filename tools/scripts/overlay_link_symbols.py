#!/usr/bin/env python3
"""Generate the one symbol script an overlay links against.

Every symbol the overlay's linked objects use but do not define is resolved,
by name, in this order:

1. `linkers/USA/overlay_abi.txt`: addresses fixed by the disc loader and the
   other overlays (load windows, runtime buffers, cross-overlay entry points).
   They stay pinned on purpose.
2. The freshly built main executable (`build/USA/main.elf`): the address main
   gives the name now, so when main grows or moves the overlay follows. A
   main symbol inside an overlay load window is not taken from main: that
   address belongs to the loader contract, not to main's layout.
3. The overlay's own address catalogue (splat's undefined_*_auto files, the
   overlay's symbol file and address-named labels such as `D_8019xxxx`),
   only for addresses inside the overlay's own load window, the hardware
   registers or the kernel. Those are the overlay's own layout.

Anything left over (a main address that main does not name, a name main does
not define, an address in somebody else's window that the ABI file does not
list) fails the build: give main the symbol, rename the reference to the name
main defines, or list the address in the ABI file with its owner.
"""

from __future__ import annotations

import argparse
import re
import subprocess
import sys
from pathlib import Path


NM = "mipsel-none-elf-nm"
OBJDUMP = "mipsel-none-elf-objdump"
UNDEFINED_RE = re.compile(r"^\s*U\s+(\S+)$")
RELOCATION_RE = re.compile(r"^[0-9A-Fa-f]+\s+R_MIPS_\S+\s+(\S+)$")
SYMBOL_ADDR_RE = re.compile(
    r"^(?:"
    r"\.L(?P<label>[0-9A-Fa-f]{8})"
    r"|func_(?P<func>[0-9A-Fa-f]{8})"
    r"|D_(?P<data>[0-9A-Fa-f]{8})"
    r"|jlabel_(?P<jlabel>[0-9A-Fa-f]{8})"
    r"|jtbl_(?P<jtbl>[0-9A-Fa-f]{8})"
    r"|jpt_(?P<jpt>[0-9A-Fa-f]{8})"
    r")$"
)
ASSIGN_RE = re.compile(
    r"^\s*(?://\s*provide:\s*)?(?:PROVIDE\s*\(\s*)?([A-Za-z_.$][\w.$]*)\s*=\s*(0x[0-9A-Fa-f]+)\s*\)?\s*;")
SEGMENT_RE = re.compile(
    r"^  - name: \S+\n(?:    .*\n)*?    start: (0x[0-9A-Fa-f]+)\n(?:    .*\n)*?    vram: (0x[0-9A-Fa-f]+)",
    re.M)
END_RE = re.compile(r"^  - \[(0x[0-9A-Fa-f]+)\]\s*$", re.M)
TARGET_RE = re.compile(r"^  target_path: *(\S+)", re.M)


def undefined_symbols(obj: Path) -> set[str]:
    proc = subprocess.run([NM, "-u", str(obj)], check=True, text=True,
                          stdout=subprocess.PIPE)
    symbols: set[str] = set()
    for line in proc.stdout.splitlines():
        match = UNDEFINED_RE.match(line)
        if match:
            symbols.add(match.group(1))
    relocs = subprocess.run([OBJDUMP, "-r", str(obj)], check=True, text=True,
                            stdout=subprocess.PIPE)
    for line in relocs.stdout.splitlines():
        match = RELOCATION_RE.match(line.strip())
        if match:
            symbols.add(match.group(1))
    return symbols


def defined_symbols(obj: Path) -> set[str]:
    proc = subprocess.run([NM, "--defined-only", "-g", str(obj)], check=True,
                          text=True, stdout=subprocess.PIPE)
    return {line.split()[-1] for line in proc.stdout.splitlines()
            if len(line.split()) == 3}


def linked_objects(ld_script: Path, build_dir: Path) -> list[Path]:
    """The objects the generated linker script links (stale ones excluded)."""
    prefix = str(build_dir).rstrip("/") + "/"
    found = re.findall(re.escape(prefix) + r"[^\s()]+\.o", ld_script.read_text())
    return sorted({Path(path) for path in found})


def read_assignments(path: Path) -> dict[str, int]:
    """name -> address from a symbol file or linker script (first wins)."""
    found: dict[str, int] = {}
    if not path.exists():
        return found
    text = re.sub(r"/\*.*?\*/", "", path.read_text(), flags=re.S)
    for line in text.splitlines():
        match = ASSIGN_RE.match(line)
        if match:
            found.setdefault(match.group(1), int(match.group(2), 16))
    return found


def main_symbols(elf: Path) -> dict[str, int]:
    """Global symbols the built main executable defines, by name."""
    proc = subprocess.run([NM, "-g", "--defined-only", str(elf)], check=True,
                          text=True, stdout=subprocess.PIPE)
    found: dict[str, int] = {}
    for line in proc.stdout.splitlines():
        parts = line.split()
        if len(parts) == 3:
            found[parts[2]] = int(parts[0], 16)
    return found


def config_windows(config: Path) -> list[tuple[int, int]]:
    """[vram, vram + size) of each segment of one overlay config."""
    text = config.read_text()
    segments = [(int(start, 16), int(vram, 16)) for start, vram in SEGMENT_RE.findall(text)]
    ends = END_RE.findall(text)
    end = int(ends[-1], 16) if ends else None
    if end is None:
        target = TARGET_RE.search(text)
        if target and Path(target.group(1)).exists():
            end = Path(target.group(1)).stat().st_size
    windows = []
    for i, (start, vram) in enumerate(segments):
        stop = segments[i + 1][0] if i + 1 < len(segments) else end
        if stop is not None:
            windows.append((vram, vram + stop - start))
    return windows


def symbol_address(symbol: str) -> int | None:
    match = SYMBOL_ADDR_RE.match(symbol)
    if match is None:
        return None
    value = next(group for group in match.groups() if group is not None)
    return int(value, 16)


def outside_program(addr: int) -> bool:
    """Hardware registers, the scratchpad, kernel/BIOS RAM: not program layout."""
    low = addr & 0x1FFFFFFF
    return low < 0x10000 or low >= 0x00200000


def inside(addr: int, windows) -> bool:
    return any(lo <= addr < hi for lo, hi in windows)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("build_dir", type=Path)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--ld", type=Path, required=True,
                        help="generated linker script: only its objects are "
                             "read, and symbols they define get no entry")
    parser.add_argument("--config", type=Path, required=True,
                        help="the overlay's splat config (its own load window)")
    parser.add_argument("--configs", type=Path, required=True,
                        help="directory of every overlay config (all windows)")
    parser.add_argument("--main-elf", type=Path, required=True)
    parser.add_argument("--abi", type=Path, required=True,
                        help="loader and cross-overlay ABI addresses")
    parser.add_argument("--catalogue", type=Path, action="append", default=[],
                        help="address catalogue for the overlay's own window "
                             "(splat auto files, the overlay symbol file)")
    parser.add_argument("--pin-defined", action="store_true",
                        help="also resolve names linked objects define (overlays "
                             "whose copied code keeps foreign addresses)")
    args = parser.parse_args()

    if not args.main_elf.exists():
        sys.exit(f"{args.main_elf} is missing: build main first (make build)")

    symbols: set[str] = set()
    defined: set[str] = set()
    for obj in linked_objects(args.ld, args.build_dir):
        symbols.update(undefined_symbols(obj))
        if not args.pin_defined:
            defined.update(defined_symbols(obj))

    abi = read_assignments(args.abi)
    main_syms = main_symbols(args.main_elf)
    own = config_windows(args.config)
    every_window = [w for cfg in sorted(args.configs.glob("*.yaml")) for w in config_windows(cfg)]
    catalogue: dict[str, int] = {}
    for path in args.catalogue:
        for name, addr in read_assignments(path).items():
            catalogue.setdefault(name, addr)

    from_abi, from_main, from_own, missing = [], [], [], []
    for symbol in sorted(symbols - defined):
        if symbol in abi:
            from_abi.append((symbol, abi[symbol]))
            continue
        addr = main_syms.get(symbol)
        if addr is not None and not inside(addr, every_window):
            from_main.append((symbol, addr))
            continue
        addr = symbol_address(symbol)
        if addr is None:
            addr = catalogue.get(symbol)
        if addr is None:
            # A local label or a name nothing catalogues: ld reports it.
            continue
        if inside(addr, own) or outside_program(addr):
            from_own.append((symbol, addr))
        else:
            missing.append((symbol, addr))

    if missing:
        for symbol, addr in missing:
            print(f"{args.config.stem}: {symbol} (0x{addr:08X}) is neither a main "
                  f"symbol nor in {args.abi}", file=sys.stderr)
        return 1

    lines = ["/* Generated by tools/scripts/overlay_link_symbols.py. */"]
    lines.append("/* From the built main executable. */")
    lines += [f"{name} = 0x{addr:08X};" for name, addr in from_main]
    lines.append("/* Loader and cross-overlay ABI (linkers/USA/overlay_abi.txt). */")
    lines += [f"{name} = 0x{addr:08X};" for name, addr in from_abi]
    lines.append("/* The overlay's own window, hardware and kernel. */")
    lines += [f"{name} = 0x{addr:08X};" for name, addr in from_own]
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text("\n".join(lines) + "\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
