import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/tests'))
from csource import find_definition, host_unit, psx_compile  # noqa: E402

ROOM = 'src/overlays/room_m273_liberty'
FUNCTION = 'func_80194470'


class DoubleSizedSprite(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef char layout[sizeof(Vector)==8 &&
    (unsigned long)&((Vector *)0)->z==4 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n#include <limits.h>\n' + host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM)) + r'''
typedef GteShortVector Vector;
RenderEffectParameters D_800F3368;
int D_800E27EC, D_800F3428;
GteSineWord D_800966EC[4096];
unsigned short D_800E1204[8];
static Vector position={12,34,56,78};
static int cluts,draws,expectedPalette,expectedSize,shadeIndex;
u_short GetClut(int x, int y) {
    assert(cluts++==0 && !draws && x==0 && y==expectedPalette);
    D_800E27EC=999;
    D_800966EC[shadeIndex].word=0x1234FFBF;
    return 0xF123;
}
void func_800CEE20(struct GteShortVector *p, struct GteRotation *rotation,
                  int w, int h, int tile, int clut, int flag, int shade,
                  struct RenderColor *color) {
    assert(cluts==1 && draws++==0 && p==&position && rotation==0);
    assert(w==expectedSize && h==expectedSize && tile==102);
    assert(clut==0xF123 && flag==1 && shade==-2 && color==0);
}
int main(void) {
    int times[]={INT_MIN,-1,0,8,9,INT_MAX};
    int words[]={0,0x12347FFF,0x12348000,0x1234FFFF};
    int samples[]={0,32767,-32768,-1};
    int i,j,kind,variant,frame,sizeIndex;
    for(i=0;i<6;++i) {
        D_800E27EC=times[i];
        assert(func_80194470(1,&position)==(times[i]>=9));
    }
    assert(!cluts && !draws);
    for(kind=0;kind<8;++kind) for(variant=0;variant<2;++variant)
        for(i=0;i<=9;++i) for(j=0;j<4;++j) {
            D_800E27EC=i; frame=i-1;
            sizeIndex=(((unsigned int)frame<<9)&0x3E00)/4;
            shadeIndex=(((unsigned int)frame<<10)&0x3C00)/4;
            D_800966EC[sizeIndex].word=words[j];
            expectedSize=(short)(samples[j]*2+4096);
            D_800F3368.palette=kind; D_800F3428=variant;
            D_800E1204[kind]=65535;
            expectedPalette=65535+(kind==4 && variant ? 9:5);
            cluts=draws=0;
            assert(func_80194470(2,&position)==0 && cluts==1 && draws==1);
        }
    assert(func_80194470(0,&position)==0);
    assert(func_80194470(-1,&position)==0);
    assert(func_80194470(INT_MAX,&position)==0);
    assert(cluts==1 && draws==1);
    assert(position.x==12 && position.y==34 && position.z==56 && position.pad==78);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=gnu11', '-O2', '-fno-strict-aliasing',
                            str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
