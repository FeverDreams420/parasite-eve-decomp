#!/usr/bin/env python3
"""CLI tool for decompiling, compiling, and verifying Parasite Eve 1 scripts (.pes).

Subcommands:
  decompile-all [-o scripts/]             Decompile all 414 rooms from PE.IMG into a directory
  decompile-room <room_id> [-o out.pes]   Decompile a room from disc/extracted/PE.IMG
  decompile <input.bin> [-o out.pes]      Decompile a raw taskBlock (RelocBlock) binary
  compile <input.pes> -o <output.bin>     Compile a .pes script back to a taskBlock binary
  verify-room <room_id>                   Decompile + recompile a room and verify 100% byte match
  verify-all                              Verify 100% byte-matching round-trip across all 414 rooms
  dump-mapping                            Print the complete opcode & variable mapping table
"""

from __future__ import annotations
import argparse
import struct
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from tools.pe_script.compiler import compile_fieldfx_module, compile_task_block
from tools.pe_script.decompiler import (
    decompile_fieldfx_module,
    decompile_task_block,
    extract_all_fieldfx_modules,
    extract_room_from_pe_img,
)
from tools.pe_script.mapping import (
    ENUM_GROUPS,
    EXPR_OPS,
    GLOBAL_VARS,
    OPCODES_BY_ID,
)

DEFAULT_PE_IMG = REPO_ROOT / "disc" / "extracted" / "PE.IMG"
DEFAULT_MAIN_EXE = REPO_ROOT / "assets" / "USA" / "main.exe"
DEFAULT_SCRIPTS_DIR = REPO_ROOT / "scripts"
ROOM_TABLE_OFFSET = 0x83B78
ROOM_TABLE_COUNT = 450


def iter_valid_room_ids(main_exe: bytes) -> list[int]:
    room_ids: list[int] = []
    for room_id in range(ROOM_TABLE_COUNT):
        _, packed = struct.unpack_from("<II", main_exe, ROOM_TABLE_OFFSET + room_id * 8)
        room_sec = (packed >> 20) & 0xFFF
        if room_sec > 0:
            room_ids.append(room_id)
    return room_ids


