"""Locate decompiled functions and turn their translation units into host C.

Source files under src/ are merged, split and renamed constantly, so tests
must not hardcode a file name.  Find the unit that defines a symbol instead:

    path = find_definition("func_80197230")
    text = host_unit("func_80197230")            # preprocessed, host-ready
    text = function_text("func_80197230")        # just that definition

host_unit() preprocesses the whole unit with the project include paths and
tools/tests/host_psyq.h, removes PSX-only constructs a 64-bit host compiler
rejects (pointer-size static asserts, fixed-register pins, top-level asm), and
keeps only the requested function definitions plus the static helpers they
reach.  Every other function in the unit is reduced to a prototype (or
dropped when static), so a test only has to stub what its subject calls.
"""
from __future__ import annotations

import functools
import pathlib
import re
import subprocess

ROOT = pathlib.Path(__file__).resolve().parents[2]
HOST_PSYQ = ROOT / "tools/tests/host_psyq.h"

_IDENT = re.compile(r"[A-Za-z_]\w*")


@functools.lru_cache(maxsize=None)
def _c_files(root: str) -> tuple[pathlib.Path, ...]:
    base = ROOT / root
    return tuple(sorted(p for p in base.rglob("*.c") if p.is_file()))


@functools.lru_cache(maxsize=None)
def _read(path: pathlib.Path) -> str:
    return path.read_text(errors="replace")


def _definition_regex(name: str) -> re.Pattern[str]:
    # Return type and name at the start of a line, parameter list, then the
    # opening brace (possibly on the next line).  Excludes prototypes.
    return re.compile(
        r"^(?!\s)(?![^\n]*\b(?:typedef|extern)\b)[^\n;#=]*?\b"
        + re.escape(name)
        + r"\s*\([^;{}]*\)\s*\{",
        re.M,
    )


def find_definitions(name: str, under: str = "src") -> list[pathlib.Path]:
    """All .c files under `under` that define function `name`."""
    pattern = _definition_regex(name)
    needle = name
    return [p for p in _c_files(under)
            if needle in _read(p) and pattern.search(_read(p))]


def find_definition(name: str, under: str = "src") -> pathlib.Path:
    """The single .c file under `under` that defines function `name`."""
    hits = find_definitions(name, under)
    if len(hits) != 1:
        raise LookupError(f"{name}: expected one defining unit under {under}, "
                          f"found {[str(p.relative_to(ROOT)) for p in hits]}")
    return hits[0]


def find_header(pattern: str, under: str = "include") -> pathlib.Path:
    """The single header under `under` whose text matches regex `pattern`."""
    rx = re.compile(pattern, re.M)
    hits = [p for p in sorted((ROOT / under).rglob("*.h")) if rx.search(_read(p))]
    if len(hits) != 1:
        raise LookupError(f"{pattern!r}: expected one header under {under}, "
                          f"found {[str(p.relative_to(ROOT)) for p in hits]}")
    return hits[0]


# --- top-level C splitting -------------------------------------------------

def _skip_literal(text: str, i: int) -> int:
    quote = text[i]
    i += 1
    while i < len(text) and text[i] != quote:
        i += 2 if text[i] == "\\" else 1
    return i + 1


def _match_brace(text: str, i: int) -> int:
    """Index just past the brace block that opens at text[i]."""
    depth = 0
    while i < len(text):
        c = text[i]
        if c in "\"'":
            i = _skip_literal(text, i)
            continue
        if text.startswith("/*", i):
            i = text.index("*/", i + 2) + 2
            continue
        if text.startswith("//", i):
            i = text.find("\n", i)
            i = len(text) if i < 0 else i
            continue
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i + 1
        i += 1
    raise ValueError("unbalanced braces")


def _items(text: str):
    """Yield (kind, header, body, start, end) for each top-level item.

    kind is "function" for a definition (body is the brace block) and "other"
    for declarations, typedefs and directives.
    """
    i = start = 0
    paren = 0
    n = len(text)
    while i < n:
        c = text[i]
        if c in "\"'":
            i = _skip_literal(text, i)
            continue
        if text.startswith("/*", i):
            i = text.index("*/", i + 2) + 2
            continue
        if text.startswith("//", i):
            j = text.find("\n", i)
            i = n if j < 0 else j
            continue
        if c == "#" and text[start:i].strip() == "":
            j = text.find("\n", i)
            while j > 0 and text[j - 1] == "\\":
                j = text.find("\n", j + 1)
            j = n if j < 0 else j + 1
            yield "other", text[start:j], "", start, j
            i = start = j
            continue
        if c == "(":
            paren += 1
        elif c == ")":
            paren -= 1
        elif c == ";" and paren == 0:
            yield "other", text[start:i + 1], "", start, i + 1
            start = i + 1
        elif c == "{" and paren == 0:
            end = _match_brace(text, i)
            header = text[start:i]
            stripped = re.sub(r"__attribute__\s*\(\(.*?\)\)", "", header).strip()
            is_function = (stripped.endswith(")")
                           and not re.match(r"\s*(typedef|struct|union|enum)\b", stripped)
                           and "=" not in stripped)
            if is_function:
                yield "function", header, text[i:end], start, end
                i = start = end
                continue
            # Aggregate definition or initializer: continue to the ';'.
            i = end
            continue
        i += 1
    if text[start:].strip():
        yield "other", text[start:], "", start, n


