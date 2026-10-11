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
FUNCTION = 'func_80198B1C'


class GroundLayerSprite(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef char layout[sizeof(Vector)==8 && __alignof__(Vector)==2 &&
    (unsigned long)&((Vector *)0)->y==2 &&
    (unsigned long)&((Vector *)0)->pad==6 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n#include <limits.h>\n' + host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM)) + r'''
int D_800E27EC,D_800F3428;
short D_800966EE[8192];
unsigned short D_800E1204[8];
RoomFloorY g_RoomFloorY[1];
RenderEffectParameters D_800F3368;
GteRotation D_8019AB68;
unsigned char D_8019AE18[8],D_8019AB70[8];
static Vector input={12,-34,56,78};
static Vector *local;
static int stage,expectedPalette,expectedSize,expectedShade,sizeIndex,shadeIndex;
u_short GetClut(int x,int y) {
    assert(stage++==0 && x==0 && y==expectedPalette);
    D_800E27EC=999;
    D_800966EE[sizeIndex]=111; D_800966EE[shadeIndex]=222;
    return 0xF123;
}
void func_800CEE20(struct GteShortVector *p, struct GteRotation *rotation,
                  int w, int h, int tile, int clut, int flag, int shade,
                  struct RenderColor *color) {
    assert(stage++==1 && p!=&input && !rotation);
    assert(p->x==12 && p->y==-34 && p->z==56 && p->pad==78);
    assert(w==expectedSize && h==expectedSize && tile==100 && clut==0xF123);
    assert(flag==1 && shade==expectedShade && !color);
    local=p; p->x=-111; p->y=222; p->z=-333; p->pad=444;
    g_RoomFloorY->raw=0xFEDC;
}
void func_800D004C(struct GteShortVector *p,int w,int h,int mode,
                  struct GteRotation *rotation,int sx,int sy,
                  struct RenderColor *a,struct RenderColor *b,int shade,int flag) {
    assert(stage++==2 && p==local && w==384 && h==384 && mode==8);
    assert(p->x==-111 && p->y==(short)0xFEDC && p->z==-333 && p->pad==444);
    assert(rotation==&D_8019AB68 && sx==4096 && sy==4096);
    assert((void *)a==D_8019AE18 && (void *)b==D_8019AB70 && shade==expectedShade && flag==1);
}
int main(void) {
    int times[]={INT_MIN,-1,0,31,32,INT_MAX};
    int frames[]={-2,-1,0,1,31,32,127,128,4096};
    short samples[]={-32768,-4097,-4096,-1,0,32767};
    int i,j,k,kind,variant;
    unsigned int phase;
    for(i=0;i<6;++i) {
        D_800E27EC=times[i];
        assert(func_80198B1C(1,&input)==(times[i]>=32));
    }
    assert(stage==0);
    for(kind=0;kind<8;++kind) for(variant=0;variant<2;++variant)
        for(i=0;i<9;++i) for(j=0;j<6;++j) for(k=0;k<6;++k) {
            D_800E27EC=frames[i]+1; phase=(unsigned int)frames[i]<<5;
            sizeIndex=(phase&4095)*2; shadeIndex=((phase+1024)&4095)*2;
            D_800966EE[sizeIndex]=samples[j]; D_800966EE[shadeIndex]=samples[k];
            expectedSize=(short)(samples[j]*2+4096);
            expectedShade=(short)((unsigned int)(samples[k]+4096)>>5);
            D_800F3368.palette=kind; D_800F3428=variant; D_800E1204[kind]=65535;
            expectedPalette=65535+(kind==4 && variant ?8:4);
            stage=0; g_RoomFloorY->raw=123;
            assert(func_80198B1C(2,&input)==0 && stage==3);
            assert(input.x==12 && input.y==-34 && input.z==56 && input.pad==78);
        }
    assert(func_80198B1C(0,&input)==0 && func_80198B1C(-1,&input)==0);
    assert(stage==3);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=gnu11', '-O2', str(path), '-o', str(exe)],
                           check=True)
            subprocess.run([str(exe)], check=True)
