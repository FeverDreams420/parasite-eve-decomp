#!/usr/bin/env python3
"""Merge adjacent C units of ``src/main`` into one translation unit.

    reorg_merge.py NEW.c UNIT.c [UNIT.c ...] [--marker LINE ...]
                   [--common DECL ...] [--dry-run]

``UNIT.c`` are repository paths in manifest order; ``NEW.c`` is the merged
file (it may be one of the units). The units must be consecutive ``c``
entries of ``configs/USA/main.yaml``, and their ``.rodata``/``.data``/
``.sdata`` entries, if any, must also be consecutive (no foreign blob between
them), otherwise the merge is refused.

The merged source is:

* the marker lines (``--marker``; by default the units' own markers, which
  must then agree);
* the union of the units' leading shared ``#include`` lines, in order of
  first appearance;
* ``--common`` declarations (tentative definitions of the ``$gp`` words, see
  ``docs/REORGANIZATION_G_FLAGS.md``);
* each unit's remaining text in manifest order, with its marker lines dropped
  and its includes of private ``*_externs*.h`` headers dropped: their
  declarations move to ``--externs-to HEADER`` (a shared ``include/pe1``
  header), because file-scope ``extern``s in a ``.c`` file count as debt.
  A macro defined by one unit and defined again by a later unit (template
  instantiations) is ``#undef``-ed at the end of the first unit.

The manifest keeps one ``c`` entry (and one entry per data section) at the
first unit's offset, renamed to the new file; the other units' entries go.
Path-qualified mentions of the old units elsewhere (docs, tests, comments)
are rewritten to the new file with ``reorg_move``'s rules. Old files and
their private externs headers are removed with ``git rm``.

Byte identity is not checked here; use ``marker_probe.py`` and the make
gates.
"""
from __future__ import annotations

import argparse
import os
import pathlib
import re
import sys

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent))
import reorg_move as rm  # noqa: E402

ROOT = rm.ROOT
MANIFEST = "configs/USA/main.yaml"
MARKER = re.compile(
    r"^\s*/\*\s*(CC1_FLAGS|MASPSX_FLAGS|MASPSX_FORCE_G0|ASPSX_VERSION|"
    r"GCC_VERSION|GAS_VERSION|ASSEMBLER)\s*:.*\*/\s*$")
DEFINE = re.compile(r"^\s*#\s*define\s+(\w+)")
UNDEF = re.compile(r"^\s*#\s*undef\s+(\w+)")
GUARD = re.compile(r"^\s*#\s*(ifndef|define|endif)\b")


def unit_name(path: str) -> str:
    return path[len("src/main/"):-2]


def split_unit(path: str) -> tuple[list[str], list[str], list[str], list[str]]:
    """(markers, hoisted includes, body lines, inlined headers) of one unit."""
    text = (ROOT / path).read_text()
    markers, includes, body, inlined = [], [], [], []
    in_preamble = True
    in_comment = False
    for line in text.splitlines():
        stripped = line.strip()
        if MARKER.match(line):
            markers.append(stripped)
            continue
        match = rm.INCLUDE.match(line)
        if match:
            target = rm.norm(os.path.join(os.path.dirname(path), match["path"]))
            local = (ROOT / target).is_file()
            if local and re.search(r"_externs(_\d+)?\.h$", target):
                inlined.append(target)
                continue
            if in_preamble and not local:
                includes.append(stripped)
                continue
            body.append(line)
            continue
        if in_preamble and not in_comment:
            if stripped and not stripped.startswith(("/*", "//")):
                in_preamble = False
        if stripped.startswith("/*") and "*/" not in stripped:
            in_comment = True
        elif in_comment and "*/" in stripped:
            in_comment = False
        body.append(line)
    while body and not body[0].strip():
        body.pop(0)
    while body and not body[-1].strip():
        body.pop()
    return markers, includes, body, inlined


TENTATIVE = re.compile(r"^(?!extern\b|static\b|typedef\b|return\b)[A-Za-z_][\w\s*]*"
                       r"\b\w+(\[\w*\])?;\s*$")
COMMON_NOTE = ("/* COMMON supplies small-data metadata to MASPSX: these words are "
               "reached\n * through $gp, every other extern stays absolute. */")