def _function_name(header: str) -> str | None:
    header = re.sub(r"__attribute__\s*\(\(.*?\)\)", "", header)
    m = re.search(r"(\w+)\s*\([^()]*(?:\([^()]*\)[^()]*)*\)\s*$", header.strip())
    return m.group(1) if m else None


def function_text(name: str, path: pathlib.Path | None = None) -> str:
    """Raw source text of one function definition (no preprocessing)."""
    path = path or find_definition(name)
    for kind, header, body, _, _ in _items(_read(path)):
        if kind == "function" and _function_name(header) == name:
            return header.lstrip("\n") + body + "\n"
    raise LookupError(f"{name} not defined in {path}")


def defined_functions(path: pathlib.Path) -> list[str]:
    return [_function_name(h) for k, h, _, _, _ in _items(_read(path))
            if k == "function"]


# --- host conversion -------------------------------------------------------

_STATIC_ASSERT = re.compile(
    r"typedef\s+char\s+\w+\s*\[[^;]*?\?\s*1\s*:\s*-\s*1\s*\]\s*;")
_REGISTER_PIN = re.compile(r"\b(?:asm|__asm__)\s*\(\s*\"\$?\w+\"\s*\)")
_TOP_ASM = re.compile(r"^\s*(?:asm|__asm__)\b")


def preprocess(path: pathlib.Path, prefix: str = "", include_dirs=()) -> str:
    """Host-preprocess a unit with project include paths and host_psyq.h."""
    args = ["cc", "-E", "-P", "-std=gnu11", "-DPERMUTER",
            "-include", str(HOST_PSYQ),
            "-I", str(ROOT / "include"), "-I", str(path.parent)]
    for d in include_dirs:
        args += ["-I", str(d)]
    source = prefix + "\n#line 1 \"" + str(path) + "\"\n" + _read(path)
    result = subprocess.run(args + ["-x", "c", "-"], input=source, text=True,
                            capture_output=True, cwd=path.parent)
    if result.returncode != 0:
        raise RuntimeError(result.stderr)
    return result.stdout


def _identifiers(text: str) -> set[str]:
    return set(_IDENT.findall(text))


def host_unit(*names: str, path: pathlib.Path | None = None, prefix: str = "",
              keep_all: bool = False, include_dirs=()) -> str:
    """Preprocessed host-ready C for the unit that defines `names[0]`.

    Only the definitions in `names` (and static functions they reach) are kept
    unless `keep_all`; other non-static functions become prototypes.
    `prefix` is inserted before the unit during preprocessing (for #defines).
    """
    path = path or find_definition(names[0])
    text = preprocess(path, prefix, include_dirs)
    text = _STATIC_ASSERT.sub("", text)
    text = _REGISTER_PIN.sub("", text)

    items = list(_items(text))
    functions = {}
    for kind, header, body, _, _ in items:
        if kind == "function":
            functions[_function_name(header)] = (header, body)
    missing = [n for n in names if n not in functions]
    if missing:
        raise LookupError(f"{missing} not defined in {path}")

    keep = set(functions) if keep_all else set(names)
    work = list(keep)
    while work:
        header, body = functions[work.pop()]
        for ident in _identifiers(body):
            if ident in functions and ident not in keep \
                    and re.search(r"\bstatic\b", functions[ident][0]):
                keep.add(ident)
                work.append(ident)

    out = []
    for kind, header, body, _, _ in items:
        if kind == "function":
            name = _function_name(header)
            if name in keep:
                out.append(header + body)
            elif not re.search(r"\bstatic\b", header):
                out.append(header.rstrip() + ";")
        elif _TOP_ASM.match(header):
            continue
        else:
            out.append(header)
    return "".join(out) + "\n"


def psx_compile(source: str, out: pathlib.Path, like: pathlib.Path,
                *cflags: str) -> subprocess.CompletedProcess:
    """Compile `source` text with tools/scripts/cc.sh as if it were `like`.

    The text is written next to `out`, so quoted includes of headers that
    live beside the original unit are resolved through an extra -I.
    """
    path = out.with_suffix(".c")
    path.write_text(source)
    return subprocess.run(
        [str(ROOT / "tools/scripts/cc.sh"), str(path), str(out),
         "-I" + str(like.parent), *cflags],
        cwd=ROOT, capture_output=True, text=True)
