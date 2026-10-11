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
FUNCTION = 'func_801945A8'


class ThresholdBurst(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef char layout[sizeof(Vector)==8 &&
    (unsigned long)&((RoomM273PoolContext *)0)->pool==8 &&
    (unsigned long)&((RoomM273EffectStateContext *)0)->state==8 &&
    (unsigned long)&((RoomM273EffectModeState *)0)->kind==14 &&
    (unsigned long)&((RoomM273EffectModeState *)0)->value22==22 &&
    (unsigned long)&((RoomM273EffectModeState *)0)->value26==26 &&
    (unsigned long)&((FieldActor *)0)->render_object.matrices==0x238 &&
    (unsigned long)&((RoomM273PlayerTransform *)0)->position.x==20 &&
    (unsigned long)&((RoomM273PlayerTransform *)0)->position.y==24 &&
    (unsigned long)&((RoomM273PlayerTransform *)0)->position.z==28 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n#include <limits.h>\n' + host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM)) + r'''
#define memcmp __builtin_memcmp
typedef RoomM273EffectModeState State;
static RoomM273PoolContext context;
static RoomM273EffectStateContext stateContext;
static State state;
static FieldActor owner,replacement;
static RoomM273PlayerTransform transform;
static Vector output;
static unsigned char pool[64];
FieldAnimTaskSlot *D_800F33E0=(FieldAnimTaskSlot *)&context;
FieldAnimObjectPrefix *D_800F32D0=(FieldAnimObjectPrefix *)&stateContext;
FieldActor *g_PlayerEntity=&owner;
unsigned char D_8019AE9A;
short D_8019AE60,D_8019AE64;
unsigned short D_800E11FA,D_800E2850[8];
RenderEffectParameters D_800F3368;
static int init,allocations,fail;
int func_80194470(int mode, Vector *input) { assert(0); return 0; }
int func_800CE560(char *p,int stride,int count,FieldAnimCallbackListCallback callback) {
    assert(init++==0 && p==(char *)pool && stride==8 && count==4 &&
           callback==(FieldAnimCallbackListCallback)func_80194470);
    assert(D_8019AE60==0 && D_8019AE64==0); return 60;
}
void *func_800CE610(char *p) {
    assert(p==(char *)pool && allocations++==0);
    g_PlayerEntity=&replacement;
    return fail ? 0 : &output;
}
int main(void) {
    int values[]={-32768,-1,0,1,2,32767};
    int counts[]={-1,0,1,4};
    int stop,kind,i,j,c,d,f;
    context.pool=pool; stateContext.state.mode=&state;
    replacement.render_object.matrices=(RenderMatrix *)&transform;
    transform.position.x=65535; transform.position.y=-65537; transform.position.z=32768;
    D_8019AE60=9; D_8019AE64=7;
    assert(func_801945A8(0)==60 && init==1);
    for(stop=0;stop<2;++stop) for(kind=8;kind<=9;++kind)
        for(i=0;i<6;++i) for(j=0;j<6;++j) for(c=0;c<4;++c)
        for(d=0;d<6;++d) for(f=0;f<2;++f) {
            State before;
            short count=counts[c],delay=values[d];
            int wanted=0;
            memset(&state,0xA5,sizeof(state));
            state.kind=kind; state.value22=values[i]; state.value26=values[j]; before=state;
            D_8019AE9A=stop; D_8019AE60=count; D_8019AE64=delay;
            if(!stop) {
                if(kind==9 && values[i]>=2 && values[j]<2) { count=4; delay=0; }
                if(count) {
                    short old=delay; delay=(short)(delay-1);
                    if(old<=0) { wanted=1; if(!f) { delay=2; --count; } }
                }
            }
            allocations=0; fail=f; g_PlayerEntity=&owner;
            memset(&output,0xA5,sizeof(output));
            assert(func_801945A8(1)==(stop ? 2:0));
            assert(allocations==wanted && D_8019AE60==count && D_8019AE64==delay);
            assert(memcmp(&state,&before,sizeof(state))==0 && output.pad==(short)0xA5A5);
            if(wanted && !f) assert(output.x==-1 && output.y==-1 && output.z==-32768);
            else assert(output.x==(short)0xA5A5 && output.y==(short)0xA5A5 && output.z==(short)0xA5A5);
        }
    for(i=0;i<8;++i) {
        D_800E11FA=i; D_800E2850[i]=65535-i;
        assert(func_801945A8(2)==0);
        assert(D_800F3368.parameter00==32 && D_800F3368.parameter02==2 && D_800F3368.palette==3 && D_800F3368.parameter06==1);
        assert(D_800F3368.tpage==65535-i && D_800F3368.parameter0A==0 && D_800F3368.depth==32);
        assert(D_800F3368.extent_x==32 && D_800F3368.extent_y==32);
    }
    allocations=0;
    assert(func_801945A8(-1)==0 && func_801945A8(3)==0);
    assert(func_801945A8(INT_MIN)==0 && func_801945A8(INT_MAX)==0);
    assert(!allocations && init==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=gnu11', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
