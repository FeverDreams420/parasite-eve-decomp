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
FUNCTION = 'func_801939B4'


class LatchedPlayer(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef char layout[sizeof(RoomM273SpritePoolEffect)==4 &&
    sizeof(RoomM273WorldPosition)==12 &&
    (unsigned long)&((RoomM273PoolContext *)0)->pool==8 &&
    (unsigned long)&((FieldActor *)0)->mode==14 &&
    (unsigned long)&((FieldActor *)0)->render_object.matrices==0x238 &&
    (unsigned long)&((RoomM273PlayerTransform *)0)->position==20 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM)) + r'''
static RoomM273PoolContext context;
static FieldActor owner,replacement;
static RoomM273PlayerTransform transform;
static RoomM273SpritePoolEffect output;
static RoomM273WorldPosition sentinel;
static unsigned char pool[32];
FieldFxTaskSlot *D_800F33E0=(FieldFxTaskSlot *)&context;
FieldActor *g_PlayerEntity=&owner;
int D_800E27EC;
unsigned char D_8019AE5C;
unsigned short D_800E11FA,D_800E2850[8];
RenderEffectParameters D_800F3368;
static int init,allocations,fail;
int func_80193870(int mode, RoomM273SpritePoolEffect *state) { assert(0); return 0; }
int func_800CE560(char *p,int stride,int count,FieldFxCallbackListCallback callback) {
    assert(init++==0 && p==(char *)pool && stride==4 && count==1 &&
           callback==(FieldFxCallbackListCallback)func_80193870);
    assert(D_8019AE5C==0); return 36;
}
void *func_800CE610(char *p) {
    assert(p==(char *)pool && allocations++==0 && D_8019AE5C==0);
    g_PlayerEntity=&replacement;
    return fail ? 0 : &output;
}
int main(void) {
    int counters[]={INT_MIN,-1,0,19,20,INT_MAX};
    int latches[]={0,1,255};
    int i,l,k,f;
    context.pool=pool; replacement.render_object.matrices=(RenderMatrix *)&transform;
    D_8019AE5C=255;
    assert(func_801939B4(0)==36 && init==1);
    for(i=0;i<6;++i) for(l=0;l<3;++l) for(k=0;k<256;++k) for(f=0;f<2;++f) {
        int stopped=counters[i]>=20 || latches[l]!=0;
        int wanted=!stopped && k<4;
        D_800E27EC=counters[i]; D_8019AE5C=latches[l]; owner.mode=k;
        g_PlayerEntity=&owner; owner.render_object.matrices=0; allocations=0; fail=f;
        output.position=&sentinel;
        assert(func_801939B4(1)==(stopped ? 2:0) && allocations==wanted);
        assert(D_8019AE5C==(wanted && !f ? 1:latches[l]));
        assert(output.position==(wanted && !f ? &transform.position:&sentinel));
    }
    for(l=0;l<3;++l) for(i=0;i<8;++i) {
        int enabled=latches[l]!=0;
        D_8019AE5C=latches[l]; D_800E11FA=i; D_800E2850[i]=65535-i;
        D_800F3368.parameter00=D_800F3368.parameter02=D_800F3368.palette=D_800F3368.parameter06=99;
        D_800F3368.tpage=D_800F3368.parameter0A=D_800F3368.depth=D_800F3368.extent_x=D_800F3368.extent_y=99;
        assert(func_801939B4(2)==0);
        assert(D_800F3368.parameter00==(enabled?32:99) && D_800F3368.parameter02==(enabled?2:99));
        assert(D_800F3368.palette==(enabled?3:99) && D_800F3368.parameter06==(enabled?1:99));
        assert(D_800F3368.tpage==(enabled?65535-i:99));
        assert(D_800F3368.parameter0A==(enabled?0:99) && D_800F3368.depth==(enabled?0:99));
        assert(D_800F3368.extent_x==(enabled?32:99) && D_800F3368.extent_y==(enabled?32:99));
        assert(D_8019AE5C==latches[l]);
    }
    allocations=0;
    assert(func_801939B4(-1)==0 && func_801939B4(3)==0);
    assert(func_801939B4(INT_MIN)==0 && func_801939B4(INT_MAX)==0);
    assert(allocations==0 && init==1);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=gnu11', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
