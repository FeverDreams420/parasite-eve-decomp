import subprocess
import tempfile
import unittest
from pathlib import Path

from tools.scripts import ld_postpass as lp
from tools.scripts import shift_test as st
from tools.scripts.shift_audit import BUILD, LINKERS, tool

SCRIPT = """\
SECTIONS
{
    main_ROM_START = __romPos;
    .main 0x80010000 : AT(main_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        main_DATA_START = .;
        build/a.o(.data*);
        main_main_sbss_OFFSET = .;
        build/b.o(.data*);
        main_main_scommon_OFFSET = .;
        build/c.o(.data*);
        . = ALIGN(., 4);
        main_DATA_END = .;
        main_DATA_SIZE = ABSOLUTE(main_DATA_END - main_DATA_START);
    }
    main_bss_VRAM = ADDR(.main_bss);
    .main_bss (NOLOAD) : SUBALIGN(4)
    {
        FILL(0x00000000);
        main_BSS_START = .;
        build/a.o(.bss*);
        build/b.o(.bss*);
        . = ALIGN(., 4);
        main_BSS_END = .;
        main_BSS_SIZE = ABSOLUTE(main_BSS_END - main_BSS_START);
    }
    __romPos += SIZEOF(.main);
    .field_engine_bss (NOLOAD) : SUBALIGN(4)
    {
        field_engine_BSS_SIZE = ABSOLUTE(field_engine_BSS_END - field_engine_BSS_START);
    }
    __romPos += SIZEOF(.field_engine);
    field_engine_ROM_END = __romPos;
    field_engine_VRAM_END = .;

    overlay_windows_ROM_START = __romPos;
    overlay_windows_VRAM = ADDR(.overlay_windows);
    .overlay_windows 0x8010BCF8 : AT(overlay_windows_ROM_START) SUBALIGN(4)
    {
        build/w.o(.data*);
    }
    __romPos += SIZEOF(.overlay_windows);
    overlay_windows_ROM_END = __romPos;
}
"""


class RewriteTests(unittest.TestCase):
    def test_bss_moves_into_main_with_catch_alls_and_startup_symbols(self):
        out = lp.rewrite(SCRIPT)
        self.assertNotIn("NOLOAD) : SUBALIGN(4)\n    {\n        FILL(0x00000000);\n        main_BSS_START", out)
        main = out[out.index(".main 0x80010000"):out.index("__romPos += SIZEOF(.main);")]
        order = ["*(.sdata .sdata.*)", "main_main_sbss_OFFSET", "_edata = .;", "__bss_start = .;",
                 "*(.sbss .sbss.*)", "*(.scommon)", "main_main_scommon_OFFSET", "main_DATA_END",
                 "main_BSS_START = .;", "build/a.o(.bss*);", "build/b.o(.bss*);", "*(COMMON)",
                 "main_BSS_END = .;", "_end = .;"]
        positions = [main.index(s) for s in order]
        self.assertEqual(positions, sorted(positions))
        self.assertIn("main_bss_VRAM = main_BSS_START;", out)
        self.assertIn("ASSERT(SIZEOF(.field_engine_bss) == 0,", out)

    def test_overlay_windows_anchored_with_assertions(self):
        out = lp.rewrite(SCRIPT)
        windows = out[out.index("/* ld_postpass: overlay windows"):]
        self.assertIn(".overlay_windows 0x8010BCF8 : "
                      "AT(main_ROM_START + (ADDR(.overlay_windows) - ADDR(.main))) SUBALIGN(4)", windows)
        self.assertNotIn("overlay_windows_ROM_START = __romPos;", out)
        order = ["build/w.o(.data*);", "overlay_windows_ROM_START = LOADADDR(.overlay_windows);",
                 "ASSERT(field_engine_VRAM_END <= overlay_windows_VRAM,",
                 "ASSERT(field_engine_ROM_END <= overlay_windows_ROM_START,",
                 "overlay_windows_HEADROOM = ABSOLUTE(overlay_windows_VRAM - field_engine_VRAM_END);",
                 "__romPos = overlay_windows_ROM_START + SIZEOF(.overlay_windows);",
                 "overlay_windows_ROM_END = __romPos;"]
        positions = [windows.index(s) for s in order]
        self.assertEqual(positions, sorted(positions))
        self.assertLess(out.index("field_engine_VRAM_END = .;"), out.index("/* ld_postpass: overlay windows"))

    def test_idempotent(self):
        once = lp.rewrite(SCRIPT)
        self.assertEqual(lp.rewrite(once), once)

    def test_unexpected_shapes_fail(self):
        with self.assertRaises(lp.PostPassError):
            lp.rewrite(SCRIPT.replace("        main_main_scommon_OFFSET = .;\n", ""))
        with self.assertRaises(lp.PostPassError):
            lp.rewrite(SCRIPT.replace("build/b.o(.bss*);", "build/b.o(.sbss*);"))
        with self.assertRaises(lp.PostPassError):
            lp.rewrite(SCRIPT.replace("    __romPos += SIZEOF(.main);\n", ""))
        with self.assertRaises(lp.PostPassError):  # no fixed window segment
            lp.rewrite(SCRIPT.replace(".overlay_windows 0x8010BCF8 :", ".overlay_windows field_engine_VRAM_END :"))
        with self.assertRaises(lp.PostPassError):
            lp.rewrite(SCRIPT.replace("    __romPos += SIZEOF(.overlay_windows);\n", ""))
        with self.assertRaises(lp.PostPassError):
            lp.rewrite(SCRIPT.replace("    field_engine_VRAM_END = .;\n", ""))


