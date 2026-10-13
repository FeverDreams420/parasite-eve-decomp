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


@unittest.skipUnless((BUILD / "main.map").exists() and (LINKERS / "main.ld").exists(),
                     "needs `make build`")
class BuiltLayoutTests(unittest.TestCase):
    def link(self, tmp: Path, extra: list[Path]):
        out = tmp / "main.elf"
        cmd = [tool("mipsel-none-elf-ld"), "-EL", "-T", str(LINKERS / "main.ld"),
               "-T", str(LINKERS / "undefined_syms_manual.txt"),
               "-T", str(LINKERS / "undefined_addr_aliases.main.txt"),
               "-o", str(out), *map(str, extra)]
        res = subprocess.run(cmd, cwd=st.ROOT, capture_output=True, text=True)
        self.assertEqual(res.returncode, 0, res.stderr[-2000:])
        nm = subprocess.run([tool("mipsel-none-elf-nm"), str(out)], capture_output=True,
                            text=True, check=True).stdout
        return {p[2]: int(p[0], 16) for p in (l.split() for l in nm.splitlines()) if len(p) == 3}

    def test_padded_relink_links(self):
        with tempfile.TemporaryDirectory(prefix="shift_pad_") as tmp:
            m = st.measure(0x10, Path(tmp))
        self.assertTrue(m.base_identical)
        self.assertEqual(m.pinned, [])
        self.assertIsNone(m.plain_errors)
        self.assertIsNone(m.shifted_error)
        self.assertEqual(m.counts["unexplained_difference"], 0)
        self.assertEqual(m.counts["unexplained_relocated"], 0)

    def test_new_globals_link_and_main_grows(self):
        with tempfile.TemporaryDirectory(prefix="ld_growth_") as tmp:
            tmp = Path(tmp)
            (tmp / "probe.s").write_text(GROWTH_PROBE)
            subprocess.run([tool("mipsel-none-elf-as"), "-EL", "-G8", "-march=r3000",
                            "-o", str(tmp / "probe.o"), str(tmp / "probe.s")], check=True)
            base = self.link(tmp, [])
            grown = self.link(tmp, [tmp / "probe.o"])
        self.assertEqual(base["_end"], base["main_VRAM_END"])
        self.assertEqual(base["__bss_start"], base["_edata"])
        # small data stays in the gp window, before BSS or with the small pool
        self.assertLess(grown["probe_sdata"], grown["_edata"])
        for name in ("probe_sbss", "probe_small"):
            self.assertGreaterEqual(grown[name], grown["__bss_start"])
            self.assertLess(grown[name], grown["main_main_scommon_OFFSET"])
        self.assertGreaterEqual(grown["probe_big"], grown["main_BSS_START"])
        self.assertLess(grown["probe_big"], grown["_end"])
        growth = 4 + 8 + 4 + 4 + 64  # sdata word, two loads, sbss, small, big
        self.assertEqual(grown["_end"] - base["_end"], growth)
        # the field engine follows main in the file and in memory
        for syms in (base, grown):
            self.assertEqual(syms["field_engine_VRAM"], syms["_end"])
            self.assertEqual(syms["field_engine_ROM_START"],
                             syms["field_engine_VRAM"] - 0x80010000 + syms["main_ROM_START"])


if __name__ == "__main__":
    unittest.main()
