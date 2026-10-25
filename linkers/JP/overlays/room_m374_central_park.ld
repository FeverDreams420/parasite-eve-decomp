SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m374_central_park_ROM_START = __romPos;
    room_m374_central_park_VRAM = ADDR(.room_m374_central_park);
    .room_m374_central_park (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m374_central_park_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m374_central_park_RODATA_START = .;
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/room_m374_central_park.rodata.s.o(.rodata);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/effect_sequence.c.o(.rodata);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_nop0.c.o(.rodata);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_spawn.c.o(.rodata);
        . = ALIGN(., 4);
        room_m374_central_park_RODATA_END = .;
        room_m374_central_park_RODATA_SIZE = ABSOLUTE(room_m374_central_park_RODATA_END - room_m374_central_park_RODATA_START);
        room_m374_central_park_TEXT_START = .;
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_nop0.c.o(.text);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/effect_sequence.c.o(.text);
        . = ALIGN(., 4);
        room_m374_central_park_TEXT_END = .;
        room_m374_central_park_TEXT_SIZE = ABSOLUTE(room_m374_central_park_TEXT_END - room_m374_central_park_TEXT_START);
        room_m374_central_park_DATA_START = .;
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/room_m374_central_park.rodata.s.o(.data);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/effect_sequence.c.o(.data);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_nop0.c.o(.data);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192B5C.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192B7C.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192B9C.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192BBC.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192BCC.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/RoomLib_TableB_8018F03C.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/g_EffectSequenceLimeGlowRamp.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/g_EffectSequenceAmberGlowRamp.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/g_EffectSequenceGlowSprite.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192C0C.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192C10.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/g_EffectSequenceGlowSpriteSecondary.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192C1C.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192C20.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/g_EffectSequenceFinalGlowSprite.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192C2C.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192C30.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/g_EffectSequenceModel.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/g_EffectSequenceSparkleSprite.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_80192C3C.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/room_m374_central_park/D_80192C40.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/D_801CCE44.data.s.o(.data);
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/room_m374_central_park/D_801CCE50.s.o(.data);
        . = ALIGN(., 4);
        room_m374_central_park_DATA_END = .;
        room_m374_central_park_DATA_SIZE = ABSOLUTE(room_m374_central_park_DATA_END - room_m374_central_park_DATA_START);
        room_m374_central_park_BSS_START = .;
        build/JP/overlays/room_m374_central_park/asm/JP/overlays/room_m374_central_park/data/room_m374_central_park.rodata.s.o(.bss);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/effect_sequence.c.o(.bss);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_nop0.c.o(.bss);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/room_m374_central_park/src/jp/overlays/room_lib/class_spawn.c.o(.bss);
        . = ALIGN(., 4);
        room_m374_central_park_BSS_END = .;
        room_m374_central_park_BSS_SIZE = ABSOLUTE(room_m374_central_park_BSS_END - room_m374_central_park_BSS_START);
    }
    __romPos += SIZEOF(.room_m374_central_park);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m374_central_park_ROM_END = __romPos;
    room_m374_central_park_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
