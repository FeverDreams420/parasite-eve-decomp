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
FUNCTION = 'func_80197A48'


class PositionEmitter(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef char layout[sizeof(Vector)==8 && __alignof__(Vector)==2 &&
    (unsigned long)&((Vector *)0)->y==2 &&
    (unsigned long)&((Vector *)0)->z==4 &&
    (unsigned long)&((Vector *)0)->pad==6 &&
    (unsigned long)&((RoomM273PoolContext *)0)->pool==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n#include <limits.h>\n' + host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM)) + r'''
FieldAnimTaskSlot *D_800F33E0;
unsigned char D_8019AF68,D_8019AF69;
RoomM273SweepPoint D_8019AEFC;
volatile unsigned short D_8019AEFE,D_8019AF00;
unsigned short D_800E11EA,D_800E2850[65536];
RenderEffectParameters D_800F3368;
static RoomM273PoolContext context;
static Vector output;
static int pool,initCalls,initResult,allocs,fail;
int func_801977F8(int mode,Vector *p) { assert(0); return 0; }
int func_800CE560(char *p,int size,int count,FieldAnimCallbackListCallback callback) {
    assert(p==(char *)&pool && size==8 && count==4 &&
           callback==(FieldAnimCallbackListCallback)func_801977F8);
    ++initCalls;
    return initResult;
}
void *func_800CE610(char *p) {
    assert(p==(char *)&pool && allocs++==0);
    if(fail) return 0;
    /* Source coordinates must be read after allocation. */
    D_8019AEFC.x=65535; D_8019AEFE=32768; D_8019AF00=123;
    return &output;
}
int main(void) {
    int stop,emit,i,calls;
    unsigned char flags[]={0,1,255};
    unsigned short indices[]={0,1,32768,65535};
    D_800F33E0=(FieldAnimTaskSlot *)&context; context.pool=&pool;
    for(initResult=-1;initResult<=1;++initResult)
        assert(func_80197A48(0)==initResult);
    for(stop=0;stop<3;++stop) for(emit=0;emit<3;++emit)
        for(fail=0;fail<2;++fail) {
            output.x=11; output.y=22; output.z=33; output.pad=0xA55A;
            D_8019AEFC.x=1; D_8019AEFE=2; D_8019AF00=3;
            D_8019AF68=flags[emit]; D_8019AF69=flags[stop]; allocs=0;
            assert(func_80197A48(1)==(stop ?2:0));
            calls=!stop && emit;
            assert(allocs==calls);
            assert((unsigned short)output.x==(calls && !fail ?65535:11));
            assert((unsigned short)output.y==(calls && !fail ?32768:22));
            assert((unsigned short)output.z==(calls && !fail ?123:33));
            assert((unsigned short)output.pad==0xA55A);
            assert(D_8019AF68==flags[emit] && D_8019AF69==flags[stop]);
        }
    calls=allocs;
    for(i=0;i<4;++i) {
        D_800E11EA=indices[i]; D_800E2850[indices[i]]=65535-i;
        D_800F3368.parameter00=D_800F3368.parameter02=D_800F3368.palette=D_800F3368.parameter06=0xAAAA;
        D_800F3368.tpage=D_800F3368.parameter0A=D_800F3368.depth=0xAAAA;
        D_800F3368.extent_x=D_800F3368.extent_y=0xAAAA;
        assert(func_80197A48(2)==0 && allocs==calls);
        assert(D_800F3368.parameter00==32 && D_800F3368.parameter02==2 && D_800F3368.palette==3);
        assert(D_800F3368.parameter06==0 && D_800F3368.tpage==65535-i && D_800F3368.parameter0A==0);
        assert(D_800F3368.depth==64 && D_800F3368.extent_x==32 && D_800F3368.extent_y==32);
    }
    assert(func_80197A48(-1)==0 && func_80197A48(3)==0);
    assert(func_80197A48(INT_MIN)==0 && func_80197A48(INT_MAX)==0);
    assert(initCalls==3 && allocs==calls);
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
