"""Parasite Eve 1 Script Compiler (Scene VM & FieldFx VM).

Parses the structured `.pes` DSL produced by `decompiler.py` and compiles it
back into 100% byte-identical `taskBlock` (`RelocBlock`) and `FieldFx` binaries.
"""

from __future__ import annotations
import re
import struct
from dataclasses import dataclass, field

from tools.pe_script.decompiler import (
    BinaryExpr,
    ExprNode,
    LeafExpr,
    UnaryExpr,
)
from tools.pe_script.mapping import (
    BINARY_SYMBOL_TO_EOP,
    ENUM_GROUPS,
    EXPR_OPS,
    GLOBAL_VARS_BY_NAME,
    MODE_GLOBAL,
    MODE_IMM,
    MODE_LOCAL,
    MODE_REG,
    MODE_ROOM,
    OPCODES_BY_NAME,
    UNARY_SYMBOL_TO_EOP,
)

# Build inverse lookup for all symbolic enum constants (VEC3_*, EVENT_*, PAD_*, ACTOR_*)
SYMBOLIC_CONSTANTS: dict[str, int] = {}
for _group in ENUM_GROUPS.values():
    for _val, _name in _group.items():
        SYMBOLIC_CONSTANTS[_name] = _val & 0xFFFFFFFF

# Binary operator lookup by symbol -> (eop, precedence)
BINOP_BY_SYMBOL: dict[str, tuple[int, int]] = {}
for _eop, _spec in EXPR_OPS.items():
    if not _spec.is_unary and not _spec.is_func:
        BINOP_BY_SYMBOL[_spec.symbol] = (_eop, _spec.precedence)

FUNC_EOPS: dict[str, tuple[int, int]] = {
    "mov": (19, 1),
    "fixed_mul": (20, 2),
    "fixed_div": (21, 2),
}


# ---------------------------------------------------------------------------
# Lexer / Tokenizer
# ---------------------------------------------------------------------------
TOKEN_RE = re.compile(
    r"""
    (?P<WS>\s+)
  | (?P<COMMENT>//[^\n]*|/\*[\s\S]*?\*/)
  | (?P<FP>[+-]?\d+\.\d+fp\b)
  | (?P<HEX>[+-]?0[xX][0-9a-fA-F]+\b)
  | (?P<INT>[+-]?\d+\b)
  | (?P<AT>@[a-zA-Z_][a-zA-Z0-9_]*)
  | (?P<IDENT>[a-zA-Z_][a-zA-Z0-9_]*)
  | (?P<OP>\|\||&&|==|!=|>=|<=|<<|>>|\+=|-=|[+\-*/%|&^!~<>={}\(\)\[\],;:])
""",
    re.VERBOSE,
)


@dataclass(frozen=True)
class Token:
    kind: str
    value: str
    line: int
    col: int


def tokenize(source: str) -> list[Token]:
    tokens: list[Token] = []
    line_num = 1
    line_start = 0
    pos = 0
    n = len(source)
    while pos < n:
        m = TOKEN_RE.match(source, pos)
        if not m:
            snippet = source[pos : pos + 30]
            raise SyntaxError(f"Unexpected character at line {line_num}, col {pos - line_start + 1}: {snippet!r}")
        kind = m.lastgroup
        val = m.group()
        col = pos - line_start + 1
        if kind in ("WS", "COMMENT"):
            newlines = val.count("\n")
            if newlines:
                line_num += newlines
                line_start = pos + val.rfind("\n") + 1
            pos = m.end()
            continue
        # Note: if a binary operator `+` or `-` is immediately followed by a digit without space
        # after an expression token (IDENT, INT, HEX, FP, `]`, `)`), split the sign as an OP token.
        if kind in ("INT", "HEX", "FP") and val[0] in "+-" and tokens:
            prev = tokens[-1]
            if prev.kind in ("IDENT", "INT", "HEX", "FP") or prev.value in (")", "]"):
                tokens.append(Token("OP", val[0], line_num, col))
                val = val[1:]
                col += 1
        tokens.append(Token(kind, val, line_num, col))
        pos = m.end()
    tokens.append(Token("EOF", "", line_num, pos - line_start + 1))
    return tokens