def hoist_commons(body: list[str]) -> tuple[list[str], list[str]]:
    """Take the `/* COMMON ... */` blocks and their tentative definitions out."""
    out, commons = [], []
    index = 0
    while index < len(body):
        line = body[index]
        if line.lstrip().startswith("/* COMMON"):
            end = index
            while "*/" not in body[end]:
                end += 1
            defs = end + 1
            while defs < len(body) and TENTATIVE.match(body[defs]):
                commons.append(body[defs].strip())
                defs += 1
            if defs > end + 1:
                index = defs
                continue
        out.append(line)
        index += 1
    return out, commons


def externs_body(path: str) -> list[str]:
    lines = (ROOT / path).read_text().splitlines()
    out = [line for line in lines if not GUARD.match(line)]
    while out and not out[0].strip():
        out.pop(0)
    while out and not out[-1].strip():
        out.pop()
    return out


def manifest_entries(text: str):
    """[(line index, kind, name)] for every subsegment list entry."""
    entries = []
    for index, line in enumerate(text.splitlines()):
        sub = rm.SUBSEG.match(line)
        if sub:
            entries.append((index, sub["kind"], sub["name"]))
            continue
        brace = re.match(r"^\s*-\s*\{.*type:\s*(\S+?),\s*name:\s*([^,}\s]+)", line)
        if brace:
            entries.append((index, brace.group(1), brace.group(2)))
    return entries


def collapse_manifest(text: str, units: list[str], new: str) -> str:
    names = [unit_name(u) for u in units]
    new_name = unit_name(new)
    lines = text.splitlines(keepends=True)
    entries = manifest_entries(text)
    drop: set[int] = set()
    for kind in sorted({k for _, k, n in entries if n in names}):
        # entries of this kind in manifest order, with their position
        seq = [(i, n) for i, k, n in entries if k == kind]
        hits = [pos for pos, (_, n) in enumerate(seq) if n in names]
        if kind == "c" and len(hits) != len(names):
            sys.exit(f"not every unit has one c entry: {names}")
        if hits != list(range(hits[0], hits[0] + len(hits))):
            between = [seq[p][1] for p in range(hits[0], hits[-1] + 1)
                       if seq[p][1] not in names]
            sys.exit(f"{kind} entries are not consecutive (foreign: {between})")
        if kind == "c" and [seq[p][1] for p in hits] != names:
            sys.exit("units are not in manifest order: "
                     + ", ".join(seq[p][1] for p in hits))
        # any other entry (any kind) physically between them?
        first, last = seq[hits[0]][0], seq[hits[-1]][0]
        for i, k, n in entries:
            if first < i < last and n not in names:
                sys.exit(f"{kind}: entry {k} {n} lies between the units")
        keep = seq[hits[0]][0]
        lines[keep] = lines[keep].replace(seq[hits[0]][1], new_name, 1)
        drop.update(seq[p][0] for p in hits[1:])
    return "".join(line for i, line in enumerate(lines) if i not in drop)