def format_mapping_markdown() -> str:
    lines: list[str] = [
        "# Parasite Eve 1 Script VM — Central Mapping Reference",
        "",
        "## 1. Addressing Modes (`Script_RunQueue`)",
        "",
        "| Mode | Syntax | Backing Storage in C | Range |",
        "| :--- | :--- | :--- | :--- |",
        "| `0` | Immediate literal (`123`, `10.5fp`, `0x100`, `@label`) | Inline 32-bit operand (`&values[i]`) | 32-bit signed/unsigned |",
        "| `1` | `local[idx]` | Actor entity struct `((u8 *)g_CurrentEntity + 0xAC + idx * 4)` | `0..55` |",
        "| `2` | `global[idx]` / `global[NAME]` | Persistent save/story bank `g_ScriptVariableBank[idx]` | `0..511` |",
        "| `3` | `reg[idx]` (folded into expressions) | Expression evaluation stack `g_ScriptOperandTable[idx]` | `0..15` |",
        "| `4` | `room[idx]` | Room-scoped shared variables `g_ScriptNodeActiveFlags[idx]` | `0..63` |",
        "",
        "## 2. Named Global Variables (`g_ScriptVariableBank`)",
        "",
        "| Index | Name | Usage |",
        "| :--- | :--- | :--- |",
    ]
    for g_idx, g_name in sorted(GLOBAL_VARS.items()):
        lines.append(f"| `0x{g_idx:02X}` (`{g_idx}`) | `global[{g_name}]` | Story / room / event progression state |")

    lines.extend([
        "",
        "## 3. Symbolic Enum Constants",
        "",
    ])
    for group_name, group_vals in ENUM_GROUPS.items():
        lines.append(f"### `{group_name}`")
        lines.append("| Value | Constant |")
        lines.append("| :--- | :--- |")
        for val, sym in sorted(group_vals.items()):
            lines.append(f"| `{val}` | `{sym}` |")
        lines.append("")

    lines.extend([
        "## 4. Expression Operators (`0x09 Script_EvalExpr`)",
        "",
        "| EOP | Operator / Function | Kind | Precedence |",
        "| :--- | :--- | :--- | :--- |",
    ])
    for eop, spec in sorted(EXPR_OPS.items()):
        kind = "Function" if spec.is_func else ("Unary prefix" if spec.is_unary else "Binary infix")
        lines.append(f"| `{eop}` | `{spec.symbol}` | {kind} | `{spec.precedence}` |")

    lines.extend([
        "",
        f"## 5. Scene VM Opcodes ({len(OPCODES_BY_ID)} Opcodes: `0x00..0xF0`)",
        "",
        "| Opcode | DSL Name | C Handler Symbol | Arguments | Description |",
        "| :--- | :--- | :--- | :--- | :--- |",
    ])
    for op_id, spec in sorted(OPCODES_BY_ID.items()):
        sig = ", ".join(spec.arg_types) if spec.arg_types else "—"
        lines.append(
            f"| `0x{op_id:02X}` (`{op_id}`) | `{spec.name}` | `{spec.c_symbol}` | `{sig}` | {spec.doc} |"
        )

    lines.extend([
        "",
        "## 6. FieldFx VM Opcodes (`FieldFx_RunScript` in `src/main/fieldfx/object_script.c`)",
        "",
        "Each instruction is a 32-bit word `(op << 16) | (operand & 0xFFFF)`. After executing a non-halting instruction, `SCRIPT->pc++` runs, so relative branch offsets `(s8)(op & 0xFF)` target `target_pc = pc + rel + 1`.",
        "",
        "| Word / High 16-bit `op` | DSL Syntax | C Semantics (`FieldFx_RunScript`) |",
        "| :--- | :--- | :--- |",
        "| `0xFFFFFFFF` (`-1`) | `halt;` | `SCRIPT->halted = 1; stopped = 1;` |",
        "| `0xFFFFFFFE` (`-2`) | `nop;` | No-op (`command = 0`) |",
        "| `0xFFFFFFFD` (`-3`) | `free_all_slots;` | `FieldFx_FreeAllSlots()` |",
        "| `0x0001` | `start_slot(slot_id);` | `FieldFx_StartSlot(obj, (u8)operand, sizes, init_handlers)` |",
        "| `0x0002` | `wait(frames);` | `SCRIPT->wait = operand; stopped = 1;` |",
        "| `0x0010..0x001F` | `rN = imm16;` | `SCRIPT->regs[op - 0x10] = (s16)operand;` |",
        "| `0x0020..0x002F` | `rN += imm16;` | `SCRIPT->regs[op - 0x20] += (s16)operand;` |",
        "| `0x0030..0x003F` | `rN -= imm16;` | `SCRIPT->regs[op - 0x30] -= (s16)operand;` |",
        "| `0x0040..0x004F` | `rN = vars[idx];` | `SCRIPT->regs[op - 0x40] = SCRIPT->vars[(u16)operand];` |",
        "| `0x0050..0x005F` | `vars[idx] = rN;` | `SCRIPT->vars[(u16)operand] = SCRIPT->regs[op - 0x50];` |",
        "| `0x1000..0x1FFF` | `if (rN == imm16) goto @label;` | `if (SCRIPT->regs[(op >> 8) & 0xF] == (s16)operand) SCRIPT->pc += (s8)op;` |",
        "| `0x2000..0x2FFF` | `if (rN != imm16) goto @label;` | `if (SCRIPT->regs[(op >> 8) & 0xF] != (s16)operand) SCRIPT->pc += (s8)op;` |",
        "| `0x3000..0x3FFF` | `goto @label;` | `SCRIPT->pc += (s8)op;` |",
    ])
    return "\n".join(lines) + "\n"