def parse_fp_token(text: str) -> int:
    """Convert `<number>.<frac>fp` to a 32-bit unsigned 16.16 fixed-point integer."""
    body = text[:-2]  # strip 'fp'
    neg = body.startswith("-")
    if body[0] in "+-":
        body = body[1:]
    int_str, frac_str = body.split(".")
    int_part = int(int_str)
    if frac_str in ("0", "00"):
        frac_val = 0
    elif frac_str in ("25",):
        frac_val = 0x4000
    elif frac_str in ("5", "50"):
        frac_val = 0x8000
    elif frac_str in ("75",):
        frac_val = 0xC000
    else:
        frac_val = int(round(float("0." + frac_str) * 65536.0))
    total = (int_part << 16) + frac_val
    if neg:
        total = -total
    return total & 0xFFFFFFFF


def parse_int_token(tok: Token) -> int:
    if tok.kind == "FP":
        return parse_fp_token(tok.value)
    if tok.kind == "HEX":
        return int(tok.value, 16) & 0xFFFFFFFF
    if tok.kind == "INT":
        return int(tok.value, 10) & 0xFFFFFFFF
    raise SyntaxError(f"Expected number at line {tok.line}:{tok.col}, got {tok.value!r}")


# ---------------------------------------------------------------------------
# AST & Unresolved Instruction Representations for Compilation
# ---------------------------------------------------------------------------
@dataclass(frozen=True)
class LabelRefExpr:
    """A symbolic `@label_XXXX` or `@poly_XXXX` immediate operand."""
    label: str


CompExpr = ExprNode | LabelRefExpr


@dataclass
class EmittedItem:
    """Either a bytecode instruction or a raw polygon vertex table."""
    kind: str  # "instr" or "polygon"
    opcode: int = 0
    modes: list[int] = field(default_factory=list)
    args: list[int | str] = field(default_factory=list)  # int or label name string
    vertices: list[tuple[int, int]] = field(default_factory=list)
    hw_offset: int = 0

    @property
    def word_len(self) -> int:
        if self.kind == "polygon":
            return len(self.vertices) * 2
        return 2 + len(self.args)


