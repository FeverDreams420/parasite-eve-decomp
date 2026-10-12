import pathlib
import re
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/tests'))
from csource import find_definition, host_unit, psx_compile  # noqa: E402

ROOM = 'src/overlays/room_m273_liberty'
FUNCTION = 'func_801960F4'


class PeriodicDualPools(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef char layout[sizeof(Vector)==8 &&
    (unsigned long)&((RoomM273PoolContext *)0)->pool==8 &&
    (unsigned long)&((RoomM273EffectStateContext *)0)->state==8 &&
    (unsigned long)&((State *)0)->kind==14 &&
    (unsigned long)&((State *)0)->value22==22 &&
    (unsigned long)&((RoomM273BossState *)0)->done==92 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        # The two addresses are aliases within the same PSX allocation.
        source = host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM))
        source = re.sub(r'extern (?:u8|unsigned char) D_8019AEF8;', '', source)
        self.assertIn('extern RoomM273BossState D_8019AE9C;', source)
        source = source.replace('extern RoomM273BossState D_8019AE9C;',
            'extern RoomM273BossState D_8019AE9C;\n#define D_8019AEF8 D_8019AE9C.done\n', 1)
        source = '#include <assert.h>\n#include <string.h>\n#include <limits.h>\n' + source + r'''
RoomM273BossState D_8019AE9C;
static RoomM273PoolContext context;
static RoomM273EffectStateContext stateContext;
static State state,replacement;
FieldFxTaskSlot *D_800F33E0=(FieldFxTaskSlot *)&context;
FieldFxObjectPrefix *D_800F32D0=(FieldFxObjectPrefix *)&stateContext;
static unsigned char primary[64],otherPrimary[64],secondary[64],otherSecondary[64];
void *D_8019AE80=secondary;
unsigned short D_800E11FA,D_800E2850[8];
RenderEffectParameters D_800F3368;
static Vector *primaryLinks[2],*secondaryLinks[2];
static int init,attempts,aux,updates,renders,failPrimary,failSecondary;
int func_8019A398(void) { assert(0); return 0; }
int func_8019A4CC(void) { assert(0); return 0; }
int func_800CE560(char *p,int stride,int count,FieldFxCallbackListCallback callback) {
    assert(init++==0 && p==(char *)primary && stride==4 && count==6 &&
           callback==(FieldFxCallbackListCallback)func_8019A398);
    return 48;
}
int func_800CE5AC(void *p,int offset,int stride,int count,void *callback) {
    assert(init++==1 && p==&D_8019AE80 && offset==48 && stride==4 && count==6 &&
           callback==(void *)func_8019A4CC);
    *(void **)p=secondary; return 32;
}
void *func_800CE610(char *p) {
    int n;
    if(p==primary || p==otherPrimary) {
        n=attempts++;
        assert(n<2 && p==(n==0 ? primary:otherPrimary) && aux==0);
        context.pool=otherPrimary;
        stateContext.state.mode=&replacement; state.value22=100; D_8019AE9C.done=1;
        return n==failPrimary ? 0:&primaryLinks[n];
    }
    n=aux++;
    assert(n<2 && p==(n==0 ? secondary:otherSecondary));
    assert(attempts==(failPrimary<2 ? failPrimary+1:2));
    D_8019AE80=otherSecondary;
    return n==failSecondary ? 0:&secondaryLinks[n];
}
int func_800CE688(void *p) { assert(p==D_8019AE80 && updates++==0); return 0; }
int func_800CE78C(char *p) {
    assert(p==D_8019AE80 && renders++==0);
    D_800E11FA=3; D_800E2850[3]=0xFEDC;
    return 0;
}
int main(void) {
    int timers[]={-32768,-16,-8,-1,0,1,7,8,16,24,32,34,35,40,32767};
    int stops[]={0,1,255};
    int s,k,t,f,g,n,active,pSuccess,sSuccess;
    context.pool=primary;
    assert(func_801960F4(0)==80 && init==2 && D_8019AE80==secondary);
    for(s=0;s<3;++s) for(k=10;k<=12;++k) for(t=0;t<15;++t)
        for(f=0;f<=2;++f) for(g=0;g<=2;++g) {
            D_8019AE9C.done=stops[s]; state.kind=k; state.value22=timers[t];
            stateContext.state.mode=&state; context.pool=primary; D_8019AE80=secondary;
            attempts=aux=updates=renders=0; failPrimary=f; failSecondary=g;
            memset(primaryLinks,0,sizeof(primaryLinks)); memset(secondaryLinks,0,sizeof(secondaryLinks));
            active=!stops[s] && k==11 && timers[t]<35 && !(timers[t]&7);
            pSuccess=active ? f:0; sSuccess=active ? g:0;
            assert(func_801960F4(1)==(stops[s] ? 2:0));
            assert(attempts==(active ? (f<2 ? f+1:2):0));
            assert(aux==(active ? (g<2 ? g+1:2):0));
            assert(updates==(!stops[s] && k==11) && renders==0);
            for(n=0;n<2;++n) {
                assert(primaryLinks[n]==(n<pSuccess ? &D_8019AE9C.hands[n]:0));
                assert(secondaryLinks[n]==(n<sSuccess ? &D_8019AE9C.hands[n]:0));
            }
        }
    for(s=0;s<3;++s) {
        D_8019AE9C.done=stops[s]; renders=0; D_800E11FA=0; D_800E2850[0]=0;
        assert(func_801960F4(2)==0 && renders==1);
        assert(D_800F3368.parameter00==32 && D_800F3368.parameter02==2 && D_800F3368.extent_x==32 && D_800F3368.extent_y==32);
        assert(D_800F3368.palette==3 && D_800F3368.parameter06==1 && D_800F3368.parameter0A==0 && D_800F3368.depth==0 && D_800F3368.tpage==0xFEDC);
    }
    attempts=updates=renders=0;
    assert(func_801960F4(-1)==0 && func_801960F4(3)==0);
    assert(func_801960F4(INT_MIN)==0 && func_801960F4(INT_MAX)==0);
    assert(!attempts && !updates && !renders && init==2);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory) / 'behavior.c'
            path.write_text(source)
            exe = path.with_suffix('')
            subprocess.run(['cc', '-std=gnu11', '-O2', str(path), '-o', str(exe)], check=True)
            subprocess.run([str(exe)], check=True)