# A unit that adds small data, small and large COMMON and a gp-relative load
# from an executable (not data) section, the shapes MASPSX -G8 units emit.
GROWTH_PROBE = """\
    .set noreorder
    .section .sdata, "aw"
probe_sdata:
    .word 1
    .section .sbss, "aw", @nobits
    .align 2
probe_sbss:
    .space 4
    .comm probe_small, 4
    .comm probe_big, 64
    .section .sdata, "aw"
    lw $2, %gp_rel(probe_small)($28)
    lw $2, %gp_rel(probe_sbss)($28)
"""


WORK_AREA = "build/USA/asm/USA/main/data/engine/boot_work_area.data.s.o(.data*);"
WORK_AREA_SIZE = 0x18800
WINDOW = 0x8010BCF8


def small_work_area(size: int) -> str:
    """A stand-in for Boot_InitMemoryLayout's work area, SIZE bytes long."""
    return ("    .section .data, \"aw\"\n    .globl g_BootWorkArea\n"
            f"g_BootWorkArea:\n    .space 0x{size:X}\n")


@unittest.skipUnless((BUILD / "main.map").exists() and (LINKERS / "main.ld").exists(),
                     "needs `make build`")
class BuiltLayoutTests(unittest.TestCase):
    def run_link(self, tmp: Path, extra: list[Path], script: Path | None = None):
        out = tmp / "main.elf"
        cmd = [tool("mipsel-none-elf-ld"), "-EL", "-T", str(script or LINKERS / "main.ld"),
               "-T", str(LINKERS / "undefined_syms_manual.txt"),
               "-T", str(LINKERS / "undefined_addr_aliases.main.txt"),
               "-o", str(out), *map(str, extra)]
        return subprocess.run(cmd, cwd=st.ROOT, capture_output=True, text=True), out

    def link(self, tmp: Path, extra: list[Path], script: Path | None = None):
        res, out = self.run_link(tmp, extra, script)
        self.assertEqual(res.returncode, 0, res.stderr[-2000:])
        nm = subprocess.run([tool("mipsel-none-elf-nm"), str(out)], capture_output=True,
                            text=True, check=True).stdout
        return {p[2]: int(p[0], 16) for p in (l.split() for l in nm.splitlines()) if len(p) == 3}

    def assemble(self, tmp: Path, name: str, source: str) -> Path:
        (tmp / f"{name}.s").write_text(source)
        subprocess.run([tool("mipsel-none-elf-as"), "-EL", "-G8", "-march=r3000",
                        "-o", str(tmp / f"{name}.o"), str(tmp / f"{name}.s")], check=True)
        return tmp / f"{name}.o"

    def with_work_area(self, tmp: Path, size: int) -> Path:
        """main.ld with the work area replaced by one of SIZE bytes."""
        obj = self.assemble(tmp, "work_area", small_work_area(size))
        text = (LINKERS / "main.ld").read_text()
        self.assertEqual(text.count(WORK_AREA), 1)
        script = tmp / "main.ld"
        script.write_text(text.replace(WORK_AREA, f"{obj}(.data*);"))
        return script

    def test_padded_relink_links(self):
        with tempfile.TemporaryDirectory(prefix="shift_pad_") as tmp:
            m = st.measure(0x10, Path(tmp))
        self.assertTrue(m.base_identical)
        self.assertEqual(m.pinned, [])
        self.assertEqual(m.anchored, [f".overlay_windows @ 0x{WINDOW:X}"])
        self.assertEqual(m.headroom, 0)
        # retail leaves no room before the windows: the unchanged script fails loudly
        self.assertTrue(any("run into the first overlay window" in l for l in m.plain_errors or []),
                        m.plain_errors)
        self.assertIsNone(m.shifted_error)
        self.assertEqual(m.counts["unexplained_difference"], 0)
        self.assertEqual(m.counts["unexplained_relocated"], 0)

    def test_growth_past_the_window_fails(self):
        with tempfile.TemporaryDirectory(prefix="ld_growth_") as tmp:
            tmp = Path(tmp)
            res, _ = self.run_link(tmp, [self.assemble(tmp, "probe", GROWTH_PROBE)])
        self.assertNotEqual(res.returncode, 0)
        self.assertIn("main, the field engine and the boot work area run into the first overlay window",
                      res.stderr)

    def test_new_globals_link_and_main_grows(self):
        growth = 4 + 8 + 4 + 4 + 64  # sdata word, two loads, sbss, small, big
        room = 0x100
        with tempfile.TemporaryDirectory(prefix="ld_growth_") as tmp:
            tmp = Path(tmp)
            base = self.link(tmp, [])
            # Give the layout ROOM bytes of headroom by shortening the work area.
            script = self.with_work_area(tmp, WORK_AREA_SIZE - room)
            roomy = self.link(tmp, [], script)
            grown = self.link(tmp, [self.assemble(tmp, "probe", GROWTH_PROBE)], script)
        self.assertEqual(base["_end"], base["main_VRAM_END"])
        self.assertEqual(base["__bss_start"], base["_edata"])
        # small data stays in the gp window, before BSS or with the small pool
        self.assertLess(grown["probe_sdata"], grown["_edata"])
        for name in ("probe_sbss", "probe_small"):
            self.assertGreaterEqual(grown[name], grown["__bss_start"])
            self.assertLess(grown[name], grown["main_main_scommon_OFFSET"])
        self.assertGreaterEqual(grown["probe_big"], grown["main_BSS_START"])
        self.assertLess(grown["probe_big"], grown["_end"])
        self.assertEqual(grown["_end"] - base["_end"], growth)
        # the field engine and the work area follow main in the file and in memory
        for syms in (base, roomy, grown):
            self.assertEqual(syms["field_engine_VRAM"], syms["_end"])
            self.assertEqual(syms["field_engine_ROM_START"],
                             syms["field_engine_VRAM"] - 0x80010000 + syms["main_ROM_START"])
        for name in ("field_engine_VRAM", "FieldFx_TurnToward", "g_FieldFxSlotTable", "g_BootWorkArea"):
            self.assertEqual(grown[name] - base[name], growth, name)
        # the overlay windows stay at their ABI address and file offset
        for syms in (base, roomy, grown):
            self.assertEqual(syms["overlay_windows_VRAM"], WINDOW)
            self.assertEqual(syms["overlay_windows_ROM_START"], WINDOW - 0x80010000 + syms["main_ROM_START"])
            self.assertEqual(syms["g_StrFileDirArea"], base["g_StrFileDirArea"])
        self.assertEqual(base["overlay_windows_HEADROOM"], 0)
        self.assertEqual(roomy["overlay_windows_HEADROOM"], room)
        self.assertEqual(grown["overlay_windows_HEADROOM"], room - growth)


if __name__ == "__main__":
    unittest.main()
