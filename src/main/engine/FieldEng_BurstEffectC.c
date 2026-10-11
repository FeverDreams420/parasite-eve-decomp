#include "common.h"
#include "pe1/field_engine_slot.h"
#include "pe1/akao/commands.h"
#include "pe1/field_sprite_draw.h"
#include "pe1/field_sprite_state.h"
#include "pe1/field_glow_sprite.h"
#include "pe1/field_anim.h"
#include "pe1/game_audio_state.h"
#include "pe1/field_anim_particle.h"
#include "pe1/field_billboard.h"
#include "pe1/field_actor.h"

extern int D_800E0FFC;
extern int D_800E22B8;
extern int D_800E22BC;
extern int D_800E22C0;
extern short D_800E22B0;
extern short D_800E22B2;
extern short D_800E22B4;
extern u8 D_800E22C8;
extern u8 D_800E22C9;
extern u8 D_800E22CA;
extern u8 D_800E22CD;
extern short D_800E22CE;
int func_800CE084(char *object)
{
  void **slot = FieldEng_GetSlot(object);
  register int value;
  register void *slotData;
  slotData = &D_800E0FFC;
 do { *slot = slotData; value = 0x300; D_800E22B8 = value; D_800E22BC = value; } while (0);
  D_800E22C0 = value;
  value = 5;
  D_800E22CD = value;
  value = -0x64;
  D_800E22CE = value;
  value = 0x80;
  D_800E22B0 = 0;
  D_800E22B2 = 0;
  D_800E22B4 = 0;
  D_800E22C8 = value;
  D_800E22C9 = value;
  D_800E22CA = value;
  return 0;
}

int func_800CE118(int a, int b, int c, int d, int e, int f) {
    FieldEng_Spawn6(a, b, c, d, e, f);
    return 0;
}

extern char D_800E0FC0[];

int func_800CE144(void *o) {
    FieldEng_Register(o, D_800E0FC0);
    return 0;
}

extern char D_800E0FCC[];
extern char D_800E0FB4[];
extern char D_800E0FD8[];
extern int func_800CE1DC(char *arg0);

int func_800CE16C(void *obj) {
    int first;
    int second;
    int status;

    first = func_800C251C(obj, D_800E0FCC);
    second = func_800C2758(obj, D_800E0FB4, D_800E0FD8);
    status = first | second;
    if (status == -1) {
        func_800CE1DC(obj);
    }
    return 0;
}

int func_800CE1DC(char *arg0) {
    *arg0 = 4;
    return 0;
}

int func_800CE1EC(void) {
    return 0;
}

int func_800CE1F4(void) {
    return 0;
}

int *func_800C2B10(int index);
int *func_800C2B28(int index);

extern int D_800E2848;

int func_800CE1FC(void) {
    int index;
    int *table;
    int *entry;
    int *next;
    FieldActor *data;

    index = *func_800C2B10(0xD);
    table = (int *)*func_800C2B28(1);
    entry = (int *)table[index];
    index++;
    next = (int *)table[index];
    D_800E2848 = (int)entry;

    if (next == 0) {
        *func_800C2B10(0xE) = 1;
    }

    *func_800C2B10(0xD) = index;

    data = (FieldActor *)D_800E2848;
    D_800E2808.x = data->render_object.target_x;
    D_800E2808.y = data->render_object.target_y;
    D_800E2808.z = data->render_object.target_z;

    func_800CEDA8(0);
}

int func_800CE2B4(void *arg0, void *arg1, FieldAnimTimedSpark *anim) {
    int randomX;
    int randomY;
    u16 base;
    int randomZ;

    randomX = rand();
    base = D_800E2808.x - 0x28;
    anim->position.x = base + (randomX % 80);

    randomY = rand();
    base = D_800E2808.y - 0x28;
    anim->position.y = base + (randomY % 80);

    randomZ = rand();
    base = D_800E2808.z;

    anim->brightness = 0x7F;
    anim->reserved04 = 0;
    anim->age = 0;
    base -= 0x28;
    anim->position.z = base + (randomZ % 80);
}

void func_800CE3AC(void) {
}

extern u16 D_800E22A8;
extern u16 D_800E22AA;
extern u16 D_800E22AC;
extern short D_800E22D0;
extern signed char D_800E22CC;

int func_800CE3B4(void *arg0, void *arg1, u8 *anim) {
    u16 value_v0;
    FieldBillboard *output;
    func_800C2EAC(3);
    func_800C3098(0x10);
    func_800C2FF0(0x20, 0x20);
    func_800C3238(2);

    value_v0 = *(u16 *)(anim + 0x6);
    output = (FieldBillboard *)&D_800E22A8;
    output->position.x = value_v0;
    D_800E22AA = *(u16 *)(anim + 0x8);
    D_800E22AC = *(u16 *)(anim + 0xA);
    D_800E22D0 = (signed char)anim[1];
    D_800E22CC = (anim[3] * 2) - 0x60;

    func_800C3B04(output);
    func_800C3098(0x10);
}

int func_800CE464(void *arg0, char *arg1) {
    int value;

    value = 2;
    arg1[1] = value;
    return value;
}

void func_800CE470(void *arg0, char *arg1, FieldAnimTimedSpark *anim) {
    anim->age++;
    if ((signed char)anim->age == 6) {
        arg1[1] = 2;
    }
}