# ---------------------------------------------------------------------------
# Recursive-Descent Parser + Bytecode Emitter
# ---------------------------------------------------------------------------
class ScriptCompiler:
    def __init__(self, tokens: list[Token]) -> None:
        self.tokens = tokens
        self.pos = 0
        self.reg_counter = 0
        self.auto_label_id = 0
        self.items: list[EmittedItem] = []
        self.labels: dict[str, int] = {}  # label_name -> halfword offset
        self.pending_labels: list[str] = []

    def cur(self) -> Token:
        return self.tokens[self.pos]

    def peek(self, offset: int = 1) -> Token:
        idx = min(self.pos + offset, len(self.tokens) - 1)
        return self.tokens[idx]

    def eat(self, kind_or_val: str | None = None) -> Token:
        tok = self.cur()
        if kind_or_val is not None:
            if tok.kind != kind_or_val and tok.value != kind_or_val:
                raise SyntaxError(
                    f"Expected {kind_or_val!r} at line {tok.line}:{tok.col}, got {tok.value!r}"
                )
        self.pos += 1
        return tok

    def match(self, val: str) -> bool:
        if self.cur().value == val:
            self.pos += 1
            return True
        return False

    def new_internal_label(self, prefix: str = "L") -> str:
        self.auto_label_id += 1
        return f"__auto_{prefix}_{self.auto_label_id}"

    def mark_label(self, name: str) -> None:
        self.pending_labels.append(name)

    def _Current_hw(self) -> int:
        if not self.items:
            return 0
        last = self.items[-1]
        return last.hw_offset + last.word_len * 2

    def _append_item(self, item: EmittedItem) -> None:
        hw = self._Current_hw()
        item.hw_offset = hw
        for lbl in self.pending_labels:
            self.labels[lbl] = hw
        self.pending_labels.clear()
        self.items.append(item)

    def _emit_instr(self, opcode: int, modes: list[int], args: list[int | str]) -> None:
        self._append_item(EmittedItem(kind="instr", opcode=opcode, modes=modes, args=args))
        # Update Square's register allocation state machine
        if opcode == 0x09 and modes[1] == MODE_REG:
            assert isinstance(args[1], int)
            self.reg_counter = args[1] + 1
        elif opcode in (0x05, 0x1D):
            pass
        else:
            self.reg_counter = 0

    def _emit_eval_tree(self, expr: CompExpr, reg_override: int | None = None) -> tuple[int, int | str]:
        """Emit post-order `0x09 EvalExpr` instructions for `expr` and return `(mode, val)`."""
        if isinstance(expr, LeafExpr):
            return expr.mode, expr.value
        if isinstance(expr, LabelRefExpr):
            return MODE_IMM, expr.label

        if reg_override is not None:
            self.reg_counter = reg_override

        def visit(node: CompExpr) -> tuple[int, int | str]:
            if isinstance(node, LeafExpr):
                return node.mode, node.value
            if isinstance(node, LabelRefExpr):
                return MODE_IMM, node.label
            if isinstance(node, UnaryExpr):
                lm, lv = visit(node.operand)
                dst_r = self.reg_counter
                self._emit_instr(
                    0x09,
                    [MODE_IMM, MODE_REG, lm, MODE_IMM],
                    [node.eop, dst_r, lv, 0],
                )
                return MODE_REG, dst_r
            if isinstance(node, BinaryExpr):
                lm, lv = visit(node.lhs)
                rm, rv = visit(node.rhs)
                dst_r = self.reg_counter
                self._emit_instr(
                    0x09,
                    [MODE_IMM, MODE_REG, lm, rm],
                    [node.eop, dst_r, lv, rv],
                )
                return MODE_REG, dst_r
            raise TypeError(f"Unsupported expression node: {node!r}")

        return visit(expr)

    # -----------------------------------------------------------------------
    # Expression Parser (Precedence Climbing)
    # -----------------------------------------------------------------------
    def parse_expr(self, min_prec: int = 1) -> CompExpr:
        lhs = self.parse_unary()
        while True:
            tok = self.cur()
            if tok.kind == "OP" and tok.value in BINOP_BY_SYMBOL:
                eop, prec = BINOP_BY_SYMBOL[tok.value]
                if prec < min_prec:
                    break
                self.eat()
                rhs = self.parse_expr(prec + 1)
                lhs = BinaryExpr(eop, lhs, rhs)  # type: ignore[arg-type]
            else:
                break
        return lhs

    def parse_unary(self) -> CompExpr:
        tok = self.cur()
        if tok.kind == "OP" and tok.value in ("!", "~", "-", "+"):
            op_sym = self.eat().value
            if op_sym == "+":
                return self.parse_unary()
            if op_sym == "-":
                # Check if next token is a plain literal -> negative immediate
                if self.cur().kind in ("INT", "HEX", "FP"):
                    num_tok = self.eat()
                    val = parse_int_token(num_tok)
                    return LeafExpr(MODE_IMM, (-val) & 0xFFFFFFFF)
                operand = self.parse_unary()
                return UnaryExpr(UNARY_SYMBOL_TO_EOP["-"], operand)  # type: ignore[arg-type]
            if op_sym in UNARY_SYMBOL_TO_EOP:
                operand = self.parse_unary()
                return UnaryExpr(UNARY_SYMBOL_TO_EOP[op_sym], operand)  # type: ignore[arg-type]
        return self.parse_primary()

    def parse_primary(self) -> CompExpr:
        tok = self.cur()
        if tok.value == "(":
            self.eat("(")
            inner = self.parse_expr(1)
            self.eat(")")
            return inner

        if tok.kind in ("INT", "HEX", "FP"):
            self.eat()
            return LeafExpr(MODE_IMM, parse_int_token(tok))

        if tok.kind == "AT":
            self.eat()
            return LabelRefExpr(tok.value)

        if tok.kind == "IDENT":
            ident = self.eat().value
            # Addressing modes: local[i], global[i], room[i], reg[i]
            if ident in ("local", "global", "room", "reg") and self.cur().value == "[":
                self.eat("[")
                idx_tok = self.cur()
                if ident == "global" and idx_tok.kind == "IDENT":
                    g_name = self.eat().value
                    if g_name not in GLOBAL_VARS_BY_NAME:
                        raise SyntaxError(f"Unknown global variable name {g_name!r} at line {idx_tok.line}")
                    idx_val = GLOBAL_VARS_BY_NAME[g_name]
                else:
                    idx_expr = self.parse_expr()
                    if not isinstance(idx_expr, LeafExpr) or idx_expr.mode != MODE_IMM:
                        raise SyntaxError(f"Expected constant index inside {ident}[...] at line {idx_tok.line}")
                    idx_val = idx_expr.value
                self.eat("]")
                mode_map = {
                    "local": MODE_LOCAL,
                    "global": MODE_GLOBAL,
                    "room": MODE_ROOM,
                    "reg": MODE_REG,
                }
                return LeafExpr(mode_map[ident], idx_val)

            # Built-in expression functions: mov, fixed_mul, fixed_div
            if ident in FUNC_EOPS and self.cur().value == "(":
                eop, arity = FUNC_EOPS[ident]
                self.eat("(")
                arg0 = self.parse_expr()
                if arity == 1:
                    self.eat(")")
                    return UnaryExpr(eop, arg0)  # type: ignore[arg-type]
                self.eat(",")
                arg1 = self.parse_expr()
                self.eat(")")
                return BinaryExpr(eop, arg0, arg1)  # type: ignore[arg-type]

            # Symbolic constants (VEC3_POS, EVENT_INTERACT, PAD_READ, ACTOR_X, etc.)
            if ident in SYMBOLIC_CONSTANTS:
                return LeafExpr(MODE_IMM, SYMBOLIC_CONSTANTS[ident])

            # Bare named global variable (if user wrote STORY_FLAGS directly)
            if ident in GLOBAL_VARS_BY_NAME:
                return LeafExpr(MODE_GLOBAL, GLOBAL_VARS_BY_NAME[ident])

            raise SyntaxError(f"Unknown identifier {ident!r} in expression at line {tok.line}:{tok.col}")

        raise SyntaxError(f"Unexpected token {tok.value!r} in expression at line {tok.line}:{tok.col}")

    # -----------------------------------------------------------------------
    # Statement & Block Parser
    # -----------------------------------------------------------------------
    def parse_optional_reg_override(self) -> int | None:
        if self.cur().kind == "AT" and self.cur().value == "@reg":
            self.eat("@reg")
            self.eat("(")
            r_val = parse_int_token(self.eat())
            self.eat(")")
            return r_val
        return None

    def parse_block_body(self) -> None:
        while self.cur().value != "}" and self.cur().kind != "EOF":
            self.parse_stmt()

    def parse_stmt(self) -> None:
        # Collect any leading `@label_XXXX:` declarations
        while self.cur().kind == "AT" and self.peek().value == ":":
            lbl_tok = self.eat()
            self.eat(":")
            self.mark_label(lbl_tok.value)

        if self.cur().value == "}":
            return

        reg_override = self.parse_optional_reg_override()
        tok = self.cur()

        # 1. `while [@reg(r)] (cond) { body }`
        if tok.value == "while":
            self.eat("while")
            if reg_override is None:
                reg_override = self.parse_optional_reg_override()
            loop_start = self.new_internal_label("while_start")
            loop_end = self.new_internal_label("while_end")
            self.mark_label(loop_start)
            self.eat("(")
            cond = self.parse_expr()
            self.eat(")")
            cm, cv = self._emit_eval_tree(cond, reg_override)
            self._emit_instr(0x05, [cm, MODE_IMM], [cv, loop_end])
            self.eat("{")
            self.parse_block_body()
            self.eat("}")
            self._emit_instr(0x00, [MODE_IMM], [loop_start])
            self.mark_label(loop_end)
            return

        # 2. `if [@reg(r)] (cond) { then } [else if ... | else { else }]`
        if tok.value == "if":
            self._parse_if_stmt(reg_override)
            return

        # 3. `case_if [@reg(r)] (lhs == rhs) { body }`
        if tok.value == "case_if":
            self.eat("case_if")
            if reg_override is None:
                reg_override = self.parse_optional_reg_override()
            self.eat("(")
            full_cond = self.parse_expr()
            if not isinstance(full_cond, BinaryExpr) or full_cond.eop != BINARY_SYMBOL_TO_EOP["=="]:
                raise SyntaxError(f"Expected `lhs == rhs` inside case_if at line {tok.line}")
            self.eat(")")
            case_end = self.new_internal_label("case_end")
            if reg_override is not None:
                self.reg_counter = reg_override
            lm, lv = self._emit_eval_tree(full_cond.lhs)
            rm, rv = self._emit_eval_tree(full_cond.rhs)
            self._emit_instr(0x1D, [lm, rm, MODE_IMM], [lv, rv, case_end])
            self.eat("{")
            self.parse_block_body()
            self.eat("}")
            self.mark_label(case_end)
            return

        # 4. `polygon @poly_XXXX { (x, z), ... }`
        if tok.value == "polygon":
            self.eat("polygon")
            poly_lbl = self.eat("AT").value
            self.mark_label(poly_lbl)
            self.eat("{")
            verts: list[tuple[int, int]] = []
            while self.cur().value != "}":
                self.eat("(")
                vx_expr = self.parse_expr()
                self.eat(",")
                vz_expr = self.parse_expr()
                self.eat(")")
                self.match(",")
                if not isinstance(vx_expr, LeafExpr) or not isinstance(vz_expr, LeafExpr):
                    raise SyntaxError(f"Polygon vertices must be constants at line {tok.line}")
                verts.append((vx_expr.value, vz_expr.value))
            self.eat("}")
            self._append_item(EmittedItem(kind="polygon", vertices=verts))
            return

        # 5. `goto @label_XXXX;`
        if tok.value == "goto":
            self.eat("goto")
            target_lbl = self.eat("AT").value
            self.eat(";")
            self._emit_instr(0x00, [MODE_IMM], [target_lbl])
            return

        # 6. `if_not (cond) goto @label_XXXX;`
        if tok.value == "if_not":
            self.eat("if_not")
            self.eat("(")
            cond = self.parse_expr()
            self.eat(")")
            self.eat("goto")
            target_lbl = self.eat("AT").value
            self.eat(";")
            cm, cv = self._emit_eval_tree(cond, reg_override)
            self._emit_instr(0x05, [cm, MODE_IMM], [cv, target_lbl])
            return

        # 7. `if_ne (lhs != rhs) goto @label_XXXX;`
        if tok.value == "if_ne":
            self.eat("if_ne")
            self.eat("(")
            full_cond = self.parse_expr()
            if not isinstance(full_cond, BinaryExpr) or full_cond.eop != BINARY_SYMBOL_TO_EOP["!="]:
                raise SyntaxError(f"Expected `lhs != rhs` inside if_ne at line {tok.line}")
            self.eat(")")
            self.eat("goto")
            target_lbl = self.eat("AT").value
            self.eat(";")
            if reg_override is not None:
                self.reg_counter = reg_override
            lm, lv = self._emit_eval_tree(full_cond.lhs)
            rm, rv = self._emit_eval_tree(full_cond.rhs)
            self._emit_instr(0x1D, [lm, rm, MODE_IMM], [lv, rv, target_lbl])
            return

        # 8. Opcode call `OpcodeName(arg0, ...);` OR assignment `lhs = rhs;`
        if tok.kind == "IDENT" and tok.value in OPCODES_BY_NAME and self.peek().value == "(":
            op_name = self.eat().value
            spec = OPCODES_BY_NAME[op_name]
            self.eat("(")
            arg_exprs: list[CompExpr] = []
            if self.cur().value != ")":
                arg_exprs.append(self.parse_expr())
                while self.match(","):
                    arg_exprs.append(self.parse_expr())
            self.eat(")")
            self.eat(";")
            if len(arg_exprs) != spec.argc:
                raise SyntaxError(
                    f"{op_name} expects {spec.argc} args, got {len(arg_exprs)} at line {tok.line}"
                )
            if reg_override is not None:
                self.reg_counter = reg_override
            modes: list[int] = []
            vals: list[int | str] = []
            for a_expr in arg_exprs:
                am, av = self._emit_eval_tree(a_expr)
                modes.append(am)
                vals.append(av)
            self._emit_instr(spec.opcode, modes, vals)
            return

        # 9. Assignment `lhs = rhs;` (either `0x09 fixed_mul/fixed_div` or `0x0A Assign`)
        lhs_expr = self.parse_expr()
        if not isinstance(lhs_expr, LeafExpr):
            raise SyntaxError(f"Invalid assignment target at line {tok.line}:{tok.col}")
        self.eat("=")
        rhs_expr = self.parse_expr()
        self.eat(";")

        if (
            isinstance(rhs_expr, BinaryExpr)
            and rhs_expr.eop in (20, 21)
            and lhs_expr.mode == MODE_LOCAL
        ):
            if reg_override is not None:
                self.reg_counter = reg_override
            lm, lv = self._emit_eval_tree(rhs_expr.lhs)
            rm, rv = self._emit_eval_tree(rhs_expr.rhs)
            self._emit_instr(
                0x09,
                [MODE_IMM, MODE_LOCAL, lm, rm],
                [rhs_expr.eop, lhs_expr.value, lv, rv],
            )
            return

        rm, rv = self._emit_eval_tree(rhs_expr, reg_override)
        self._emit_instr(0x0A, [lhs_expr.mode, rm], [lhs_expr.value, rv])

    def _parse_if_stmt(self, reg_override: int | None = None) -> None:
        self.eat("if")
        if reg_override is None:
            reg_override = self.parse_optional_reg_override()
        self.eat("(")
        cond = self.parse_expr()
        self.eat(")")
        cm, cv = self._emit_eval_tree(cond, reg_override)
        false_label = self.new_internal_label("if_false")
        self._emit_instr(0x05, [cm, MODE_IMM], [cv, false_label])

        self.eat("{")
        self.parse_block_body()
        self.eat("}")

        if self.match("else"):
            end_label = self.new_internal_label("if_end")
            self._emit_instr(0x00, [MODE_IMM], [end_label])
            self.mark_label(false_label)
            if self.cur().value == "if" or (
                self.cur().kind == "AT" and self.cur().value == "@reg"
            ):
                next_reg = self.parse_optional_reg_override()
                self._parse_if_stmt(next_reg)
            else:
                self.eat("{")
                self.parse_block_body()
                self.eat("}")
            self.mark_label(end_label)
        else:
            self.mark_label(false_label)

    # -----------------------------------------------------------------------
    # Actor & Room Script Compilation
    # -----------------------------------------------------------------------
    def compile_actor_script(self) -> bytes:
        self.items.clear()
        self.labels.clear()
        self.pending_labels.clear()
        self.reg_counter = 0

        self.eat("actor_script")
        self.eat("IDENT")  # actor_N
        has_terminator = True
        if self.cur().kind == "AT" and self.cur().value == "@no_terminator":
            self.eat("@no_terminator")
            has_terminator = False
        self.eat("{")
        self.parse_block_body()
        self.eat("}")

        # Flush any trailing labels at end_hw
        end_hw = self._Current_hw()
        for lbl in self.pending_labels:
            self.labels[lbl] = end_hw
        self.pending_labels.clear()

        # Pass 2: resolve labels and pack 32-bit words
        words: list[int] = []
        for item in self.items:
            if item.kind == "polygon":
                for vx, vz in item.vertices:
                    words.append(vx & 0xFFFFFFFF)
                    words.append(vz & 0xFFFFFFFF)
            else:
                argc = len(item.args)
                hdr = (item.opcode & 0x1FFF) | ((argc & 0xF) << 13)
                tail = 0
                for k, m in enumerate(item.modes):
                    if k < 5:
                        hdr |= (m & 7) << (17 + 3 * k)
                    else:
                        tail |= (m & 7) << (3 * (k - 5))
                words.append(hdr & 0xFFFFFFFF)
                words.append(tail & 0xFFFFFFFF)
                for a in item.args:
                    if isinstance(a, str):
                        if a not in self.labels:
                            raise ValueError(f"Unresolved label {a!r}")
                        words.append(self.labels[a] & 0xFFFFFFFF)
                    else:
                        words.append(a & 0xFFFFFFFF)

        if has_terminator:
            words.append(0xFFFFFFFF)
        return struct.pack(f"<{len(words)}I", *words)

    def compile_room_script(self) -> bytes:
        self.eat("room_script")
        self.eat("IDENT")
        self.eat("{")

        spawns: list[tuple[int, int]] = []
        if self.cur().value == "spawn_table":
            self.eat("spawn_table")
            self.eat("{")
            while self.cur().value != "}":
                self.eat("(")
                t_id = parse_int_token(self.eat())
                self.eat(",")
                s_id = parse_int_token(self.eat())
                self.eat(")")
                self.match(",")
                spawns.append((t_id & 0xFF, s_id & 0xFF))
            self.eat("}")

        actor_blobs: list[bytes] = []
        while self.cur().value == "actor_script":
            actor_blobs.append(self.compile_actor_script())

        self.eat("}")

        count = len(actor_blobs)
        header_size = 8 + count * 4
        offsets: list[int] = []
        cur_off = header_size
        for blob in actor_blobs:
            offsets.append(cur_off)
            cur_off += len(blob)
        u0_off = cur_off

        u0_raw = bytearray([len(spawns)])
        for t_id, s_id in spawns:
            u0_raw.append(t_id)
            u0_raw.append(s_id)
        while len(u0_raw) % 4 != 0:
            u0_raw.append(0)

        header = struct.pack(f"<II{count}I", u0_off, count, *offsets)
        return header + b"".join(actor_blobs) + bytes(u0_raw)


