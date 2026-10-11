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
FUNCTION = 'func_80199568'


class DelayedRisingSprite(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef RoomM273RisingParticle Particle;
typedef char layout[sizeof(Particle)==8 &&
    (unsigned long)&((Particle *)0)->y==2 &&
    (unsigned long)&((Particle *)0)->z==4 &&
    (unsigned long)&((Particle *)0)->speed==6 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n#include <limits.h>\n' + host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM)) + r'''
typedef RoomM273RisingParticle Particle;
int D_800E27EC, D_800F3428;
GteSineWord D_800966EC[4096];
unsigned short D_800E1204[8];
RenderEffectParameters D_800F3368;
static Particle particle;
static int cluts,draws,expectedPalette,expectedTile,expectedShade;
static int sampleIndex,newSample,newScale;
u_short GetClut(int x, int y) {
    assert(cluts++==0 && !draws && x==16 && y==expectedPalette);
    D_800E27EC=999;
    D_800F3368.parameter02=newScale;
    D_800966EC[sampleIndex].word=newSample;
    return 0xF123;
}
void func_800CEE20(struct GteShortVector *p, struct GteRotation *rotation,
                  int w, int h, int tile, int clut, int flag, int shade,
                  struct RenderColor *color) {
    assert(cluts==1 && draws++==0 && (void *)p==&particle && rotation==0);
    assert(w==12288 && h==12288 && tile==expectedTile);
    assert(clut==0xF123 && flag==1 && shade==expectedShade && color==0);
}
int main(void) {
    int times[]={INT_MIN,-1,0,3,4,5,35,36,INT_MAX};
    unsigned short values[]={0,1,32767,32768,65535};
    int words[]={0,0x12347FFF,0x12348000,0x1234FFFF};
    int scales[]={-32768,-1,0,32767};
    int frames[]={0,1,3,4,15,31,32,63,64};
    int i,j,k,kind,variant,f;
    for(i=0;i<9;++i) for(j=0;j<5;++j) for(k=0;k<5;++k) {
        D_800E27EC=times[i]; particle.x=12; particle.z=-34;
        particle.y=values[j]; particle.speed=values[k];
        assert(func_80199568(1,&particle)==(times[i]>=36));
        assert(particle.x==12 && particle.z==-34);
        assert(particle.y==(unsigned short)(times[i]>=4 && times[i]<36
            ? values[j]-values[k]:values[j]));
        assert(particle.speed==(unsigned short)(times[i]>=4 && times[i]<36
            ? values[k]+1:values[k]));
    }
    for(i=-3;i<5;++i) {
        D_800E27EC=i;
        assert(func_80199568(2,&particle)==0);
    }
    assert(!cluts && !draws);
    for(kind=0;kind<8;++kind) for(variant=0;variant<2;++variant)
        for(f=0;f<9;++f) for(j=0;j<4;++j) for(k=0;k<4;++k) {
            D_800E27EC=frames[f]+5;
            sampleIndex=(((unsigned int)frames[f]<<8)&0x3F00)/4;
            D_800966EC[sampleIndex].word=123; D_800F3368.parameter02=123;
            newSample=words[j]; newScale=scales[k];
            expectedShade=(short)newSample >> 5;
            expectedTile=newScale*(frames[f]/4)+200;
            D_800F3368.palette=kind; D_800F3428=variant;
            D_800E1204[kind]=65535;
            expectedPalette=65535+(kind==4 && variant ? 4:0);
            particle.x=12; particle.y=65530; particle.z=-34; particle.speed=8;
            cluts=draws=0;
            assert(func_80199568(2,&particle)==0 && cluts==1 && draws==1);
            assert(particle.x==12 && particle.y==65530 &&
                   particle.z==-34 && particle.speed==8);
        }
    assert(func_80199568(0,&particle)==0);
    assert(func_80199568(-1,&particle)==0);
    assert(func_80199568(INT_MAX,&particle)==0);
    assert(cluts==1 && draws==1);
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
