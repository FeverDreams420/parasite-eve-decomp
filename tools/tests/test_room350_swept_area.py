import pathlib
import shutil
import subprocess
import sys
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT / 'tools/tests'))
from csource import find_definition, host_unit, psx_compile  # noqa: E402

# Overlay function addresses repeat across rooms; search this room only.
ROOM = 'src/overlays/room_m350_cruiser'

EMITTER = 'func_80198400'
# Keep pe1/gte.h's asm out of the host build; the screen-space clip test is
# a recorded COP2 boundary.
HOST_GTE = r'''
#include "pe1/gte_types.h"
#define PE1_GTE_H
static void screen(int,int),clip(int *);
#define gte_ldsxy0(x) screen(0,x)
#define gte_ldsxy1(x) screen(1,x)
#define gte_ldsxy2(x) screen(2,x)
#define gte_nclip() ((void)0)
#define gte_stmac0(p) clip(p)
'''


class SweptAreaBehavior(unittest.TestCase):
    def test_target_layout(self):
        unit = find_definition(EMITTER, under=ROOM)
        source = unit.read_text() + r'''
#define OFF(t,f) ((unsigned long)&((t *)0)->f)
typedef char a[sizeof(GteShortVector)==8 && OFF(GteShortVector,z)==4 ? 1:-1];
typedef char b[sizeof(GteMatrix)==32 && OFF(GteMatrix,t)==20 ? 1:-1];
typedef char c[OFF(RoomM350EffectActor,instance)==8 && OFF(RoomM350EffectEmitter,pool)==8 ? 1:-1];
typedef char d[OFF(FieldActor,mode)==0xE && OFF(FieldActor,anim.parts.integer)==0x16 ? 1:-1];
typedef char e[OFF(FieldActor,render_object.model_matrix)==0x1E8 && OFF(FieldActor,render_object.matrices)==0x238 ? 1:-1];
typedef char f[OFF(FieldActorState,core_flags)==0 && OFF(FieldActorState,flags)==0x4C ? 1:-1];
typedef char g[OFF(RoomM350FlareParticle,y)==0 && OFF(RoomM350FlareParticle,z)==2 && OFF(RoomM350FlareParticle,shade)==4 ? 1:-1];
'''
        with tempfile.TemporaryDirectory() as directory:
            result = psx_compile(source, pathlib.Path(directory)/'layout.o', unit)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)

    @unittest.skipUnless(shutil.which('cc'), 'host compiler unavailable')
    def test_behavior(self):
        source = host_unit(EMITTER, path=find_definition(EMITTER, under=ROOM), prefix=HOST_GTE)
        # The side endpoint records are two GteShortVector-sized words; map
        # their halfword symbols onto host arrays.
        for line in ('extern volatile short D_8019A898,D_8019A8A0;',
                     'extern volatile short D_8019A89C,D_8019A89E,D_8019A8A4;',
                     'extern short D_8019A8AE;'):
            self.assertIn(line, source)
            source = source.replace(line, '')
        prefix = r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
static short rightRecord[4],leftRecord[4];
#define D_8019A898 (*(volatile short *)&rightRecord[0])
#define D_8019A8A0 (*(volatile short *)&leftRecord[0])
#define D_8019A89C (*(volatile short *)&rightRecord[2])
#define D_8019A89E (*(volatile short *)&rightRecord[3])
#define D_8019A8A4 (*(volatile short *)&leftRecord[2])
#define D_8019A8AE D_8019A8A8.pad
'''
        body = r'''
