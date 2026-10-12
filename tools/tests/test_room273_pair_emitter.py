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
FUNCTION = 'func_801993F0'


class PairedEmitter(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(FUNCTION, ROOM)
        source = unit.read_text() + r'''
typedef char layout[sizeof(Entry)==8 &&
    (unsigned long)&((Entry *)0)->emitter.flags==6 &&
    (unsigned long)&((EmitterState *)0)->stopped==30 &&
    (unsigned long)&((RoomM273PoolContext *)0)->pool==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / 'layout.o', unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = '#include <assert.h>\n' + host_unit(FUNCTION, path=find_definition(FUNCTION, ROOM)) + r'''
RenderEffectParameters D_800F3368;
#define memset __builtin_memset
struct SceneAssetBlob *D_800B0E64[1];
void *D_8019AF6C;
FieldFxTaskSlot *D_800F33E0;
EmitterState D_8019AF84;
static RoomM273PoolContext context;
static int asset,pool,stage,initResult,allocs,failAt;
static Entry outputs[2];
void *Asset_FindTable08ByU32Key(void *owner,int id) {
    assert(stage++==0 && owner==&context && (unsigned int)id==0xC5541704);
    return &asset;
}
void FieldFx_ModelScrollUv(union FieldTexturedModel *handle,u8 a,u8 b) {
    assert(stage++==1 && (void *)handle==&asset && D_8019AF6C==&asset && !a && !b);
    context.pool=&pool;
}
int func_80198E94(int mode,GteShortVector *p) { assert(0); return 0; }
int func_800CE560(char *p,int size,int count,FieldFxCallbackListCallback callback) {
    assert(stage++==2 && p==(char *)&pool && size==8 && count==4);
    assert(callback==(FieldFxCallbackListCallback)func_80198E94);
    return initResult;
}
void *func_800CE610(char *p) {
    int index=allocs++;
    assert(p==(char *)&pool && index<2);
    if(index==failAt) return 0;
    return &outputs[index];
}
int main(void) {
    int mask,fail,i,next,successful,expectedCalls,stopped;
    D_800B0E64[0]=(struct SceneAssetBlob *)&context;
    D_800F33E0=(FieldFxTaskSlot *)&context;
    for(initResult=-1;initResult<=1;++initResult) {
        stage=0; context.pool=0;
        assert(func_801993F0(0)==initResult && stage==3);
    }
    for(mask=0;mask<4;++mask) for(fail=-1;fail<2;++fail) {
        memset(&D_8019AF84,0xA6,sizeof(D_8019AF84));
        memset(outputs,0x55,sizeof(outputs));
        D_8019AF84.stopped=0;
        for(i=0;i<2;++i) {
            D_8019AF84.entries[i].emitter.x=65535-i;
            D_8019AF84.entries[i].emitter.y=32768+i;
            D_8019AF84.entries[i].emitter.z=123+i;
            D_8019AF84.entries[i].emitter.flags=0xA5A0 | ((mask>>i)&1);
        }
        allocs=0; failAt=fail;
        assert(func_801993F0(1)==0);
        next=successful=expectedCalls=stopped=0;
        for(i=0;i<2;++i) {
            int active=(mask>>i)&1;
            if(active && !stopped) {
                ++expectedCalls;
                if(next==fail) stopped=1;
                else {
                    Entry *out=&outputs[next];
                    assert(out->emitter.x==65535-i && out->emitter.y==32768+i && out->emitter.z==123+i);
                    assert(out->emitter.flags==0);
                    ++successful;
                }
                ++next;
            }
            assert(D_8019AF84.entries[i].emitter.flags==
                   (unsigned short)(0xA5A0 | (active && stopped)));
        }
        assert(allocs==expectedCalls);
        for(i=successful;i<2;++i) assert(outputs[i].emitter.flags==0x5555);
        for(i=0;i<14;++i) assert(D_8019AF84.unknown[i]==0xA6);
        D_8019AF84.stopped=255;
        assert(func_801993F0(1)==2 && allocs==expectedCalls);
    }
    D_800F3368.parameter0A=-1; D_800F3368.depth=123;
    assert(func_801993F0(2)==0 && !D_800F3368.parameter0A && !D_800F3368.depth);
    assert(func_801993F0(-1)==0 && func_801993F0(3)==0);
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
