"""Full linked-byte regression for four Psy-Q LIBPAD/LIBDS functions."""
import hashlib
import pathlib
import shutil
import tempfile
import sys
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/tests'))
from csource import linked_range_bytes  # noqa: E402

# Entire linked text ranges at their retail addresses.
CASES = [('libpad/padportd_4',
  928,
  '155be32d071e436ea5ebfa2786d5cb1de439d92dabae661b6d5f2eaf2dffae69',
  'bzero = 0x80071A24;\nD_8009B76C = 0x8009B76C;\nD_800A5B70 = 0x800A5B70;\nSECTIONS { .text 0x800847a0 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libds/dssys1_vsync',
  720,
  '7df21a00b56e81e4d4a811bd38215db6ff65f4246173802e2e0beb262734fabd',
  'D_8009B598 = 0x8009B598;\nLIBDS_DSSYS_1_text_774 = 0x800800F4;\nD_8009B594 = 0x8009B594;\nD_8009B6A4 = 0x8009B6A4;\nLIBDS_DSSYS_1_text_368 = 0x8007FCFC;\nD_800A36A0 = 0x800A36A0;\ng_DsReadSysEnabled = 0x8009B554;\nD_8009B574 = 0x8009B574;\nD_8009B570 = 0x8009B570;\nSECTIONS { .text 0x8007fe24 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libds/dsready_2',
  732,
  'dd749c65e05714e3bd6130961ed0cc79bdbbef7acfedd2fd2622c3e6f21af5a7',
  'DS_lastmode = 0x8007FC18;\nDS_lastpos = 0x8007FC28;\nD_8009B6EC = 0x8009B6EC;\nDsCommand = 0x8007EE84;\nDsDataCallback = 0x800824F0;\nDsGetSector = 0x80080AE4;\nDsPosToInt = 0x80080C48;\nDsQueueLen = 0x8007F778;\nDsReadyCallback = 0x800824C8;\nDsStartCallback = 0x800824DC;\nER_retry = 0x80082204;\ng_DsReadBusy = 0x8009B70C;\nSECTIONS { .text 0x80081e70 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }'),
 ('libpad/padif_3',
  892,
  'a435b3916bf94b1ffe701eb59664b0f9a9b3b57710343a2f3e1a7f39070089c3',
  '_padSioRW = 0x800830DC;\nD_8009B724 = 0x8009B724;\nD_8009B72C = 0x8009B72C;\nD_8009B730 = 0x8009B730;\nD_8009B744 = 0x8009B744;\nD_8009B748 = 0x8009B748;\nD_8009B758 = 0x8009B758;\nD_8009B764 = 0x8009B764;\nD_8009B770 = 0x8009B770;\nD_8009B77C = 0x8009B77C;\nD_8009B79C = 0x8009B79C;\nD_8009B7A0 = 0x8009B7A0;\nMemCard_WaitReadyForTransfer = 0x800834E8;\nMemCard_WaitStatusBit2 = 0x80083578;\nMemCard_WriteByte = 0x800832B4;\nsetRC2wait = 0x80084FC4;\nSECTIONS { .text 0x80084168 : SUBALIGN(4) { *(.text .text.*) } /DISCARD/ : { *(.reginfo) *(.mdebug) } }')]


class PsyqPadDsCallbacksFourTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("mipsel-none-elf-ld"), "MIPS binutils unavailable")
    def test_all_linked_bytes_match_retail(self):
        for name, size, digest, script in CASES:
            with self.subTest(function=name), tempfile.TemporaryDirectory() as directory:
                work = pathlib.Path(directory)
                symbols, sections = script.split("SECTIONS", 1)
                address = int(sections.split()[2], 16)
                # Units churn; link whichever current units cover the range.
                code = linked_range_bytes(address, size, symbols, work)
                self.assertEqual(len(code), size)
                self.assertEqual(hashlib.sha256(code).hexdigest(), digest)
