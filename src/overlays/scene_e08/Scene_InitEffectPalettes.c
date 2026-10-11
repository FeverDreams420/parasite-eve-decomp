#include "common.h"
#include "pe1/room_sound_slot.h"
#include "pe1/akao_send.h"
#include "pe1/pe_image.h"
#include "pe1/scene_player_orb.h"
#include "pe1/scene_grab_head.h"
extern FieldGlowSprite D_80199540, D_80199570, D_80199580;
/* The 0x80199560 glow sprite, field by field as Scene_DrawLayeredSprite
 * stores its colour. */
extern u8 D_80199560, D_80199561, D_80199562, D_80199564, D_80199565, D_80199566;
extern s16 D_80199568, D_8019956A;
void func_800C2B40(void *);
extern s16 D_80199558;
extern volatile u8 D_80199555;
extern u8 D_80199554;
extern s16 D_8019955A;
extern u8 D_80199550;
extern u8 D_80199551;
extern u8 D_80199552;
extern u8 D_80199556;
extern s16 D_801994A0;
extern s16 D_801994A2;
extern u8 D_80199490;
extern u8 D_80199491;
extern u8 D_80199492;
extern u8 D_80199494;
extern u8 D_80199495;
extern u8 D_80199496;
extern u8 D_80199498;
extern u8 D_80199499;
extern u8 D_8019949A;
extern u8 D_8019949C;
extern u8 D_8019949D;
extern u8 D_8019949E;

void func_80193C68(SceneGrabHead *actor, void *unused, SceneE08Context *state) {
    FieldActor *source;
    RenderMatrix *block;
    void *sound;
    RoomSoundSlot *soundAddress;
    int sixtyFour;
    int one;
    u16 half128;
    u8 byte128;
    int thirtyTwo;
    register int two asm("$8");
    int minus300;
    int scratch; /* -50 offset, later the sound pan */
    int scale;
    func_800C2B40(state);
    source = actor->actor;
    state->source = source;
    block = source->render_object.matrices;
    state->rootMatrix = *block;
    state->spriteAsset = Asset_SearchByKeyType(0x2E);
    scale = 30;
        sixtyFour = 64;
    one = 1;
    state->hitReload = scale;
    asm volatile("" : : "m"(state->hitReload));
    scratch = -50;
    half128 = 128;
    byte128 = 128;
    thirtyTwo = 32;
    two = 2;
    asm volatile("" : : "r"(two));
    /* hitTimer and hitRequest: as field stores GCC would sink them past
     * the global stores below. */
    *(s16 *)((char *)state + 0x2A) = 0;
    *(s16 *)((char *)state + 0x2C) = 0;
    D_80199558 = scratch;
    D_80199580.offset = -100;
    D_80199580.g = 16;
    D_80199580.b = 5;
    D_80199564 = 70;
    D_80199565 = 3;
    D_80199568 = 1;
    D_80199540.cell = 96;
    D_80199555 = one;
    D_80199580.clut = one;
    soundAddress = (RoomSoundSlot *)D_800B0E64;
    D_80199554 = sixtyFour;
    D_8019955A = half128;
    D_80199550 = byte128;
    D_80199551 = sixtyFour;
    D_80199552 = thirtyTwo;
    D_80199556 = two;
    D_80199580.cell = sixtyFour;
    D_80199580.depth = half128;
    D_80199580.r = thirtyTwo;
    D_80199580.flip = two;
    D_8019956A = half128;
    D_80199560 = byte128;
    D_80199561 = byte128;
    D_80199562 = byte128;
    D_80199566 = 0;
    D_80199540.clut = 7;
    D_80199540.offset = 0;
    D_80199540.depth = half128;
    D_80199540.r = byte128;
    D_80199540.g = byte128;
    D_80199540.b = byte128;
    D_80199540.flip = 0;
    D_80199570.cell = 0;
    D_80199570.clut = 0;
    minus300 = -300;
    D_80199570.offset = minus300;
    D_80199570.depth = half128;
    D_80199570.r = byte128;
    D_80199570.g = byte128;
    D_80199570.b = byte128;
    D_80199570.flip = 0;
    sound = *(void *volatile *)&soundAddress->channel;
    D_801994C8.cell = 68;
    D_801994C8.clut = two;
    D_801994C8.offset = minus300;
    D_801994C8.depth = half128;
    D_801994C8.r = byte128;
    D_801994C8.g = sixtyFour;
    D_801994C8.b = thirtyTwo;
    D_801994C8.flip = 0;
    D_801994A0 = minus300;
    D_801994A2 = half128;
    D_80199490 = 0;
    D_80199491 = 0;
    D_80199492 = 0;
    D_80199494 = 0;
    D_80199495 = 0;
    D_80199496 = 0;
    D_80199498 = byte128;
    D_80199499 = sixtyFour;
    D_8019949A = thirtyTwo;
    D_8019949C = byte128;
    D_8019949D = sixtyFour;
    D_8019949E = thirtyTwo;
    if (sound) {
        scratch = 0x7F;
        Akao_SendTableCommand(soundAddress->channel, 0x604, 0, 0x80, scratch);
    }
}
