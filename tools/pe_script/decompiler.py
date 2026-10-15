"""Parasite Eve 1 Script Decompiler (Scene VM & FieldFx VM).

Converts binary `taskBlock` (`RelocBlock`) archives and `FieldFx` room scripts
into a structured, human-readable C-like DSL that round-trips 100% byte-for-byte.
"""

from __future__ import annotations
import struct
from dataclasses import dataclass, field
from typing import Any

from tools.pe_script.mapping import (
    ENUM_GROUPS,
    EXPR_OPS,
    GLOBAL_VARS,
    MODE_GLOBAL,
    MODE_IMM,
    MODE_LOCAL,
    MODE_REG,
    MODE_ROOM,
    OPCODES_BY_ID,
)
from tools.scripts.pe1_text import _CODE_TO_CH


# ---------------------------------------------------------------------------
# Expression AST nodes
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class LeafExpr:
    mode: int
    value: int

    def format(self, arg_type: str = "int", vec3_kind_val: int | None = None) -> str:
        if self.mode == MODE_IMM:
            return format_immediate(self.value, arg_type, vec3_kind_val)
        if self.mode == MODE_LOCAL:
            return f"local[{self.value}]"
        if self.mode == MODE_GLOBAL:
            name = GLOBAL_VARS.get(self.value)
            if name is not None:
                return f"global[{name}]"
            return f"global[{self.value}]"
        if self.mode == MODE_ROOM:
            return f"room[{self.value}]"
        if self.mode == MODE_REG:
            return f"reg[{self.value}]"
        raise ValueError(f"Unknown operand mode {self.mode}")


@dataclass(frozen=True)
class UnaryExpr:
    eop: int
    operand: ExprNode

    def format(self, arg_type: str = "int", vec3_kind_val: int | None = None) -> str:
        spec = EXPR_OPS[self.eop]
        if spec.is_func:
            return f"{spec.symbol}({self.operand.format()})"
        inner = self.operand.format()
        if isinstance(self.operand, (BinaryExpr, UnaryExpr)):
            return f"{spec.symbol}({inner})"
        return f"{spec.symbol}{inner}"


@dataclass(frozen=True)
class BinaryExpr:
    eop: int
    lhs: ExprNode
    rhs: ExprNode

    def format(self, arg_type: str = "int", vec3_kind_val: int | None = None) -> str:
        spec = EXPR_OPS[self.eop]
        if spec.is_func:
            return f"{spec.symbol}({self.lhs.format()}, {self.rhs.format()})"
        lhs_str = self.lhs.format()
        rhs_str = self.rhs.format()
        if isinstance(self.lhs, BinaryExpr) and not EXPR_OPS[self.lhs.eop].is_func:
            if EXPR_OPS[self.lhs.eop].precedence < spec.precedence:
                lhs_str = f"({lhs_str})"
        if isinstance(self.rhs, BinaryExpr) and not EXPR_OPS[self.rhs.eop].is_func:
            if EXPR_OPS[self.rhs.eop].precedence <= spec.precedence:
                rhs_str = f"({rhs_str})"
        return f"{lhs_str} {spec.symbol} {rhs_str}"


ExprNode = LeafExpr | UnaryExpr | BinaryExpr


def _to_s32(val: int) -> int:
    val &= 0xFFFFFFFF
    return val - 0x100000000 if val & 0x80000000 else val


def format_fixed16(val: int) -> str:
    uval = val & 0xFFFFFFFF
    sval = _to_s32(uval)
    # Exact quarters (.0, .25, .5, .75) in 16.16 fixed-point
    if (uval & 0x3FFF) == 0 and -0x80000000 <= sval <= 0x7FFFFFFF:
        fval = sval / 65536.0
        if (uval & 0xFFFF) == 0:
            return f"{int(sval >> 16)}.0fp"
        if (uval & 0x7FFF) == 0:
            return f"{fval:.1f}fp"
        return f"{fval:.2f}fp"
    return f"0x{uval:08X}"


def format_immediate(val: int, arg_type: str = "int", vec3_kind_val: int | None = None) -> str:
    uval = val & 0xFFFFFFFF
    sval = _to_s32(uval)

    if arg_type in ENUM_GROUPS:
        enum_map = ENUM_GROUPS[arg_type]
        if sval in enum_map:
            return enum_map[sval]

    if arg_type == "label" or arg_type == "poly_label":
        prefix = "poly" if arg_type == "poly_label" else "label"
        return f"@{prefix}_{uval:04X}"

    if arg_type == "label_or_neg":
        if sval < 0:
            return str(sval)
        return f"@label_{uval:04X}"

    if arg_type == "fixed":
        return format_fixed16(uval)

    if arg_type == "fixed_if_pos":
        if vec3_kind_val in (0, 1):
            return format_fixed16(uval)
        return str(sval) if -0x10000 < sval < 0x10000 else f"0x{uval:08X}"

    if arg_type == "hex":
        if 0 <= uval <= 9:
            return str(uval)
        return f"0x{uval:X}"

    # Default "int": decimal for [-65536..65536], hex for bitmasks / large constants
    if -0x10000 <= sval <= 0x10000:
        return str(sval)
    return f"0x{uval:08X}"


# ---------------------------------------------------------------------------
# Low-level decoded instruction / statement representations
# ---------------------------------------------------------------------------
@dataclass
class RawInstruction:
    hw_offset: int
    opcode: int
    modes: tuple[int, ...]
    args: tuple[int, ...]

    @property
    def word_len(self) -> int:
        return 2 + len(self.args)

    @property
    def end_hw(self) -> int:
        return self.hw_offset + self.word_len * 2


@dataclass
class FoldedStmt:
    start_hw: int
    instr_hw: int
    end_hw: int
    kind: str  # "jump", "jiz", "cond_branch", "assign", "fixed_op", "call", "polygon"
    opcode: int = 0
    exprs: list[ExprNode] = field(default_factory=list)
    target_hw: int | None = None
    reg_override: int | None = None
    vertices: list[tuple[int, int]] = field(default_factory=list)
    comment: str | None = None


# ---------------------------------------------------------------------------
# High-level structured AST nodes
# ---------------------------------------------------------------------------
@dataclass
class BlockStmt:
    kind: str  # "while", "if", "case_if", "leaf"
    start_hw: int
    end_hw: int
    cond: ExprNode | None = None
    rhs_cond: ExprNode | None = None
    then_body: list[BlockStmt] = field(default_factory=list)
    else_body: list[BlockStmt] = field(default_factory=list)
    else_start_hw: int | None = None
    else_jump_reg_override: int | None = None
    leaf: FoldedStmt | None = None
    reg_override: int | None = None