def compile_task_block(source: str) -> bytes:
    """Compile `.pes` source code into a binary `taskBlock` (`RelocBlock`)."""
    tokens = tokenize(source)
    compiler = ScriptCompiler(tokens)
    return compiler.compile_room_script()


# ---------------------------------------------------------------------------
# VM 2: FieldFx Script Compiler
# ---------------------------------------------------------------------------
def compile_fieldfx_script(source: str) -> list[int]:
    """Compile a `fieldfx_script` block back into a list of 32-bit unsigned words."""
    tokens = tokenize(source)
    c = ScriptCompiler(tokens)
    c.eat("fieldfx_script")
    c.eat("IDENT")
    c.eat("{")

    labels: dict[str, int] = {}
    ins_list: list[tuple[str, tuple]] = []

    while c.cur().value != "}" and c.cur().kind != "EOF":
        while c.cur().kind == "AT" and c.peek().value == ":":
            lbl = c.eat().value
            c.eat(":")
            labels[lbl] = len(ins_list)
        if c.cur().value == "}":
            break
        tok = c.cur()
        if tok.value == "halt":
            c.eat("halt")
            c.eat(";")
            ins_list.append(("word", (0xFFFFFFFF,)))
        elif tok.value == "nop":
            c.eat("nop")
            c.eat(";")
            ins_list.append(("word", (0xFFFFFFFE,)))
        elif tok.value == "free_all_slots":
            c.eat("free_all_slots")
            c.eat(";")
            ins_list.append(("word", (0xFFFFFFFD,)))
        elif tok.value in ("start_slot", "wait"):
            op_name = c.eat().value
            c.eat("(")
            val = parse_int_token(c.eat()) & 0xFFFF
            c.eat(")")
            c.eat(";")
            op_hi = 0x0001 if op_name == "start_slot" else 0x0002
            ins_list.append(("word", ((op_hi << 16) | val,)))
        elif tok.value == "raw_word":
            c.eat("raw_word")
            c.eat("(")
            val = parse_int_token(c.eat()) & 0xFFFFFFFF
            c.eat(")")
            c.eat(";")
            ins_list.append(("word", (val,)))
        elif tok.value == "if":
            c.eat("if")
            c.eat("(")
            r_tok = c.eat("IDENT").value
            reg = int(r_tok[1:])
            op_sym = c.eat("OP").value
            val = parse_int_token(c.eat()) & 0xFFFF
            c.eat(")")
            c.eat("goto")
            lbl = c.eat("AT").value
            c.eat(";")
            base_hi = 0x1000 if op_sym == "==" else 0x2000
            ins_list.append(("branch", (base_hi | (reg << 8), lbl, val)))
        elif tok.value == "goto":
            c.eat("goto")
            lbl = c.eat("AT").value
            operand = 0
            if c.match("("):
                operand = parse_int_token(c.eat()) & 0xFFFF
                c.eat(")")
            c.eat(";")
            ins_list.append(("branch", (0x3000, lbl, operand)))
        elif tok.value == "vars":
            c.eat("vars")
            c.eat("[")
            idx = parse_int_token(c.eat()) & 0xFFFF
            c.eat("]")
            c.eat("=")
            r_tok = c.eat("IDENT").value
            reg = int(r_tok[1:])
            c.eat(";")
            ins_list.append(("word", (((0x0050 + reg) << 16) | idx,)))
        elif tok.kind == "IDENT" and tok.value.startswith("r"):
            reg = int(c.eat().value[1:])
            assign_op = c.eat("OP").value
            if assign_op == "=" and c.cur().value == "vars":
                c.eat("vars")
                c.eat("[")
                idx = parse_int_token(c.eat()) & 0xFFFF
                c.eat("]")
                c.eat(";")
                ins_list.append(("word", (((0x0040 + reg) << 16) | idx,)))
            else:
                val = parse_int_token(c.eat()) & 0xFFFF
                c.eat(";")
                base_op = {"=": 0x0010, "+=": 0x0020, "-=": 0x0030}[assign_op]
                ins_list.append(("word", (((base_op + reg) << 16) | val,)))
        else:
            raise SyntaxError(f"Unexpected token {tok.value!r} in fieldfx_script at line {tok.line}")

    c.eat("}")
    words: list[int] = []
    for pc, (kind, data) in enumerate(ins_list):
        if kind == "word":
            words.append(data[0] & 0xFFFFFFFF)
        else:
            hi_prefix, lbl, operand = data
            rel = (labels[lbl] - pc) & 0xFF
            words.append(((hi_prefix | rel) << 16) | (operand & 0xFFFF))
    return words
