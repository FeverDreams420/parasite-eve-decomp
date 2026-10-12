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
FUNCTION = 'func_80198CD4'


class GatedPairEmitter(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef char layout[sizeof(Record)==8 && __alignof__(Record)==2 &&
    (unsigned long)&((RoomM273SwayState *)0)->done==46 &&
    (unsigned long)&((RoomM273PoolContext *)0)->pool==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n' + host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM)) + r'''
#define memcmp __builtin_memcmp
FieldFxTaskSlot *D_800F33E0;
RoomM273SwayState D_8019AF74;
unsigned short D_800E11FA,D_800E2850[65536];
RenderEffectParameters D_800F3368;
static RoomM273PoolContext context;
static int pool,initCalls,initResult,allocs,failAt;
static Record outputs[2];
int func_80198B1C(int mode,Vector *p) { assert(0); return 0; }
int func_800CE560(char *p,int size,int count,FieldFxCallbackListCallback callback) {
    assert(p==(char *)&pool && size==8 && count==4 &&
           callback==(FieldFxCallbackListCallback)func_80198B1C);
    ++initCalls;
    return initResult;
}
void *func_800CE610(char *p) {
    int index=allocs++;
    assert(p==(char *)&pool && index<2);
    if(index==failAt) return 0;
    /* The record must be captured after allocation, not before it. */
    D_8019AF74.points[index].pad=0xFE00+index;
    return &outputs[index];
}
int main(void) {
    int a,b,c,stop,fail,i,eligible,calls,success;
    short states[]={-32768,14,15,16,32767};
    short counts[]={-32768,3,4,32767};
    unsigned short indices[]={0,1,32768,65535};
    D_800F33E0=(FieldFxTaskSlot *)&context; context.pool=&pool;
    for(initResult=-1;initResult<=1;++initResult)
        assert(func_80198CD4(0)==initResult);
    for(a=0;a<5;++a) for(b=0;b<4;++b) for(c=0;c<4;++c)
        for(stop=0;stop<2;++stop) for(fail=-1;fail<2;++fail) {
            memset(&D_8019AF74,0xA6,sizeof(D_8019AF74));
            memset(outputs,0x55,sizeof(outputs));
            D_8019AF74.done=stop ?255:0;
            D_8019AF74.animation=states[a]; D_8019AF74.frame=counts[b]; D_8019AF74.frame_1A=counts[c];
            for(i=0;i<2;++i) {
                D_8019AF74.points[i].x=65535-i;
                D_8019AF74.points[i].y=32768+i;
                D_8019AF74.points[i].z=123+i;
            }
            allocs=0; failAt=fail;
            assert(func_80198CD4(1)==(stop ?2:0));
            eligible=!stop && states[a]==15 && counts[b]>=4 && counts[c]<4;
            calls=eligible ? (fail==0 ?1:2):0;
            success=eligible ? (fail<0 ?2:fail):0;
            assert(allocs==calls);
            for(i=0;i<success;++i) {
                assert(memcmp(&outputs[i],&D_8019AF74.points[i],8)==0);
                assert(outputs[i].values[3]==0xFE00+i);
            }
            for(i=success;i<2;++i) assert(outputs[i].values[0]==0x5555);
            for(i=16;i<46;++i) if(i<0x20 || i>=0x26)
                assert(((unsigned char *)&D_8019AF74)[i]==0xA6);
            assert(D_8019AF74.animation==states[a] && D_8019AF74.frame==counts[b]);
            assert(D_8019AF74.frame_1A==counts[c]);
        }
    for(i=0;i<4;++i) {
        D_800E11FA=indices[i]; D_800E2850[indices[i]]=65535-i;
        assert(func_80198CD4(2)==0);
        assert(D_800F3368.parameter00==32 && D_800F3368.parameter02==2 && D_800F3368.palette==3);
        assert(D_800F3368.parameter06==1 && D_800F3368.tpage==65535-i && D_800F3368.parameter0A==0);
        assert(D_800F3368.depth==32 && D_800F3368.extent_x==32 && D_800F3368.extent_y==32);
    }
    assert(func_80198CD4(-1)==0 && func_80198CD4(3)==0 && initCalls==3);
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