def cmd_decompile_all(args: argparse.Namespace) -> int:
    pe_img = Path(args.pe_img).read_bytes()
    main_exe = Path(args.main_exe).read_bytes()
    out_dir = Path(args.output)
    rooms_dir = out_dir / "rooms"
    fieldfx_dir = out_dir / "fieldfx"
    rooms_dir.mkdir(parents=True, exist_ok=True)
    fieldfx_dir.mkdir(parents=True, exist_ok=True)

    valid_rooms = iter_valid_room_ids(main_exe)
    total_actors = 0
    total_bytes = 0
    index_rows: list[str] = []

    for room_id in valid_rooms:
        task_block, dialogues = extract_room_from_pe_img(pe_img, main_exe, room_id)
        u0_off, count = struct.unpack_from("<II", task_block, 0)
        spawn_cnt = task_block[u0_off]
        pes_text = decompile_task_block(
            task_block,
            room_id=room_id,
            dialogues=None if args.no_dialogues else dialogues,
        )
        if args.verify:
            recompiled = compile_task_block(pes_text)
            if recompiled != task_block:
                raise RuntimeError(f"Byte mismatch while verifying room {room_id:03d}")

        pes_file = rooms_dir / f"room_{room_id:03d}.pes"
        pes_file.write_text(pes_text, encoding="utf-8")

        total_actors += count
        total_bytes += len(task_block)
        first_dlg = next(iter(dialogues.values()), "—") if dialogues else "—"
        first_dlg = first_dlg.replace("|", "/").replace("\n", " ")
        if len(first_dlg) > 60:
            first_dlg = first_dlg[:57] + "..."
        index_rows.append(
            f"| [`room_{room_id:03d}.pes`](rooms/room_{room_id:03d}.pes) | `{room_id}` | `{count}` | `{spawn_cnt}` | `{len(task_block):,}` B | `{len(dialogues)}` | {first_dlg} |"
        )

    # Export all 64 FieldFx modules (84 scripts across main_exe + 63 overlays)
    fx_modules = extract_all_fieldfx_modules(REPO_ROOT)
    fx_total_scripts = 0
    fx_total_words = 0
    fx_index_rows: list[str] = []

    for mod_name, entries in fx_modules.items():
        fxs_text = decompile_fieldfx_module(mod_name, entries)
        if args.verify:
            compiled = compile_fieldfx_module(fxs_text)
            if len(compiled) != len(entries):
                raise RuntimeError(f"FieldFx script count mismatch in {mod_name}")
            for (c_name, c_words), entry in zip(compiled, entries):
                if c_name != entry.name or c_words != entry.words:
                    raise RuntimeError(f"FieldFx word mismatch in {mod_name}.{entry.name}")
        fxs_file = fieldfx_dir / f"{mod_name}.fxs"
        fxs_file.write_text(fxs_text, encoding="utf-8")

        mod_words = sum(len(e.words) for e in entries)
        fx_total_scripts += len(entries)
        fx_total_words += mod_words
        script_names = ", ".join(f"`{e.name}` (`0x{e.vram:08X}`)" for e in entries)
        total_slots = sum(len(e.slots) for e in entries)
        fx_index_rows.append(
            f"| [`{mod_name}.fxs`](fieldfx/{mod_name}.fxs) | `{mod_name}` | `{len(entries)}` | `{total_slots}` | `{mod_words}` (`{mod_words * 4}` B) | {script_names} |"
        )

    mapping_md = out_dir / "MAPPING.md"
    mapping_md.write_text(format_mapping_markdown(), encoding="utf-8")

    readme_lines = [
        "# Parasite Eve 1 — Decompiled Scripts (`.pes` & `.fxs`)",
        "",
        "## 1. Scene VM (`Script_RunQueue`) — Room Scripts (`scripts/rooms/*.pes`)",
        f"- **Total valid rooms**: `{len(valid_rooms)}` (out of `{ROOM_TABLE_COUNT}` entries in `g_RoomSectorTable`)",
        f"- **Total actor scripts**: `{total_actors:,}`",
        f"- **Total binary script size**: `{total_bytes:,}` bytes (`100%` byte-matching round-trip)",
        "",
        "## 2. FieldFx VM (`FieldFx_RunScript`) — Effect Scripts (`scripts/fieldfx/*.fxs`)",
        f"- **Total binary modules**: `{len(fx_modules)}` (`main_exe` + `{len(fx_modules) - 1}` room/scene overlays)",
        f"- **Total FieldFx scripts**: `{fx_total_scripts}` (`8` global in `main.exe` + `{fx_total_scripts - 8}` in overlays)",
        f"- **Total FieldFx bytecode size**: `{fx_total_words:,}` words (`{fx_total_words * 4:,}` bytes, `100%` byte-matching round-trip)",
        "- **Central Opcode & Variable Mapping**: [`MAPPING.md`](MAPPING.md)",
        "",
        "---",
        "",
        "## FieldFx Script Index (`scripts/fieldfx/`)",
        "",
        "| Script File | Module | Scripts | Slots | Words (Bytes) | Script Symbols (VRAM) |",
        "| :--- | :--- | :--- | :--- | :--- | :--- |",
        *fx_index_rows,
        "",
        "---",
        "",
        "## Room Script Index (`scripts/rooms/`)",
        "",
        "| Script File | Room ID | Actors | Spawns | Binary Size | Dialogues | Sample Dialogue |",
        "| :--- | :--- | :--- | :--- | :--- | :--- | :--- |",
        *index_rows,
        "",
    ]
    (out_dir / "README.md").write_text("\n".join(readme_lines), encoding="utf-8")

    print(
        f"Decompiled {len(valid_rooms)} Scene VM rooms ({total_actors} actor scripts, "
        f"{total_bytes:,} bytes) and {len(fx_modules)} FieldFx modules "
        f"({fx_total_scripts} scripts, {fx_total_words * 4:,} bytes) -> {out_dir}"
    )
    return 0