# ---------------------------------------------------------------------------
# Room Dialogue Decoder (for helpful inline comments)
# ---------------------------------------------------------------------------
def extract_room_dialogues(room_bytes: bytes) -> dict[int, str]:
    """Extract English dialogue strings from a room archive's taskSlots[1]."""
    if len(room_bytes) < 8:
        return {}
    _, dir_off = struct.unpack_from("<II", room_bytes, 0)
    if dir_off + 0x34 > len(room_bytes):
        return {}
    d = struct.unpack_from("<13I", room_bytes, dir_off)
    ts_packed = d[8]  # 0x20: taskSlots
    ts_off = ts_packed & 0x3FFFFF
    ts_cnt = ts_packed >> 22
    if ts_cnt < 2 or ts_off + ts_cnt * 8 > len(room_bytes):
        return {}

    # Slot 1 holds English text in USA PE.IMG
    blob_off = None
    for i in range(ts_cnt):
        _, r1 = struct.unpack_from("<II", room_bytes, ts_off + i * 8)
        slot = r1 >> 24
        poff = r1 & 0xFFFFFF
        if slot == 1:
            blob_off = poff
            break
    if blob_off is None or blob_off >= len(room_bytes):
        return {}

    blob = room_bytes[blob_off:]
    dialogues: dict[int, str] = {}
    i = 0
    while i + 3 < len(blob):
        if blob[i] in (0xFF, 0xF9) and blob[i + 1] == 0xFE:
            page_id = blob[i + 2]
            pos = i + 3
            chars: list[str] = []
            while pos < len(blob) and blob[pos] not in (0xFF, 0xF9):
                b = blob[pos]
                if b == 0xF7:
                    chars.append(" / ")
                elif b == 0xF8:
                    chars.append(" | ")
                elif b == 0xFA:
                    chars.append("{Aya}")
                elif b == 0xFB and pos + 1 < len(blob):
                    pos += 1
                    chars.append(f"<FB:{blob[pos]:02X}>")
                elif b in _CODE_TO_CH:
                    chars.append(_CODE_TO_CH[b])
                else:
                    chars.append(f"<{b:02X}>")
                pos += 1
            if page_id not in dialogues:
                text = " ".join("".join(chars).split())
                if len(text) > 90:
                    text = text[:87] + "..."
                dialogues[page_id] = text
            i = pos
        else:
            i += 1
    return dialogues


# ---------------------------------------------------------------------------
# Actor Script Decompilation Pipeline
# ---------------------------------------------------------------------------
def decode_actor_words(words: tuple[int, ...]) -> tuple[list[RawInstruction], dict[int, int]]:
    """Decode an actor script's 32-bit word stream into instructions and polygon data words."""
    if not words:
        raise ValueError("Empty actor script")

    has_term = words[-1] == 0xFFFFFFFF
    data_words: set[int] = set()
    poly_bounds: dict[int, int] = {}  # start_word -> max_end_word
    instrs: list[RawInstruction] = []

    pc = 0
    limit = len(words) - 1 if has_term else len(words)
    unref_start: int | None = None

    while pc < limit:
        if pc in data_words:
            if unref_start is not None:
                poly_bounds[unref_start] = max(poly_bounds.get(unref_start, unref_start), pc)
                unref_start = None
            pc += 1
            continue
        if pc + 1 >= limit:
            break
        hdr = words[pc]
        tail = words[pc + 1]
        op = hdr & 0x1FFF
        argc = (hdr >> 13) & 0xF
        hdr_unused = hdr >> (17 + 3 * min(argc, 5))
        tail_unused = tail >> (3 * max(0, argc - 5))
        modes = tuple(
            (hdr >> (17 + 3 * k)) & 7 if k < 5 else (tail >> (3 * (k - 5))) & 7
            for k in range(argc)
        )
        # Check if (hdr, tail) is an unreferenced polygon vertex pair (x, z)
        if (
            hdr_unused != 0
            or tail_unused != 0
            or op not in OPCODES_BY_ID
            or argc != OPCODES_BY_ID[op].argc
            or any(m > 4 for m in modes)
        ):
            if unref_start is None:
                unref_start = pc
            data_words.add(pc)
            data_words.add(pc + 1)
            pc += 2
            continue

        if unref_start is not None:
            poly_bounds[unref_start] = max(poly_bounds.get(unref_start, unref_start), pc)
            unref_start = None

        args = words[pc + 2 : pc + 2 + argc]
        instrs.append(RawInstruction(pc * 2, op, modes, args))

        if op == 0xAC and modes[0] == MODE_IMM and modes[1] == MODE_IMM:
            w_off = args[0] // 2
            w_end = w_off + args[1] * 2
            data_words.update(range(w_off, w_end))
            poly_bounds[w_off] = max(poly_bounds.get(w_off, w_off), w_end)
        elif op == 0xE2 and modes[3] == MODE_IMM and modes[2] == MODE_IMM:
            w_off = args[3] // 2
            w_end = w_off + args[2] * 2
            data_words.update(range(w_off, w_end))
            poly_bounds[w_off] = max(poly_bounds.get(w_off, w_off), w_end)
        elif op == 0xE3 and modes[0] == MODE_IMM and modes[1] == MODE_IMM:
            w_off = args[0] // 2
            w_end = w_off + args[1] * 2
            data_words.update(range(w_off, w_end))
            poly_bounds[w_off] = max(poly_bounds.get(w_off, w_off), w_end)

        pc += 2 + argc

    if unref_start is not None:
        poly_bounds[unref_start] = max(poly_bounds.get(unref_start, unref_start), limit)

    return instrs, poly_bounds