typedef GteShortVector Vector;
typedef RoomM350FlareParticle Particle;
FieldAnimObjectPrefix *D_800F32D0;
FieldAnimTaskSlot *D_800F33E0;
FieldActor *g_PlayerEntity;
int D_800E27EC;
unsigned char D_8019A8C0;
GteShortVector D_8019A8A8,D_8019A8B0;
short D_8019A890[3];
RenderEffectParameters D_800F3368;
u16 D_800E11EA,D_800E2850[65536];
static RoomM350EffectActor actors[2];
static FieldActor instances[2],players[2];
static FieldActorState owners[3];
static RoomM350EffectEmitter emitter;
static RenderMatrix transforms[16];
static Particle particle;
static Vector oldRight,oldLeft,quad[4];
static int allocations,randoms,matrices,clips,screens,randomValue,failAlloc,outsideAt,mutate;
static int initCalls,packed[3],expectedPoint;
static int word(uint32_t u) { int x; memcpy(&x,&u,4); return x; }
static int half(unsigned u) { return (u&65535)<32768 ? (int)(u&65535):(int)(u&65535)-65536; }
static int xz(int x,int z) { return word(((uint32_t)z<<16)|(uint16_t)x); }
int func_801981D0(int event,RoomM350FlareParticle *p) { (void)event;(void)p; assert(0); return 0; }
int func_800CE560(char *pool,int stride,int count,FieldAnimCallbackListCallback callback) {
    assert(++initCalls==1 && pool==(char *)&particle && stride==8 && count==4);
    assert(callback==(FieldAnimCallbackListCallback)func_801981D0);
    return -17;
}
void *func_800CE610(char *pool) {
    assert(++allocations==1 && pool==(char *)&particle);
    if (mutate) D_800F32D0=(FieldAnimObjectPrefix *)&actors[1];
    return failAlloc ? 0:&particle;
}
int Inv_ScrambleGrid(void) { assert(++randoms==1 && allocations==1); return randomValue; }
GteShortVector *ApplyMatrixSV(const GteMatrix *m,const GteShortVector *in,GteShortVector *out) {
    RenderMatrix *model=&instances[0].render_object.model_matrix;
    assert(++matrices==1 && (const void *)m==model && in==out);
    assert(in->x==256 && in->y==0 && in->z==0);
    assert(!memcmp(&D_8019A8A8,&oldRight,8) && !memcmp(&D_8019A8B0,&oldLeft,8));
    out->x=-32768; out->y=123; out->z=-1;
    if(mutate) {
        D_800F32D0=(FieldAnimObjectPrefix *)&actors[1];
        model->translation[0]=INT_MAX;
        model->translation[2]=INT_MIN;
    }
    quad[0]=oldRight; quad[1]=oldLeft; quad[2]=oldLeft; quad[3]=oldRight;
    quad[0].x=half((unsigned)model->translation[0]+32768u);
    quad[0].z=half((unsigned)model->translation[2]+65535u);
    quad[0].pad=1;
    quad[1].x=half((unsigned)model->translation[0]-32768u);
    quad[1].z=half((unsigned)model->translation[2]-65535u);
    return out;
}
static void screen(int slot,int value) {
    static const int order[3]={0,2,1};
    assert(slot==order[screens%3]); screens++;
    packed[slot]=value;
}
static void clip(int *area) {
    int previous=clips ? clips-1:3;
    assert(screens==3*(clips+1));
    assert(packed[0]==expectedPoint);
    assert(packed[1]==xz(quad[clips].x,quad[clips].z));
    assert(packed[2]==xz(quad[previous].x,quad[previous].z));
    *area=clips==outsideAt ? -1:0;
    if(mutate) g_PlayerEntity=&players[1];
    clips++;
}
static void reset(void) {
    int i;
    memset(instances,0,sizeof(instances)); memset(players,0,sizeof(players));
    memset(owners,0,sizeof(owners)); memset(transforms,0,sizeof(transforms));
    for(i=0;i<2;i++) {
        actors[i].instance=(RoomM350EffectInstance *)&instances[i];
        players[i].state=&owners[i+1];
    }
    instances[0].state=&owners[0]; instances[0].render_object.matrices=transforms;
    instances[0].mode=9; instances[0].anim.parts.integer=10;
    instances[0].render_object.model_matrix.translation[0]=123456;
    instances[0].render_object.model_matrix.translation[2]=-765432;
    players[0].render_object.model_matrix.translation[0]=123;
    players[0].render_object.model_matrix.translation[2]=-456;
    expectedPoint=xz(123,-456);
    emitter.pool=&particle; D_800F33E0=(FieldAnimTaskSlot *)&emitter;
    D_800F32D0=(FieldAnimObjectPrefix *)&actors[0]; g_PlayerEntity=&players[0];
    oldRight=(Vector){100,222,300,1}; oldLeft=(Vector){-100,333,-300,444};
    memcpy(rightRecord,&oldRight,8); memcpy(leftRecord,&oldLeft,8);
    memset(&D_8019A8A8,0x55,8); memset(&D_8019A8B0,0x66,8);
    D_800E27EC=0; D_8019A8C0=0; particle=(Particle){-7,-8,999};
    initCalls=allocations=randoms=matrices=clips=screens=failAlloc=mutate=0; outsideAt=-1; randomValue=-1;
}
int main(void) {
    int i,j,k;
    reset(); assert(func_80198400(0)==-17 && initCalls==1 && !allocations && !matrices);
    reset(); assert(func_80198400(-1)==0 && !allocations && !matrices);
    for(i=31;i<65536;i++) {
        reset(); instances[0].anim.parts.integer=i;
        assert(func_80198400(1)==2 && !allocations && !matrices && !clips);
    }
    for(i=0;i<256;i++) {
        reset(); randomValue=word(0xFEDC0000u+(unsigned)i); D_8019A8C0=1;
        assert(func_80198400(1)==0 && allocations==1 && randoms==1 && !matrices);
        assert(particle.y==i && particle.z==64+i/4 && particle.shade==999);
    }
    for(i=-1;i<4;i++) for(j=0;j<2;j++) for(k=0;k<2;k++) {
        reset(); outsideAt=i; mutate=j; if(k) instances[0].state=0;
        assert(func_80198400(1)==0 && allocations==1 && randoms==1 && matrices==1);
        assert(clips==(i<0 ? 4:i+1));
        assert(!memcmp(rightRecord,&quad[0],8));
        assert(!memcmp(leftRecord,&quad[1],8));
        assert(D_8019A8C0==(i<0));
        assert(owners[j+1].flags==(i<0 ? 0x4000u:0));
        assert(owners[0].core_flags==(i<0 && !k ? 0x80000000u:0));
    }
    reset(); failAlloc=1;
    assert(func_80198400(1)==0 && allocations==1 && !randoms && matrices==1 && clips==4);
    reset(); D_800E27EC=-1; instances[0].mode=8;
    assert(func_80198400(1)==0 && !allocations && !matrices);
    reset(); oldRight.pad=0; memcpy(rightRecord,&oldRight,8);
    assert(func_80198400(1)==0 && matrices==1 && !clips && !D_8019A8C0 && D_8019A89E==1);
    reset(); D_800E11EA=65535; D_800E2850[65535]=0xFEDC;
    memset(&D_800F3368,0x9D,sizeof(D_800F3368));
    for(i=0;i<3;i++) { transforms[11].translation[i]=-3-i; transforms[15].translation[i]=i; }
    assert(func_80198400(2)==0 && !allocations && !matrices);
    for(i=0;i<3;i++) assert(D_8019A890[i]==-2);
    assert(D_800F3368.parameter00==32 && D_800F3368.parameter02==2);
    assert(D_800F3368.extent_x==32 && D_800F3368.extent_y==32);
    assert(D_800F3368.palette==3 && !D_800F3368.parameter06 && !D_800F3368.parameter0A);
    assert(!D_800F3368.depth && D_800F3368.tpage==0xFEDC);
    return 0;
}
'''
        with tempfile.TemporaryDirectory() as directory:
            path = pathlib.Path(directory)/'test.c'
            path.write_text(prefix+source+body)
            executable = path.with_suffix('')
            result = subprocess.run(['cc','-std=gnu11','-O2','-fwrapv','-fno-strict-aliasing',str(path),'-o',str(executable)],capture_output=True,text=True)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)
            result = subprocess.run([str(executable)],capture_output=True,text=True,timeout=30)
            self.assertEqual(result.returncode,0,result.stdout+result.stderr)


if __name__ == '__main__':
    unittest.main()