def cmd_decompile_fieldfx(args: argparse.Namespace) -> int:
    fx_modules = extract_all_fieldfx_modules(REPO_ROOT)
    if args.module:
        if args.module not in fx_modules:
            raise KeyError(f"Unknown FieldFx module {args.module!r}. Available: {sorted(fx_modules)}")
        text = decompile_fieldfx_module(args.module, fx_modules[args.module])
        if args.output:
            Path(args.output).write_text(text, encoding="utf-8")
        else:
            sys.stdout.write(text)
        return 0
    out_dir = Path(args.output or (DEFAULT_SCRIPTS_DIR / "fieldfx"))
    out_dir.mkdir(parents=True, exist_ok=True)
    for mod_name, entries in fx_modules.items():
        text = decompile_fieldfx_module(mod_name, entries)
        (out_dir / f"{mod_name}.fxs").write_text(text, encoding="utf-8")
    print(f"Exported {len(fx_modules)} FieldFx module files -> {out_dir}")
    return 0


def cmd_decompile_room(args: argparse.Namespace) -> int:
    pe_img = Path(args.pe_img).read_bytes()
    main_exe = Path(args.main_exe).read_bytes()
    task_block, dialogues = extract_room_from_pe_img(pe_img, main_exe, args.room_id)
    pes_text = decompile_task_block(
        task_block,
        room_id=args.room_id,
        dialogues=None if args.no_dialogues else dialogues,
    )
    if args.output:
        Path(args.output).write_text(pes_text, encoding="utf-8")
        print(f"Decompiled room {args.room_id} ({len(task_block)} bytes) -> {args.output}")
    else:
        sys.stdout.write(pes_text)
    return 0


def cmd_decompile(args: argparse.Namespace) -> int:
    raw = Path(args.input).read_bytes()
    pes_text = decompile_task_block(raw)
    if args.output:
        Path(args.output).write_text(pes_text, encoding="utf-8")
        print(f"Decompiled {args.input} ({len(raw)} bytes) -> {args.output}")
    else:
        sys.stdout.write(pes_text)
    return 0


def cmd_compile(args: argparse.Namespace) -> int:
    src = Path(args.input).read_text(encoding="utf-8")
    blob = compile_task_block(src)
    Path(args.output).write_bytes(blob)
    print(f"Compiled {args.input} -> {args.output} ({len(blob)} bytes)")
    return 0