def fold_actor_instructions(
    words: tuple[int, ...],
    instrs: list[RawInstruction],
    poly_bounds: dict[int, int],
    dialogues: dict[int, str] | None = None,
) -> list[FoldedStmt]:
    """Fold `EvalExpr` register chains into expression trees and interleave polygon blocks."""
    # First, compute predicted reg_counter at each instruction using Square's exact rule
    reg_counter = 0
    reg_overrides: dict[int, int] = {}
    for ins in instrs:
        if ins.opcode == 0x09 and ins.modes[1] == MODE_REG:
            actual_reg = ins.args[1]
            if actual_reg != reg_counter:
                reg_overrides[ins.hw_offset] = actual_reg
            reg_counter = actual_reg + 1
        elif ins.opcode in (0x05, 0x1D):
            pass
        else:
            reg_counter = 0

    # Collect all jump/callback target halfword offsets to know where chains must break
    jump_targets: set[int] = set()
    for ins in instrs:
        if ins.opcode == 0x00 and ins.modes[0] == MODE_IMM:
            jump_targets.add(ins.args[0])
        elif ins.opcode == 0x05 and ins.modes[1] == MODE_IMM:
            jump_targets.add(ins.args[1])
        elif ins.opcode == 0x1D and ins.modes[2] == MODE_IMM:
            jump_targets.add(ins.args[2])
        elif ins.opcode == 0x12 and ins.modes[0] == MODE_IMM:
            jump_targets.add(ins.args[0])
        elif ins.opcode == 0x14 and ins.modes[1] == MODE_IMM and _to_s32(ins.args[1]) >= 0:
            jump_targets.add(ins.args[1])

    # Build polygon segments split at every referenced poly start_word
    sorted_poly_starts = sorted(poly_bounds.keys())
    poly_stmts_by_hw: dict[int, FoldedStmt] = {}
    for idx_p, w_start in enumerate(sorted_poly_starts):
        w_end = poly_bounds[w_start]
        if idx_p + 1 < len(sorted_poly_starts) and sorted_poly_starts[idx_p + 1] < w_end:
            w_end = sorted_poly_starts[idx_p + 1]
        verts: list[tuple[int, int]] = []
        for w in range(w_start, w_end, 2):
            verts.append((words[w], words[w + 1]))
        poly_stmts_by_hw[w_start * 2] = FoldedStmt(
            start_hw=w_start * 2,
            instr_hw=w_start * 2,
            end_hw=w_end * 2,
            kind="polygon",
            vertices=verts,
        )

    # Fold instructions
    folded_code: list[FoldedStmt] = []
    i = 0
    while i < len(instrs):
        ins = instrs[i]
        if ins.opcode == 0x09 and ins.modes[1] == MODE_REG:
            chain = [ins]
            i += 1
            while (
                i < len(instrs)
                and instrs[i].opcode == 0x09
                and instrs[i].modes[1] == MODE_REG
                and instrs[i].hw_offset not in jump_targets
            ):
                chain.append(instrs[i])
                i += 1
            consumer = instrs[i]
            i += 1

            reg_map: dict[int, ExprNode] = {}
            chain_override = reg_overrides.get(chain[0].hw_offset)
            for c_ins in chain:
                eop, dst_r, lhs_v, rhs_v = c_ins.args
                lhs_m, rhs_m = c_ins.modes[2], c_ins.modes[3]
                lhs_node = reg_map[lhs_v] if lhs_m == MODE_REG else LeafExpr(lhs_m, lhs_v)
                spec = EXPR_OPS[eop]
                if spec.is_unary:
                    reg_map[dst_r] = UnaryExpr(eop, lhs_node)
                else:
                    rhs_node = reg_map[rhs_v] if rhs_m == MODE_REG else LeafExpr(rhs_m, rhs_v)
                    reg_map[dst_r] = BinaryExpr(eop, lhs_node, rhs_node)

            con_exprs: list[ExprNode] = []
            for km, ka in zip(consumer.modes, consumer.args):
                if km == MODE_REG:
                    con_exprs.append(reg_map[ka])
                else:
                    con_exprs.append(LeafExpr(km, ka))

            stmt = _make_consumer_stmt(
                chain[0].hw_offset, consumer, con_exprs, chain_override, dialogues
            )
            folded_code.append(stmt)
        else:
            con_exprs = [LeafExpr(km, ka) for km, ka in zip(ins.modes, ins.args)]
            stmt = _make_consumer_stmt(
                ins.hw_offset, ins, con_exprs, reg_overrides.get(ins.hw_offset), dialogues
            )
            folded_code.append(stmt)
            i += 1

    # Merge folded_code and poly_stmts_by_hw in ascending start_hw order
    all_stmts = sorted(
        folded_code + list(poly_stmts_by_hw.values()),
        key=lambda s: s.start_hw,
    )
    return all_stmts


def _make_consumer_stmt(
    start_hw: int,
    consumer: RawInstruction,
    exprs: list[ExprNode],
    reg_override: int | None,
    dialogues: dict[int, str] | None,
) -> FoldedStmt:
    op = consumer.opcode
    comment = None
    if dialogues and op in (0x0D, 0xD1) and consumer.modes[0] == MODE_IMM:
        page_id = consumer.args[0]
        if page_id in dialogues:
            comment = f'"{dialogues[page_id]}"'

    if op == 0x00 and consumer.modes[0] == MODE_IMM:
        return FoldedStmt(
            start_hw=start_hw,
            instr_hw=consumer.hw_offset,
            end_hw=consumer.end_hw,
            kind="jump",
            opcode=op,
            exprs=exprs,
            target_hw=consumer.args[0],
            reg_override=reg_override,
        )
    if op == 0x05 and consumer.modes[1] == MODE_IMM:
        return FoldedStmt(
            start_hw=start_hw,
            instr_hw=consumer.hw_offset,
            end_hw=consumer.end_hw,
            kind="jiz",
            opcode=op,
            exprs=[exprs[0]],
            target_hw=consumer.args[1],
            reg_override=reg_override,
        )
    if op == 0x1D and consumer.modes[2] == MODE_IMM:
        return FoldedStmt(
            start_hw=start_hw,
            instr_hw=consumer.hw_offset,
            end_hw=consumer.end_hw,
            kind="cond_branch",
            opcode=op,
            exprs=[exprs[0], exprs[1]],
            target_hw=consumer.args[2],
            reg_override=reg_override,
        )
    if op == 0x0A:
        return FoldedStmt(
            start_hw=start_hw,
            instr_hw=consumer.hw_offset,
            end_hw=consumer.end_hw,
            kind="assign",
            opcode=op,
            exprs=exprs,
            reg_override=reg_override,
        )
    if op == 0x09 and consumer.modes[1] == MODE_LOCAL:
        eop = consumer.args[0]
        return FoldedStmt(
            start_hw=start_hw,
            instr_hw=consumer.hw_offset,
            end_hw=consumer.end_hw,
            kind="fixed_op",
            opcode=op,
            exprs=[exprs[1], BinaryExpr(eop, exprs[2], exprs[3])],
            reg_override=reg_override,
        )
    return FoldedStmt(
        start_hw=start_hw,
        instr_hw=consumer.hw_offset,
        end_hw=consumer.end_hw,
        kind="call",
        opcode=op,
        exprs=exprs,
        reg_override=reg_override,
        comment=comment,
    )


