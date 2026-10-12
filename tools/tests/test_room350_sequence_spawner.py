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
ROOM = 'src/overlays/room_m350_cruiser'

SPAWNER = "func_80194CFC"


class SequenceSpawnerTests(unittest.TestCase):
    @unittest.skipUnless((ROOT / "tools/old-gcc/cc1").is_file() and shutil.which("mipsel-none-elf-as"), "PSX tools unavailable")
    def test_target_layout(self):
        unit = find_definition(SPAWNER, under=ROOM)
        source = unit.read_text() + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef RoomM350Particle Particle;
typedef RoomM350EffectInstance Instance;
typedef char a[sizeof(Particle)==28 ? 1:-1];
typedef char b[sizeof(GteShortVector)==8 ? 1:-1];
typedef char c[OFF(Instance,animation)==14 ? 1:-1];
typedef char d[OFF(Instance,frame)==22 ? 1:-1];
typedef char e[OFF(Instance,previousFrame)==26 ? 1:-1];
typedef char f[OFF(Instance,yaw)==58 ? 1:-1];
typedef char g[OFF(RoomM350EffectActor,instance)==8 ? 1:-1];
typedef char h[OFF(RoomM350EffectEmitter,pool)==8 ? 1:-1];
typedef char i[OFF(Particle,motion.value.spawn.phase)==12 ? 1:-1];
typedef char j[OFF(Particle,tail.value.spawn.size)==16 ? 1:-1];
typedef char k[OFF(Particle,tail.value.spawn.state)==24 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory) / "layout.o", unit)
            self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    @unittest.skipUnless(shutil.which("cc"), "host compiler unavailable")
    def test_initialization_and_crossing(self):
        harness = '#include <assert.h>\n#include <string.h>\n' + host_unit(SPAWNER, path=find_definition(SPAWNER, under=ROOM)) + r'''
typedef RoomM350Particle Particle;
FieldAnimObjectPrefix *D_800F32D0;
FieldAnimTaskSlot *D_800F33E0;
GteShortVector D_8019A7A0;
unsigned char D_8019A804;
short D_8019A802;
RenderEffectParameters D_800F3368;
u16 D_800E11E4[16];
u16 D_800E2850[65536];
static RoomM350EffectActor actors[2];
static RoomM350EffectInstance instances[2];
static RoomM350EffectEmitter emitter;
static int pool, calls, setups, setupResult, countAfterAllocation;
static Particle particle, expected;
static int half(int value) {
    unsigned int bits=(unsigned int)value & 65535;
    return bits<32768 ? (int)bits:(int)bits-65536;
}
int func_801947BC(int e, Particle *p) { return 0; }
int func_800CE560(char *p,int stride,int count,FieldAnimCallbackListCallback cb) {
    assert(p==(char *)&pool && stride==28 && count==3);
    assert(cb==(FieldAnimCallbackListCallback)func_801947BC);
    setups++;
    return setupResult;
}
void *func_800CE610(char *p) {
    assert(p==(char *)&pool && calls++==0);
    /* Allocation is assumed successful by the target. Probe helper reloads. */
    D_800F32D0=(FieldAnimObjectPrefix *)&actors[1]; D_8019A802=countAfterAllocation; D_8019A804=255;
    D_8019A7A0.x=-32768; D_8019A7A0.y=32767; D_8019A7A0.z=-123; D_8019A7A0.pad=99;
    return &particle;
}
static void reset(void) {
    calls=setups=0;
    D_800F32D0=(FieldAnimObjectPrefix *)&actors[0]; D_800F33E0=(FieldAnimTaskSlot *)&emitter;
    D_8019A802=123; D_8019A804=0;
    memset(&particle,0x5A,sizeof(particle)); expected=particle;
    memset(&D_8019A7A0,0,sizeof(D_8019A7A0));
}
static void run(int animation,int frame,int previous,int stop) {
    int active=!stop && animation==13 && frame>=15 && previous<15;
    reset(); D_8019A804=stop;
    instances[0].animation=animation; instances[0].frame=frame; instances[0].previousFrame=previous;
    assert(func_80194CFC(1)==(stop ? 2:0));
    assert(calls==active && setups==0);
    if(active) {
        expected.position=D_8019A7A0; expected.position.pad=16;
        expected.motion.value.spawn.speed=512;
        expected.motion.value.spawn.yaw=half(instances[1].yaw);
        expected.motion.value.spawn.phase=0;
        expected.tail.value.spawn.duration=16; expected.tail.value.spawn.frame=0;
        expected.tail.value.spawn.state=0;
        expected.tail.value.spawn.size=half(100-countAfterAllocation*20);
        assert(D_8019A802==half(countAfterAllocation+1) && D_8019A804==255);
    } else assert(D_8019A802==123 && D_8019A804==stop);
    assert(memcmp(&expected,&particle,sizeof(particle))==0);
}
int main(void) {
    int i,j;
    int edges[]={0,14,15,32767,32768,65535};
    emitter.pool=&pool;
    actors[0].instance=&instances[0]; actors[1].instance=&instances[1];
    instances[0].yaw=1; instances[1].yaw=65535;
    countAfterAllocation=5;
    for(i=0;i<65536;i++) { run(13,i,0,0); run(13,15,i,0); }
    for(i=0;i<256;i++) for(j=0;j<6;j++) {
        run(i,edges[j],0,0); run(13,edges[j],0,i);
    }
    for(i=-32768;i<=32767;i++) {
        countAfterAllocation=i; instances[1].yaw=(unsigned short)i;
        run(13,15,0,0);
    }
    for(setupResult=-2;setupResult<=2;setupResult++) {
        reset(); assert(func_80194CFC(0)==setupResult && setups==1 && calls==0);
        assert(D_8019A802==123);
    }
    for(i=0;i<65536;i++) {
        /* Texture slot 3 of D_800E11E4 is the halfword at 0x800E11EA. */
        reset(); D_800E11E4[3]=i; D_800E2850[i]=65535-i;
        memset(&D_800F3368,0x9D,sizeof(D_800F3368));
        assert(func_80194CFC(2)==0 && calls==0 && setups==0);
        assert(D_800F3368.parameter00==32 && D_800F3368.parameter02==2);
        assert(D_800F3368.extent_x==32 && D_800F3368.extent_y==32);
        assert(D_800F3368.parameter0A==0 && D_800F3368.depth==64);
        assert(D_800F3368.palette==3 && D_800F3368.parameter06==0);
        assert(D_800F3368.tpage==65535-i && D_8019A802==123);
    }
    for(i=-3;i<=5;i++) if(i<0 || i>2) {
        reset(); assert(func_80194CFC(i)==0 && calls==0 && setups==0);
        assert(memcmp(&expected,&particle,sizeof(particle))==0 && D_8019A802==123);
    }
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            exe = pathlib.Path(directory) / "sequence"
            result = subprocess.run(["cc", "-std=gnu11", "-O2", "-fno-strict-aliasing", "-x", "c", "-", "-o", str(exe)], input=harness, text=True, capture_output=True)
            self.assertEqual(result.returncode, 0, result.stderr)
            subprocess.run([str(exe)], check=True, timeout=20)