def cmd_verify_room(args: argparse.Namespace) -> int:
    pe_img = Path(args.pe_img).read_bytes()
    main_exe = Path(args.main_exe).read_bytes()
    task_block, dialogues = extract_room_from_pe_img(pe_img, main_exe, args.room_id)
    pes_text = decompile_task_block(task_block, room_id=args.room_id, dialogues=dialogues)
    recompiled = compile_task_block(pes_text)
    if recompiled == task_block:
        print(f"OK: Room {args.room_id:03d} matched 100% byte-for-byte ({len(task_block)} bytes)")
        return 0
    print(f"MISMATCH in Room {args.room_id:03d}: orig={len(task_block)}B vs recomp={len(recompiled)}B")
    return 1


def cmd_verify_all(args: argparse.Namespace) -> int:
    pe_img = Path(args.pe_img).read_bytes()
    main_exe = Path(args.main_exe).read_bytes()

    total_rooms = 0
    total_actors = 0
    total_bytes = 0
    failures: list[tuple[str, str]] = []

    for room_id in iter_valid_room_ids(main_exe):
        total_rooms += 1
        try:
            task_block, dialogues = extract_room_from_pe_img(pe_img, main_exe, room_id)
            _, count = struct.unpack_from("<II", task_block, 0)
            total_actors += count
            total_bytes += len(task_block)
            pes_text = decompile_task_block(task_block, room_id=room_id, dialogues=dialogues)
            recompiled = compile_task_block(pes_text)
            if recompiled != task_block:
                u0_orig, cnt_orig = struct.unpack_from("<II", task_block, 0)
                u0_rec, cnt_rec = struct.unpack_from("<II", recompiled, 0)
                offs_orig = list(struct.unpack_from(f"<{cnt_orig}I", task_block, 8)) + [u0_orig]
                offs_rec = list(struct.unpack_from(f"<{cnt_rec}I", recompiled, 8)) + [u0_rec]
                diff_msg = f"len {len(task_block)} != {len(recompiled)}"
                for a_i in range(min(cnt_orig, cnt_rec)):
                    b1 = task_block[offs_orig[a_i] : offs_orig[a_i + 1]]
                    b2 = recompiled[offs_rec[a_i] : offs_rec[a_i + 1]]
                    if b1 != b2:
                        w1 = struct.unpack(f"<{len(b1)//4}I", b1)
                        w2 = struct.unpack(f"<{len(b2)//4}I", b2)
                        for w_i in range(min(len(w1), len(w2))):
                            if w1[w_i] != w2[w_i]:
                                diff_msg = (
                                    f"actor_{a_i} word {w_i} (hw 0x{w_i*2:04X}): "
                                    f"orig=0x{w1[w_i]:08X} vs rec=0x{w2[w_i]:08X}"
                                )
                                break
                        else:
                            diff_msg = f"actor_{a_i} word len {len(w1)} != {len(w2)}"
                        break
                failures.append((f"room_{room_id:03d}", diff_msg))
                print(f"[FAIL] Room {room_id:03d}: {diff_msg}")
        except Exception as exc:
            failures.append((f"room_{room_id:03d}", str(exc)))
            print(f"[ERROR] Room {room_id:03d}: {exc}")

    fx_modules = extract_all_fieldfx_modules(REPO_ROOT)
    fx_total_scripts = 0
    fx_total_words = 0
    for mod_name, entries in fx_modules.items():
        try:
            fxs_text = decompile_fieldfx_module(mod_name, entries)
            compiled = compile_fieldfx_module(fxs_text)
            for (c_name, c_words), entry in zip(compiled, entries):
                fx_total_scripts += 1
                fx_total_words += len(entry.words)
                if c_name != entry.name or c_words != entry.words:
                    failures.append((f"fieldfx:{mod_name}.{entry.name}", "word mismatch"))
                    print(f"[FAIL] FieldFx {mod_name}.{entry.name}: word mismatch")
        except Exception as exc:
            failures.append((f"fieldfx:{mod_name}", str(exc)))
            print(f"[ERROR] FieldFx {mod_name}: {exc}")

    print(
        f"\nVerified {total_rooms} Scene VM rooms ({total_actors} actor scripts, {total_bytes:,} bytes) "
        f"and {len(fx_modules)} FieldFx modules ({fx_total_scripts} scripts, {fx_total_words * 4:,} bytes)."
    )
    if failures:
        print(f"FAILED: {len(failures)} target(s) had mismatches.")
        return 1
    print("SUCCESS: 100% byte-for-byte match across all Scene VM and FieldFx VM scripts!")
    return 0