# ---------------------------------------------------------------------------
# Control-Flow Structurer (Lossless 1:1 AST Recovery)
# ---------------------------------------------------------------------------
def structure_statements(stmts: list[FoldedStmt]) -> tuple[list[BlockStmt], set[int]]:
    """Recover nested `while`, `if / else`, `if`, and `case_if` blocks losslessly.

    Returns `(structured_blocks, required_labels)` where `required_labels` is the
    exact set of halfword offsets that are still referenced by explicit labels
    (`goto`, `if_not`, `if_ne`, `SpawnThread`, `SetEventHandler`).
    """
    hw_to_idx: dict[int, int] = {s.start_hw: idx for idx, s in enumerate(stmts)}
    end_hw_to_idx: dict[int, int] = {s.end_hw: idx + 1 for idx, s in enumerate(stmts)}

    def struct_slice(lo: int, hi: int) -> list[BlockStmt]:
        out: list[BlockStmt] = []
        limit_hw = stmts[hi - 1].end_hw if hi > lo else 0
        k = lo
        while k < hi:
            s = stmts[k]

            # 1. Check for `while (cond) { body }`:
            # s is `jiz` targeting `@loop_end` (<= limit_hw), and the statement
            # ending at `@loop_end` is `jump` back to `s.start_hw`, with no polygons inside.
            if s.kind == "jiz" and s.target_hw is not None and s.end_hw < s.target_hw <= limit_hw:
                end_idx = end_hw_to_idx.get(s.target_hw)
                if end_idx is not None and k + 1 < end_idx <= hi:
                    back_stmt = stmts[end_idx - 1]
                    if (
                        back_stmt.kind == "jump"
                        and back_stmt.target_hw == s.start_hw
                        and back_stmt.reg_override is None
                        and not any(x.kind == "polygon" for x in stmts[k + 1 : end_idx])
                        and not any(
                            x.kind in ("jiz", "cond_branch") and x.target_hw == s.target_hw
                            for x in stmts[k + 1 : end_idx - 1]
                        )
                    ):
                        body = struct_slice(k + 1, end_idx - 1)
                        out.append(
                            BlockStmt(
                                kind="while",
                                start_hw=s.start_hw,
                                end_hw=s.target_hw,
                                cond=s.exprs[0],
                                then_body=body,
                                reg_override=s.reg_override,
                            )
                        )
                        k = end_idx
                        continue

            # 2. Check for `if (cond) { then } [else { else }]`:
            if s.kind == "jiz" and s.target_hw is not None and s.end_hw <= s.target_hw <= limit_hw:
                target_idx = end_hw_to_idx.get(s.target_hw) if s.target_hw > s.end_hw else k + 1
                if target_idx is not None and k < target_idx <= hi and not any(
                    x.kind == "polygon" for x in stmts[k + 1 : target_idx]
                ):
                    # Check if `target_idx - 1` is an `else` jump `Jump(@else_end)`
                    can_else = False
                    else_end_idx = None
                    if target_idx > k + 1:
                        last_then = stmts[target_idx - 1]
                        if (
                            last_then.kind == "jump"
                            and last_then.target_hw is not None
                            and s.target_hw < last_then.target_hw <= limit_hw
                            and last_then.reg_override is None
                        ):
                            cand_end_idx = end_hw_to_idx.get(last_then.target_hw)
                            if (
                                cand_end_idx is not None
                                and target_idx < cand_end_idx <= hi
                                and not any(x.kind == "polygon" for x in stmts[target_idx:cand_end_idx])
                                # Do not claim `last_then` as an `else` jump if an inner branch in
                                # `then_body` also targets `s.target_hw` (in that case `s.target_hw`
                                # is the shared end of an outer `if` without `else`).
                                and not any(
                                    x.kind in ("jiz", "cond_branch") and x.target_hw == s.target_hw
                                    for x in stmts[k + 1 : target_idx - 1]
                                )
                            ):
                                can_else = True
                                else_end_idx = cand_end_idx

                    if can_else and else_end_idx is not None:
                        then_b = struct_slice(k + 1, target_idx - 1)
                        else_b = struct_slice(target_idx, else_end_idx)
                        out.append(
                            BlockStmt(
                                kind="if",
                                start_hw=s.start_hw,
                                end_hw=stmts[else_end_idx - 1].end_hw,
                                cond=s.exprs[0],
                                then_body=then_b,
                                else_body=else_b,
                                else_start_hw=s.target_hw,
                                reg_override=s.reg_override,
                            )
                        )
                        k = else_end_idx
                        continue
                    else:
                        then_b = struct_slice(k + 1, target_idx)
                        out.append(
                            BlockStmt(
                                kind="if",
                                start_hw=s.start_hw,
                                end_hw=s.target_hw,
                                cond=s.exprs[0],
                                then_body=then_b,
                                reg_override=s.reg_override,
                            )
                        )
                        k = target_idx
                        continue

            # 3. Check for `case_if (lhs == rhs) { body }` (0x1D CondBranch):
            if s.kind == "cond_branch" and s.target_hw is not None and s.end_hw <= s.target_hw <= limit_hw:
                target_idx = end_hw_to_idx.get(s.target_hw) if s.target_hw > s.end_hw else k + 1
                if target_idx is not None and k < target_idx <= hi and not any(
                    x.kind == "polygon" for x in stmts[k + 1 : target_idx]
                ):
                    then_b = struct_slice(k + 1, target_idx)
                    out.append(
                        BlockStmt(
                            kind="case_if",
                            start_hw=s.start_hw,
                            end_hw=s.target_hw,
                            cond=s.exprs[0],
                            rhs_cond=s.exprs[1],
                            then_body=then_b,
                            reg_override=s.reg_override,
                        )
                    )
                    k = target_idx
                    continue

            # Fallback: leaf statement
            out.append(BlockStmt(kind="leaf", start_hw=s.start_hw, end_hw=s.end_hw, leaf=s))
            k += 1

        return out

    blocks = struct_slice(0, len(stmts))

    # Collect all halfword offsets that are still referenced by any leaf statement
    required_labels: set[int] = set()

    def collect_labels(blks: list[BlockStmt]) -> None:
        for b in blks:
            if b.kind == "leaf" and b.leaf is not None:
                lf = b.leaf
                if lf.kind in ("jump", "jiz", "cond_branch") and lf.target_hw is not None:
                    required_labels.add(lf.target_hw)
                elif lf.kind == "call":
                    if lf.opcode == 0x12 and isinstance(lf.exprs[0], LeafExpr) and lf.exprs[0].mode == MODE_IMM:
                        required_labels.add(lf.exprs[0].value)
                    elif (
                        lf.opcode == 0x14
                        and isinstance(lf.exprs[1], LeafExpr)
                        and lf.exprs[1].mode == MODE_IMM
                        and _to_s32(lf.exprs[1].value) >= 0
                    ):
                        required_labels.add(lf.exprs[1].value)
            else:
                collect_labels(b.then_body)
                collect_labels(b.else_body)

    collect_labels(blocks)
    return blocks, required_labels


