import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
ROOM = 'src/overlays/room_m350_cruiser'
sys.path.insert(0, str(ROOT / "tools/tests"))
from csource import host_unit  # noqa: E402


class Room350EffectFallTests(unittest.TestCase):
    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_motion_texture_and_post_lookup_state(self):
        self.check_variant("func_801937B4", "D_8019A464", 8192, 1)

    @unittest.skipUnless(shutil.which("cc"), "host C compiler unavailable")
    def test_accelerated_motion_texture_and_post_lookup_state(self):
        self.check_variant("func_80197594", "D_8019A62C", 6144, 2)

    def check_variant(self, callback, colors, size, increment):
        source = host_unit(callback, under=ROOM)
        definitions = (f"#define CALLBACK {callback}\n#define COLORS {colors}\n"
                       f"#define SIZE {size}\n#define INCREMENT {increment}\n")
        harness = '#include <assert.h>\n#include <string.h>\n' + definitions + source + r'''
int D_800E27EC, D_800F3428;
RenderColor COLORS;
RenderEffectParameters D_800F3368;
#define D_800F336C D_800F3368.palette
#define D_800F336A D_800F3368.parameter02
unsigned short D_800E1204[8];
/* The shade is the high half of a sine-table record (bits.cos). */
GteSineWord D_800966EC[4097];
#define D_800966EE ((short *)D_800966EC + 1)
static short position[4];
static int lookups, draws, handleValue, nextCounter, expectedShade, expectedScale;
u_short GetClut(int mode, int texture) {
    assert(lookups++ == 0 && draws == 0 && mode == 0);
    assert(texture == D_800E1204[D_800F336C] +
           (D_800F336C == 4 && D_800F3428 ? 6 : 2));
    D_800E27EC = nextCounter;
    D_800F336A = expectedScale;
    return handleValue;
}
void func_800CEE20(struct GteShortVector *point, struct GteRotation *a, int b,
                   int c, int scale, int texture, int flag, int shade,
                   struct RenderColor *colors) {
    assert(draws++ == 0 && lookups == 1 && point == position);
    assert(a == 0 && b == SIZE && c == SIZE && flag == 1);
    assert(scale == expectedScale * (((nextCounter - 1) >> 1) & 7));
    assert(texture == (unsigned short)handleValue && shade == expectedShade >> 5);
    assert(colors == &COLORS);
}
int main(void) {
    const short values[] = {-32768, -33, -1, 0, 32, 32767};
    short before[4], expected[4];
    int counter, event, y, v, kind, special, h, s, i;
    assert(sizeof(GteSineWord) == 4);
    for (counter = -16; counter <= 32; ++counter)
    for (event = -1; event <= 3; ++event)
    for (y = 0; y < 6; ++y) for (v = 0; v < 6; ++v) {
        int result = 0;
        if (event == 2) continue;
        position[0] = 11; position[1] = values[y];
        position[2] = 22; position[3] = values[v];
        memcpy(expected, position, sizeof(position));
        if (event == 1) {
            if (counter >= 16) result = 1;
            else {
                expected[1] = (short)(position[1] - position[3]);
                expected[3] = (short)(position[3] + INCREMENT);
            }
        }
        D_800E27EC = counter; lookups = draws = 0;
        assert(CALLBACK(event, position) == result);
        assert(memcmp(position, expected, sizeof(position)) == 0);
        assert(lookups == 0 && draws == 0);
    }
    for (counter = 0; counter <= 32; ++counter)
    for (kind = 0; kind < 8; ++kind) for (special = 0; special < 2; ++special)
    for (h = 0; h < 2; ++h) for (s = 0; s < 6; ++s) {
        unsigned int index = ((unsigned int)counter * 256 & 0x3F00) / 2;
        for (i = 0; i < 8194; ++i) ((short *)D_800966EC)[i] = 12345;
        for (i = 0; i < 8; ++i) D_800E1204[i] = h ? 65535 : i * 100;
        D_800E27EC = counter; nextCounter = counter + 1;
        D_800F336C = kind; D_800F3428 = special;
        D_800F336A = 999; expectedScale = values[s]; expectedShade = values[5-s];
        D_800966EE[index] = expectedShade;
        handleValue = h ? -1 : 0x12345;
        memcpy(before, position, sizeof(position));
        lookups = draws = 0;
        assert(CALLBACK(2, position) == 0);
        assert(lookups == 1 && draws == 1);
        assert(memcmp(position, before, sizeof(position)) == 0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "room350-effect-fall"
            result = subprocess.run(
                ["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing",
                 "-x", "c", "-", "-o", str(exe)],
                input=harness, text=True, capture_output=True,
            )
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=10)
