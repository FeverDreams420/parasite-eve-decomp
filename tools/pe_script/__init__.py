"""Parasite Eve 1 Script Compiler, Decompiler, and Opcode Mapping Package."""

from tools.pe_script.compiler import (
    compile_fieldfx_script,
    compile_task_block,
)
from tools.pe_script.decompiler import (
    decompile_fieldfx_script,
    decompile_task_block,
    extract_room_from_pe_img,
)
from tools.pe_script.mapping import (
    ENUM_GROUPS,
    EXPR_OPS,
    GLOBAL_VARS,
    OPCODE_SPECS,
    OPCODES_BY_ID,
    OPCODES_BY_NAME,
)

__all__ = [
    "ENUM_GROUPS",
    "EXPR_OPS",
    "GLOBAL_VARS",
    "OPCODE_SPECS",
    "OPCODES_BY_ID",
    "OPCODES_BY_NAME",
    "compile_fieldfx_script",
    "compile_task_block",
    "decompile_fieldfx_script",
    "decompile_task_block",
    "extract_room_from_pe_img",
]