# ---------------------------------------------------------------------------
# Code Formatter (DSL Emitter)
# ---------------------------------------------------------------------------
def format_actor_blocks(
    actor_idx: int,
    blocks: list[BlockStmt],
    required_labels: set[int],
    end_hw: int,
    has_terminator: bool = True,
) -> list[str]:
    term_attr = "" if has_terminator else " @no_terminator"
    lines: list[str] = [f"actor_script actor_{actor_idx}{term_attr} {{"]
    emitted_labels: set[int] = set()

    def emit_label_if_needed(hw: int, indent: int) -> None:
        if hw in required_labels and hw not in emitted_labels:
            emitted_labels.add(hw)
            pad = "    " * max(0, indent - 1)
            lines.append(f"{pad}@label_{hw:04X}:")

    def emit_block_list(blks: list[BlockStmt], indent: int) -> None:
        for b in blks:
            emit_block(b, indent)

    def emit_block(b: BlockStmt, indent: int, is_else_if: bool = False) -> None:
        if not is_else_if:
            emit_label_if_needed(b.start_hw, indent)
        pad = "    " * indent
        reg_attr = f" @reg({b.reg_override})" if b.reg_override is not None else ""

        if b.kind == "while":
            assert b.cond is not None
            lines.append(f"{pad}while{reg_attr} ({b.cond.format()}) {{")
            emit_block_list(b.then_body, indent + 1)
            emit_label_if_needed(b.end_hw - 6, indent + 1)
            lines.append(f"{pad}}}")
            return

        if b.kind == "case_if":
            assert b.cond is not None and b.rhs_cond is not None
            cmp_str = BinaryExpr(11, b.cond, b.rhs_cond).format()
            lines.append(f"{pad}case_if{reg_attr} ({cmp_str}) {{")
            emit_block_list(b.then_body, indent + 1)
            lines.append(f"{pad}}}")
            return

        if b.kind == "if":
            assert b.cond is not None
            prefix = "" if is_else_if else pad
            header = f"{prefix}if{reg_attr} ({b.cond.format()}) {{"
            if is_else_if:
                lines[-1] = lines[-1] + " else " + header
            else:
                lines.append(header)
            emit_block_list(b.then_body, indent + 1)
            if b.else_body:
                assert b.else_start_hw is not None
                emit_label_if_needed(b.else_start_hw - 6, indent + 1)
                # Can we format as `else if`? Only if else_start_hw is not a required label
                # and else_body is a single `if` block!
                if (
                    len(b.else_body) == 1
                    and b.else_body[0].kind == "if"
                    and b.else_start_hw not in required_labels
                ):
                    lines.append(f"{pad}}}")
                    emit_block(b.else_body[0], indent, is_else_if=True)
                else:
                    lines.append(f"{pad}}} else {{")
                    emit_label_if_needed(b.else_start_hw, indent + 1)
                    emit_block_list(b.else_body, indent + 1)
                    lines.append(f"{pad}}}")
            else:
                lines.append(f"{pad}}}")
            return

        # Leaf statement
        lf = b.leaf
        assert lf is not None
        if lf.instr_hw != lf.start_hw:
            emit_label_if_needed(lf.instr_hw, indent)
        lf_reg = f"@reg({lf.reg_override}) " if lf.reg_override is not None else ""

        if lf.kind == "polygon":
            lines.append(f"{pad}polygon @poly_{lf.start_hw:04X} {{")
            for vx, vz in lf.vertices:
                lines.append(f"{pad}    ({format_fixed16(vx)}, {format_fixed16(vz)}),")
            lines.append(f"{pad}}}")
        elif lf.kind == "jump":
            lines.append(f"{pad}goto @label_{lf.target_hw:04X};")
        elif lf.kind == "jiz":
            lines.append(f"{pad}{lf_reg}if_not ({lf.exprs[0].format()}) goto @label_{lf.target_hw:04X};")
        elif lf.kind == "cond_branch":
            cmp_str = BinaryExpr(14, lf.exprs[0], lf.exprs[1]).format()
            lines.append(
                f"{pad}{lf_reg}if_ne ({cmp_str}) goto @label_{lf.target_hw:04X};"
            )
        elif lf.kind in ("assign", "fixed_op"):
            lines.append(f"{pad}{lf_reg}{lf.exprs[0].format()} = {lf.exprs[1].format()};")
        elif lf.kind == "call":
            spec = OPCODES_BY_ID[lf.opcode]
            vec3_kind_val = None
            if lf.opcode in (0x0B, 0x0C) and isinstance(lf.exprs[0], LeafExpr) and lf.exprs[0].mode == MODE_IMM:
                vec3_kind_val = _to_s32(lf.exprs[0].value)
            fmt_args = [
                expr.format(arg_t, vec3_kind_val)
                for expr, arg_t in zip(lf.exprs, spec.arg_types)
            ]
            call_str = f"{pad}{lf_reg}{spec.name}({', '.join(fmt_args)});"
            if lf.comment:
                call_str += f" // {lf.comment}"
            lines.append(call_str)

    emit_block_list(blocks, 1)
    emit_label_if_needed(end_hw, 1)
    lines.append("}")
    return lines


# ---------------------------------------------------------------------------
# Top-Level `RelocBlock` (`taskBlock`) and `PE.IMG` Room Decompilers
# ---------------------------------------------------------------------------
def decompile_task_block(
    task_block: bytes,
    room_id: int | None = None,
    dialogues: dict[int, str] | None = None,
) -> str:
    """Decompile a binary `taskBlock` (`RelocBlock`) into the structured `.pes` DSL."""
    u0_off, count = struct.unpack_from("<II", task_block, 0)
    offsets = list(struct.unpack_from(f"<{count}I", task_block, 8)) + [u0_off]

    u0_bytes = task_block[u0_off:]
    spawn_cnt = u0_bytes[0]
    spawns: list[tuple[int, int]] = []
    for s_idx in range(spawn_cnt):
        t_id = u0_bytes[1 + 2 * s_idx]
        sub_id = u0_bytes[2 + 2 * s_idx]
        spawns.append((t_id, sub_id))

    header_name = f"room_{room_id:03d}" if room_id is not None else "task_block"
    out_lines: list[str] = [
        f"// Parasite Eve 1 Scene Script ({header_name})",
        f"room_script {header_name} {{",
    ]

    if spawns:
        spawn_items = ", ".join(f"({t}, {s})" for t, s in spawns)
        out_lines.append(f"    spawn_table {{ {spawn_items} }}")
    else:
        out_lines.append("    spawn_table {}")
    out_lines.append("")

    for a_idx in range(count):
        s_start = offsets[a_idx]
        s_end = offsets[a_idx + 1]
        words = struct.unpack_from(f"<{(s_end - s_start) // 4}I", task_block, s_start)
        has_term = bool(words and words[-1] == 0xFFFFFFFF)
        instrs, poly_bounds = decode_actor_words(words)
        folded = fold_actor_instructions(words, instrs, poly_bounds, dialogues)
        blocks, req_labels = structure_statements(folded)
        end_hw = ((len(words) - 1) if has_term else len(words)) * 2
        actor_lines = format_actor_blocks(a_idx, blocks, req_labels, end_hw, has_term)
        for line in actor_lines:
            out_lines.append("    " + line if line else "")
        if a_idx + 1 < count:
            out_lines.append("")

    out_lines.append("}")
    return "\n".join(out_lines) + "\n"