def build_source(units: list[str], markers: list[str] | None,
                 commons: list[str]) -> tuple[str, list[str]]:
    parts = [split_unit(u) for u in units]
    if markers is None:
        sets = {tuple(sorted(p[0])) for p in parts}
        if len(sets) != 1:
            sys.exit("units have different markers; pass --marker:\n" + "\n".join(
                f"  {u}: {p[0]}" for u, p in zip(units, parts)))
        markers = parts[0][0]
    includes: list[str] = []
    for part in parts:
        for inc in part[1]:
            if inc not in includes:
                includes.append(inc)
    # #undef template macros that a later unit defines again
    defined_later: list[set[str]] = []
    for index in range(len(parts)):
        later = set()
        for part in parts[index + 1:]:
            later |= {m.group(1) for line in part[2] if (m := DEFINE.match(line))}
        defined_later.append(later)
    hoisted: list[str] = []
    for index, part in enumerate(parts):
        body, found = hoist_commons(part[2])
        parts[index] = (part[0], part[1], body, part[3])
        hoisted.extend(c for c in found if c not in hoisted)
    hoisted.extend(c for c in commons if c not in hoisted)
    out = list(markers)
    if out:
        out.append("")
    out.extend(includes)
    if hoisted:
        out.append("")
        out.append(COMMON_NOTE)
        out.extend(hoisted)
    inlined: list[str] = []
    for index, (_m, _i, body, inl) in enumerate(parts):
        inlined.extend(inl)
        out.append("")
        out.extend(body)
        own = []
        for line in body:
            if (m := DEFINE.match(line)):
                own.append(m.group(1))
            elif (m := UNDEF.match(line)) and m.group(1) in own:
                own.remove(m.group(1))
            elif (m := rm.INCLUDE.match(line)):
                inc = ROOT / os.path.dirname(units[index]) / m["path"]
                if inc.is_file():  # a template that #undefs its parameters
                    for inc_line in inc.read_text().splitlines():
                        if (u := UNDEF.match(inc_line)) and u.group(1) in own:
                            own.remove(u.group(1))
        redefined = [name for name in own if name in defined_later[index]]
        if redefined:
            out.extend(f"#undef {name}" for name in redefined)
    return re.sub(r"\n{3,}", "\n\n", "\n".join(out) + "\n"), inlined


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    parser.add_argument("new")
    parser.add_argument("units", nargs="+")
    parser.add_argument("--marker", action="append")
    parser.add_argument("--common", action="append", default=[])
    parser.add_argument("--externs-to", metavar="HEADER",
                        help="shared header that receives the private externs")
    parser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()

    units = [rm.norm(u) for u in args.units]
    new = rm.norm(args.new)
    tracked = set(rm.tracked_files())
    for unit in units:
        if unit not in tracked or not unit.startswith("src/main/"):
            sys.exit(f"not a tracked src/main unit: {unit}")
    if new in tracked and new not in units:
        sys.exit(f"target exists: {new}")

    source, inlined = build_source(units, args.marker, args.common)
    manifest = (ROOT / MANIFEST).read_text()
    new_manifest = collapse_manifest(manifest, units, new)

    # inlined externs headers must have no other users
    for header in inlined:
        users = [p for p in tracked if p.endswith((".c", ".h", ".inc"))
                 and p not in units and os.path.basename(header)
                 in (ROOT / p).read_text(errors="replace")]
        if users:
            sys.exit(f"{header} is also used by {users}")

    header_text = None
    if inlined:
        if not args.externs_to:
            sys.exit("private externs headers need --externs-to: "
                     + ", ".join(inlined))
        decls = []
        for header in inlined:
            decls.extend(d for d in externs_body(header) if d not in decls)
        header_text = (ROOT / args.externs_to).read_text()
        end = header_text.rstrip().rfind("#endif")
        if end < 0:
            sys.exit(f"{args.externs_to}: no closing #endif")
        block = (f"/* Used by {os.path.basename(new)}. */\n"
                 + "\n".join(decls) + "\n\n")
        header_text = header_text[:end] + block + header_text[end:]

    moves = {u: new for u in units if u != new}
    rules = rm.token_rules(moves)
    # bare `Old.c` mentions, when no other tracked file has that basename
    basenames = [os.path.basename(p) for p in tracked]
    bare = [(re.compile(r"(?<![\w/.-])" + re.escape(os.path.basename(u))
                        + r"(?![\w])"), os.path.basename(new))
            for u in moves if basenames.count(os.path.basename(u)) == 1]
    edits: dict[str, str] = {}
    for path in rm.tracked_files():
        if path == MANIFEST or path in units or rm.is_excluded(path):
            continue
        if pathlib.PurePath(path).suffix not in rm.TEXT_SUFFIXES:
            continue
        file = ROOT / path
        if not file.is_file() or file.is_symlink():
            continue
        try:
            text = file.read_text()
        except UnicodeDecodeError:
            continue
        new_text = rm.rewrite_tokens(
            header_text if header_text and path == args.externs_to else text,
            rules)
        for pattern, name in bare:
            new_text = pattern.sub(name, new_text)
        if path.startswith("configs/") and path.endswith(".yaml"):
            new_text = rm.rewrite_manifest(new_text, path, moves)
        if new_text != text:
            edits[path] = new_text

    print(f"merge {len(units)} units -> {new}")
    for path in sorted(edits):
        print(f"edit  {path}")
    for header in inlined:
        print(f"externs {header} -> {args.externs_to}")
    if args.dry_run:
        sys.stdout.write(source)
        return 0

    (ROOT / MANIFEST).write_text(new_manifest)
    for path, text in edits.items():
        (ROOT / path).write_text(text)
    for unit in units:
        if unit != new:
            rm.git("rm", "-q", unit)
    for header in inlined:
        rm.git("rm", "-q", header)
    (ROOT / new).write_text(source)
    rm.git("add", new, MANIFEST, *edits)
    return 0


if __name__ == "__main__":
    sys.exit(main())