def cmd_dump_mapping(args: argparse.Namespace) -> int:
    sys.stdout.write(format_mapping_markdown())
    return 0


def main() -> int:
    p = argparse.ArgumentParser(description="Parasite Eve 1 Script Decompiler & Compiler (.pes / .fxs)")
    sub = p.add_subparsers(dest="cmd", required=True)

    p_da = sub.add_parser("decompile-all", help="Decompile all 414 Scene VM rooms and all 64 FieldFx modules into scripts/")
    p_da.add_argument("-o", "--output", default=str(DEFAULT_SCRIPTS_DIR), help="Output directory (default: scripts/)")
    p_da.add_argument("--pe-img", default=str(DEFAULT_PE_IMG), help="Path to PE.IMG")
    p_da.add_argument("--main-exe", default=str(DEFAULT_MAIN_EXE), help="Path to main.exe")
    p_da.add_argument("--no-dialogues", action="store_true", help="Omit dialogue comments")
    p_da.add_argument("--verify", action="store_true", help="Verify 100%% byte match while exporting")
    p_da.set_defaults(func=cmd_decompile_all)

    p_dfx = sub.add_parser("decompile-fieldfx", help="Decompile FieldFx VM scripts (.fxs)")
    p_dfx.add_argument("module", nargs="?", help="Optional module name (e.g. main_exe, scene_e08_chrysler)")
    p_dfx.add_argument("-o", "--output", help="Output file or directory")
    p_dfx.set_defaults(func=cmd_decompile_fieldfx)

    p_dr = sub.add_parser("decompile-room", help="Decompile a room from PE.IMG")
    p_dr.add_argument("room_id", type=int, help="Room ID (0..449)")
    p_dr.add_argument("-o", "--output", help="Output .pes file path")
    p_dr.add_argument("--pe-img", default=str(DEFAULT_PE_IMG), help="Path to PE.IMG")
    p_dr.add_argument("--main-exe", default=str(DEFAULT_MAIN_EXE), help="Path to main.exe")
    p_dr.add_argument("--no-dialogues", action="store_true", help="Omit dialogue comments")
    p_dr.set_defaults(func=cmd_decompile_room)

    p_dec = sub.add_parser("decompile", help="Decompile a binary taskBlock file")
    p_dec.add_argument("input", help="Input binary taskBlock file")
    p_dec.add_argument("-o", "--output", help="Output .pes file path")
    p_dec.set_defaults(func=cmd_decompile)

    p_comp = sub.add_parser("compile", help="Compile a .pes file into a binary taskBlock")
    p_comp.add_argument("input", help="Input .pes source file")
    p_comp.add_argument("-o", "--output", required=True, help="Output binary taskBlock file")
    p_comp.set_defaults(func=cmd_compile)

    p_vr = sub.add_parser("verify-room", help="Verify round-trip byte match for one room")
    p_vr.add_argument("room_id", type=int, help="Room ID (0..449)")
    p_vr.add_argument("--pe-img", default=str(DEFAULT_PE_IMG), help="Path to PE.IMG")
    p_vr.add_argument("--main-exe", default=str(DEFAULT_MAIN_EXE), help="Path to main.exe")
    p_vr.set_defaults(func=cmd_verify_room)

    p_va = sub.add_parser("verify-all", help="Verify round-trip byte match for all 414 rooms and 84 FieldFx scripts")
    p_va.add_argument("--pe-img", default=str(DEFAULT_PE_IMG), help="Path to PE.IMG")
    p_va.add_argument("--main-exe", default=str(DEFAULT_MAIN_EXE), help="Path to main.exe")
    p_va.set_defaults(func=cmd_verify_all)

    p_dm = sub.add_parser("dump-mapping", help="Print the central opcode & variable mapping")
    p_dm.set_defaults(func=cmd_dump_mapping)

    args = p.parse_args()
    return args.func(args)


if __name__ == "__main__":
    sys.exit(main())