def extract_room_from_pe_img(
    pe_img: bytes,
    main_exe: bytes,
    room_id: int,
    room_table_offset: int = 0x83B78,
) -> tuple[bytes, dict[int, str]]:
    """Extract `(task_block_bytes, dialogues)` for `room_id` from `PE.IMG`."""
    start, packed = struct.unpack_from("<II", main_exe, room_table_offset + room_id * 8)
    scratch = packed & 0xFF
    tex = (packed >> 8) & 0xFFF
    room_sec = (packed >> 20) & 0xFFF
    if room_sec == 0:
        raise ValueError(f"Room {room_id} has 0 room sectors in g_RoomSectorTable")

    room_off = (start + scratch + tex) * 2048
    room_bytes = pe_img[room_off : room_off + room_sec * 2048]
    _, dir_off = struct.unpack_from("<II", room_bytes, 0)
    d = struct.unpack_from("<13I", room_bytes, dir_off)
    tb_rec_off = d[5] & 0x3FFFFF
    td_rec_off = d[6] & 0x3FFFFF
    tb_off = struct.unpack_from("<III", room_bytes, tb_rec_off)[1] & 0xFFFFFF
    td_off = struct.unpack_from("<III", room_bytes, td_rec_off)[1] & 0xFFFFFF

    task_block = room_bytes[tb_off:td_off]
    dialogues = extract_room_dialogues(room_bytes)
    return task_block, dialogues


# ---------------------------------------------------------------------------
# VM 2: FieldFx Room Effect Script Decompiler (`FieldFx_RunScript`)
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class FieldFxSlotHandler:
    slot_id: int
    init_fn: str
    draw_fn: str
    update_fn: str
    size: int


@dataclass(frozen=True)
class FieldFxScriptEntry:
    module: str
    name: str
    vram: int
    words: list[int]
    slots: dict[int, FieldFxSlotHandler]
    init_list_sym: str = ""
    draw_list_sym: str = ""
    update_list_sym: str = ""
    spawn_layout_sym: str = ""


def decompile_fieldfx_script(
    words: list[int] | tuple[int, ...],
    name: str = "fieldfx_0",
    slots: dict[int, FieldFxSlotHandler] | None = None,
    vram: int | None = None,
    table_syms: tuple[str, str, str, str] | None = None,
) -> str:
    """Decompile a 32-bit `FieldFx_RunScript` word array into readable DSL."""
    targets: set[int] = set()
    for pc, w in enumerate(words):
        u = w & 0xFFFFFFFF
        if u in (0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFD):
            continue
        op = u >> 16
        if 0x1000 <= op <= 0x3FFF:
            rel = op & 0xFF
            if rel >= 0x80:
                rel -= 0x100
            targets.add(pc + rel + 1)

    lines: list[str] = []
    if vram is not None:
        lines.append(f"// VRAM: 0x{vram:08X} | Words: {len(words)} ({len(words) * 4} bytes)")
    if table_syms and any(table_syms):
        init_s, draw_s, upd_s, lay_s = table_syms
        lines.append(f"// Tables: init={init_s}, draw={draw_s}, update={upd_s}, sizes={lay_s}")
    if slots:
        lines.append("// Slots:")
        for sid, sh in sorted(slots.items()):
            lines.append(
                f"//   [{sid}] init={sh.init_fn}, draw={sh.draw_fn}, update={sh.update_fn}, size=0x{sh.size:X}"
            )
    lines.append(f"fieldfx_script {name} {{")
    for pc, w in enumerate(words):
        if pc in targets:
            lines.append(f"@label_{pc:04X}:")
        u = w & 0xFFFFFFFF
        if u == 0xFFFFFFFF:
            lines.append("    halt;")
            continue
        if u == 0xFFFFFFFE:
            lines.append("    nop;")
            continue
        if u == 0xFFFFFFFD:
            lines.append("    free_all_slots;")
            continue
        op = u >> 16
        operand = u & 0xFFFF
        s_operand = operand - 0x10000 if operand & 0x8000 else operand
        if op == 0x0001:
            comment = ""
            if slots and s_operand in slots:
                sh = slots[s_operand]
                comment = f" // init={sh.init_fn}, draw={sh.draw_fn}, update={sh.update_fn}, size=0x{sh.size:X}"
            lines.append(f"    start_slot({s_operand});{comment}")
        elif op == 0x0002:
            lines.append(f"    wait({s_operand});")
        elif 0x0010 <= op <= 0x001F:
            lines.append(f"    r{op - 0x10} = {s_operand};")
        elif 0x0020 <= op <= 0x002F:
            lines.append(f"    r{op - 0x20} += {s_operand};")
        elif 0x0030 <= op <= 0x003F:
            lines.append(f"    r{op - 0x30} -= {s_operand};")
        elif 0x0040 <= op <= 0x004F:
            lines.append(f"    r{op - 0x40} = vars[{s_operand}];")
        elif 0x0050 <= op <= 0x005F:
            lines.append(f"    vars[{s_operand}] = r{op - 0x50};")
        elif 0x1000 <= op <= 0x1FFF:
            reg = (op >> 8) & 0xF
            rel = (op & 0xFF) - 0x100 if (op & 0xFF) >= 0x80 else (op & 0xFF)
            lines.append(f"    if (r{reg} == {s_operand}) goto @label_{pc + rel + 1:04X};")
        elif 0x2000 <= op <= 0x2FFF:
            reg = (op >> 8) & 0xF
            rel = (op & 0xFF) - 0x100 if (op & 0xFF) >= 0x80 else (op & 0xFF)
            lines.append(f"    if (r{reg} != {s_operand}) goto @label_{pc + rel + 1:04X};")
        elif 0x3000 <= op <= 0x3FFF:
            rel = (op & 0xFF) - 0x100 if (op & 0xFF) >= 0x80 else (op & 0xFF)
            if operand != 0:
                lines.append(f"    goto @label_{pc + rel + 1:04X} ({s_operand});")
            else:
                lines.append(f"    goto @label_{pc + rel + 1:04X};")
        else:
            lines.append(f"    raw_word(0x{u:08X});")
    lines.append("}")
    return "\n".join(lines) + "\n"


