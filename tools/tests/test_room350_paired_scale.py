import pathlib
import shutil
import subprocess
import tempfile
import sys
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / "tools/tests"))
from csource import find_definition, host_unit, psx_compile  # noqa: E402

# Overlay function addresses repeat across rooms; search this room only.
ROOM = 'src/overlays/room_m350'

CONTROLLER = "func_80193BCC"


class PairedScaleTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / "tools/old-gcc/cc1").is_file() and shutil.which("mipsel-none-elf-as"), "PSX tools unavailable")
    def test_target_layout(self):
        unit = find_definition(CONTROLLER, under=ROOM)
        source = unit.read_text() + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(Particle)==4 ? 1:-1];
typedef char b[sizeof(Vector)==8 ? 1:-1];
typedef char c[OFF(Instance,animation)==14 ? 1:-1];
typedef char d[OFF(Instance,frame)==22 ? 1:-1];
typedef char e[OFF(Instance,previousFrame)==26 ? 1:-1];
typedef char f[OFF(Actor,instance)==8 ? 1:-1];
typedef char g[OFF(RoomM350Emitter,pool)==8 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / "layout.o", unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_crossing_precedes_completion(self):
        source = host_unit(CONTROLLER, path=find_definition(CONTROLLER, under=ROOM))
        harness = '#include <assert.h>\n#include <string.h>\n' + source + r'''
FieldAnimObjectPrefix *D_800F32D0;
FieldAnimTaskSlot *D_800F33E0;
GteShortVector D_8019A778[2];
RenderEffectParameters D_800F3368;
u16 D_800E11E4[16];
u16 D_800E2850[65536];
static Actor actor;
static Instance instance;
static RoomM350Emitter emitters[2];
static int pools[2], calls, setups, failAt, setupResult;
static Particle particles[2], expected[2];
int func_80193A80(int e, void **p) { return 0; }
int func_800CE560(char *p,int stride,int count,FieldAnimCallbackListCallback cb) {
    assert(p==(char *)&pools[0] && stride==4 && count==2);
    assert(cb==(FieldAnimCallbackListCallback)func_80193A80);
    setups++;
    return setupResult;
}
void *func_800CE610(char *pool) {
    int index=calls++;
    assert(index<2 && pool==(char *)&pools[index ? 1:0]);
    /* Verify pool reload and absence of a post-allocation frame check. */
    D_800F33E0=(FieldAnimTaskSlot *)&emitters[1];
    instance.frame=65535; instance.previousFrame=65535; instance.animation=0;
    return index==failAt ? 0:&particles[index];
}
static void reset(void) {
    calls=setups=0;
    D_800F32D0=(FieldAnimObjectPrefix *)&actor;
    D_800F33E0=(FieldAnimTaskSlot *)&emitters[0];
    memset(particles,0x5A,sizeof(particles));
    memcpy(expected,particles,sizeof(particles));
}
static void run(int animation,int frame,int previous,int failure) {
    int active=animation==11 && frame>=5 && previous<5;
    int result=animation==11 && !active && frame>=41 ? 2:0;
    int i, successes=active ? (failure<2 ? failure:2):0;
    reset(); failAt=failure;
    instance.animation=animation; instance.frame=frame; instance.previousFrame=previous;
    assert(func_80193BCC(1)==result);
    assert(calls==(active ? (failure==0 ? 1:2):0) && setups==0);
    for(i=0;i<successes;i++) expected[i].position=&D_8019A778[i];
    assert(memcmp(expected,particles,sizeof(particles))==0);
}
int main(void) {
    int i,j,k;
    int edges[]={0,4,5,40,41,32767,32768,65535};
    actor.instance=&instance;
    emitters[0].pool=&pools[0]; emitters[1].pool=&pools[1];
    for(i=0;i<65536;i++) for(k=0;k<3;k++) {
        run(11,i,0,k);
        run(11,i,5,k);
        run(11,5,i,k);
        run(11,65535,i,k);
    }
    for(i=0;i<256;i++) for(j=0;j<8;j++) for(k=0;k<3;k++) run(i,edges[j],0,k);
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); assert(func_80193BCC(0)==setupResult && setups==1 && calls==0);
    }
    for(i=0;i<65536;i++) {
        /* The tpage slot is entry 11 of the texture slot table (0x800E11FA). */
        reset(); D_800E11E4[11]=i; D_800E2850[i]=65535-i;
        memset(&D_800F3368,0x9D,sizeof(D_800F3368));
        assert(func_80193BCC(2)==0 && calls==0 && setups==0);
        assert(D_800F3368.parameter00==32 && D_800F3368.parameter02==2);
        assert(D_800F3368.extent_x==32 && D_800F3368.extent_y==32);
        assert(D_800F3368.parameter0A==0 && D_800F3368.depth==0);
        assert(D_800F3368.palette==3 && D_800F3368.parameter06==1);
        assert(D_800F3368.tpage==65535-i);
    }
    for(i=-3;i<=5;i++) if(i<0 || i>2) {
        reset(); assert(func_80193BCC(i)==0 && calls==0 && setups==0);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "paired"
            result = subprocess.run(["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing", "-x", "c", "-", "-o", str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)