def decompile_fieldfx_module(module_name: str, entries: list[FieldFxScriptEntry]) -> str:
    """Decompile all `FieldFx` scripts of a binary module (`main_exe` or overlay) into a `.fxs` file."""
    total_words = sum(len(e.words) for e in entries)
    header = [
        f"// Parasite Eve 1 - FieldFx VM Module `{module_name}`",
        f"// Scripts: {len(entries)} | Total Words: {total_words} ({total_words * 4} bytes)",
        "",
    ]
    blocks: list[str] = []
    for entry in entries:
        blk = decompile_fieldfx_script(
            entry.words,
            name=entry.name,
            slots=entry.slots,
            vram=entry.vram,
            table_syms=(
                entry.init_list_sym,
                entry.draw_list_sym,
                entry.update_list_sym,
                entry.spawn_layout_sym,
            ),
        )
        blocks.append(blk.rstrip("\n"))
    return "\n".join(header) + "\n\n".join(blocks) + "\n"


def _read_fieldfx_script_words(buf: bytes, off: int) -> list[int]:
    words: list[int] = []
    max_target = 0
    pc = 0
    while pc < 256 and off + (pc + 1) * 4 <= len(buf):
        w = struct.unpack_from("<I", buf, off + pc * 4)[0]
        words.append(w)
        if w not in (0xFFFFFFFF, 0xFFFFFFFE, 0xFFFFFFFD):
            op = w >> 16
            if 0x1000 <= op <= 0x3FFF:
                rel = (op & 0xFF) - 0x100 if (op & 0xFF) >= 0x80 else (op & 0xFF)
                target = pc + rel + 1
                if target > max_target:
                    max_target = target
        if w == 0xFFFFFFFF and pc >= max_target:
            if words.count(0xFFFFFFFF) > 1 and off + (pc + 2) * 4 <= len(buf):
                nw = struct.unpack_from("<I", buf, off + (pc + 1) * 4)[0]
                if nw == 0xFFFFFFFF:
                    words.append(nw)
            break
        pc += 1
    return words


def _extract_reg_imm32(words: tuple[int, ...], start_idx: int, end_idx: int, target_reg: int) -> int | None:
    hi: dict[int, int] = {}
    val: int | None = None
    for j in range(start_idx, end_idx):
        ins = words[j]
        op = ins >> 26
        rs = (ins >> 21) & 0x1F
        rt = (ins >> 16) & 0x1F
        imm = ins & 0xFFFF
        simm = imm - 0x10000 if imm & 0x8000 else imm
        if op == 0x0F:
            hi[rt] = imm << 16
            if rt == target_reg:
                val = hi[rt]
        elif op == 0x09 and rs in hi:
            hi[rt] = (hi[rs] + simm) & 0xFFFFFFFF
            if rt == target_reg:
                val = hi[rt]
        elif op == 0x0D and rs in hi:
            hi[rt] = (hi[rs] | imm) & 0xFFFFFFFF
            if rt == target_reg:
                val = hi[rt]
        elif op == 0x25 and (rs == 0 or rt == 0):
            rd = (ins >> 11) & 0x1F
            src = rs if rt == 0 else rt
            if src in hi:
                hi[rd] = hi[src]
                if rd == target_reg:
                    val = hi[rd]
    return val


def _find_fieldfx_classes_in_buf(
    buf: bytes, base_vram: int
) -> list[tuple[int, int, int, int, int, int]]:
    jal_get_slot = 0x0C0308BE      # FieldFx_GetSlot (0x800C22F8)
    jal_register = 0x0C030905      # FieldFx_Register (0x800C2414)
    jal_update_slots = 0x0C030947  # FieldFx_UpdateSlots (0x800C251C)
    jal_run_script = 0x0C0309D6    # FieldFx_RunScript (0x800C2758)

    n = len(buf) // 4
    words = struct.unpack_from(f"<{n}I", buf, 0)
    classes: list[tuple[int, int, int, int, int, int]] = []
    plant_indices = [i for i, w in enumerate(words) if w == jal_get_slot]
    for idx, i in enumerate(plant_indices):
        next_i = plant_indices[idx + 1] if idx + 1 < len(plant_indices) else min(n, i + 250)
        hi: dict[int, int] = {}
        script_addr = 0
        for j in range(i + 1, min(n, i + 10)):
            ins = words[j]
            op = ins >> 26
            rs = (ins >> 21) & 0x1F
            rt = (ins >> 16) & 0x1F
            imm = ins & 0xFFFF
            simm = imm - 0x10000 if imm & 0x8000 else imm
            if op == 0x0F:
                hi[rt] = imm << 16
            elif op == 0x09 and rs in hi:
                hi[rt] = (hi[rs] + simm) & 0xFFFFFFFF
            elif op == 0x2B and rs == 2 and simm == 0 and rt in hi:
                script_addr = hi[rt]
                break
        draw_list = update_list = init_list = spawn_layout = 0
        for k in range(i + 1, next_i):
            if words[k] == jal_register and not draw_list:
                draw_list = _extract_reg_imm32(words, max(i, k - 12), min(n, k + 2), 5) or 0
            elif words[k] == jal_update_slots and not update_list:
                update_list = _extract_reg_imm32(words, max(i, k - 12), min(n, k + 2), 5) or 0
            elif words[k] == jal_run_script and not init_list:
                init_list = _extract_reg_imm32(words, max(i, k - 12), min(n, k + 2), 5) or 0
                spawn_layout = _extract_reg_imm32(words, max(i, k - 12), min(n, k + 2), 6) or 0
                break
        classes.append((base_vram + i * 4, script_addr, init_list, draw_list, update_list, spawn_layout))
    return classes


def _read_ptr_table(buf: bytes, off: int) -> list[int]:
    res: list[int] = []
    for i in range(64):
        if off + (i + 1) * 4 > len(buf):
            break
        w = struct.unpack_from("<I", buf, off + i * 4)[0]
        if w == 0xFFFFFFFF or w == 0 or not (0x80010000 <= w < 0x80200000):
            break
        res.append(w)
    return res


def extract_all_fieldfx_modules(repo_root: Any) -> dict[str, list[FieldFxScriptEntry]]:
    """Extract all 84 `FieldFx` scripts across `main_exe` and all 63 room/scene overlays."""
    import re
    from pathlib import Path

    root = Path(repo_root)
    pe_img = (root / "disc/extracted/PE.IMG").read_bytes()
    main_exe = (root / "disc/extracted/SLUS_006.62").read_bytes()

    main_syms: dict[int, str] = {}
    sym_main_path = root / "configs/USA/sym.main.txt"
    if sym_main_path.exists():
        for line in sym_main_path.read_text().splitlines():
            m = re.match(r"^(\w+)\s*=\s*0x([0-9A-Fa-f]+)", line)
            if m:
                main_syms[int(m.group(2), 16)] = m.group(1)

    modules: dict[str, list[FieldFxScriptEntry]] = {}

    # 1. Extract 8 global FieldFx scripts from main.exe (SLUS_006.62)
    main_buf = main_exe[0x800:]
    main_base = 0x80010000
    main_entries: list[FieldFxScriptEntry] = []
    for _, s_vram, init_vram, draw_vram, upd_vram, lay_vram in _find_fieldfx_classes_in_buf(
        main_buf, main_base
    ):
        s_off = s_vram - main_base
        words = _read_fieldfx_script_words(main_buf, s_off)
        inits = _read_ptr_table(main_buf, init_vram - main_base)
        draws = _read_ptr_table(main_buf, draw_vram - main_base)
        updates = _read_ptr_table(main_buf, upd_vram - main_base)
        slot_count = max(len(inits), len(draws), len(updates))
        lay_off = lay_vram - main_base
        sizes = [
            struct.unpack_from("<H", main_buf, lay_off + i * 2)[0]
            for i in range(slot_count)
        ]
        slots: dict[int, FieldFxSlotHandler] = {}
        for sid in range(slot_count):
            i_addr = inits[sid] if sid < len(inits) else 0
            d_addr = draws[sid] if sid < len(draws) else 0
            u_addr = updates[sid] if sid < len(updates) else 0
            slots[sid] = FieldFxSlotHandler(
                slot_id=sid,
                init_fn=main_syms.get(i_addr, f"0x{i_addr:08X}"),
                draw_fn=main_syms.get(d_addr, f"0x{d_addr:08X}"),
                update_fn=main_syms.get(u_addr, f"0x{u_addr:08X}"),
                size=sizes[sid] if sid < len(sizes) else 0,
            )
        s_name = main_syms.get(s_vram, f"g_FieldFxScript_{s_vram:08X}")
        main_entries.append(
            FieldFxScriptEntry(
                module="main_exe",
                name=s_name,
                vram=s_vram,
                words=words,
                slots=slots,
                init_list_sym=main_syms.get(init_vram, f"0x{init_vram:08X}"),
                draw_list_sym=main_syms.get(draw_vram, f"0x{draw_vram:08X}"),
                update_list_sym=main_syms.get(upd_vram, f"0x{upd_vram:08X}"),
                spawn_layout_sym=main_syms.get(lay_vram, f"0x{lay_vram:08X}"),
            )
        )
    modules["main_exe"] = main_entries

    # 2. Extract all 76 overlay FieldFx scripts across the 63 overlays in PE.IMG
    dd_re = re.compile(
        r"dd\s+if=\S+\s+of=\S+\s+bs=(?P<bs>\d+)\s+skip=(?P<skip>\d+)\s+count=(?P<count>\d+)"
    )
    for cfg_path in sorted((root / "configs/USA/overlays").glob("*.yaml")):
        mod_name = cfg_path.stem
        txt = cfg_path.read_text()
        m_dd = dd_re.search(txt)
        m_vram = re.search(r"vram:\s*0x([0-9A-Fa-f]+)", txt)
        if not m_dd or not m_vram:
            continue
        vram_base = int(m_vram.group(1), 16)
        img_off = int(m_dd.group("bs")) * int(m_dd.group("skip"))
        img_size = int(m_dd.group("bs")) * int(m_dd.group("count"))
        classes = _find_fieldfx_classes_in_buf(pe_img[img_off : img_off + img_size], vram_base)
        if not classes:
            continue

        ovl_syms: dict[int, str] = dict(main_syms)
        if mod_name == "scene_e08_chrysler":
            ovl_syms[0x80191DB8] = "SceneE08_InitParticleSlots"
        for sp in (
            root / f"configs/USA/overlays/sym.{mod_name}.txt",
            root / f"linkers/USA/overlays/link_symbols.{mod_name}.txt",
        ):
            if sp.exists():
                for line in sp.read_text().splitlines():
                    m1 = re.match(r"^(\w+)\s*=\s*0x([0-9A-Fa-f]+)", line)
                    if m1:
                        ovl_syms[int(m1.group(2), 16)] = m1.group(1).split("__")[0]
                    m2 = re.match(r"^(\w+)\s*=\s*OVERLAY_WINDOW_ROOM\s*\+\s*0x([0-9A-Fa-f]+)", line)
                    if m2:
                        ovl_syms[0x8018EFE8 + int(m2.group(2), 16)] = m2.group(1).split("__")[0]

        mod_entries: list[FieldFxScriptEntry] = []
        used_names: set[str] = set()
        for _, s_vram, init_vram, draw_vram, upd_vram, lay_vram in classes:
            s_off = img_off + (s_vram - vram_base)
            words = _read_fieldfx_script_words(pe_img, s_off)
            inits = _read_ptr_table(pe_img, img_off + (init_vram - vram_base))
            draws = _read_ptr_table(pe_img, img_off + (draw_vram - vram_base))
            updates = _read_ptr_table(pe_img, img_off + (upd_vram - vram_base))
            slot_count = max(len(inits), len(draws), len(updates))
            lay_off = img_off + (lay_vram - vram_base)
            sizes = [
                struct.unpack_from("<H", pe_img, lay_off + i * 2)[0]
                for i in range(slot_count)
            ]
            slots = {}
            for sid in range(slot_count):
                i_addr = inits[sid] if sid < len(inits) else 0
                d_addr = draws[sid] if sid < len(draws) else 0
                u_addr = updates[sid] if sid < len(updates) else 0
                slots[sid] = FieldFxSlotHandler(
                    slot_id=sid,
                    init_fn=ovl_syms.get(i_addr, f"0x{i_addr:08X}"),
                    draw_fn=ovl_syms.get(d_addr, f"0x{d_addr:08X}"),
                    update_fn=ovl_syms.get(u_addr, f"0x{u_addr:08X}"),
                    size=sizes[sid] if sid < len(sizes) else 0,
                )
            s_name = ovl_syms.get(s_vram, f"g_FieldFxScript_{s_vram:08X}")
            if s_name in used_names:
                s_name = f"{s_name}_{s_vram:08X}"
            used_names.add(s_name)
            mod_entries.append(
                FieldFxScriptEntry(
                    module=mod_name,
                    name=s_name,
                    vram=s_vram,
                    words=words,
                    slots=slots,
                    init_list_sym=ovl_syms.get(init_vram, f"0x{init_vram:08X}"),
                    draw_list_sym=ovl_syms.get(draw_vram, f"0x{draw_vram:08X}"),
                    update_list_sym=ovl_syms.get(upd_vram, f"0x{upd_vram:08X}"),
                    spawn_layout_sym=ovl_syms.get(lay_vram, f"0x{lay_vram:08X}"),
                )
            )
        modules[mod_name] = mod_entries

    return modules

