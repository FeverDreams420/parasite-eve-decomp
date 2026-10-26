SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m049_nypd_ROM_START = __romPos;
    room_m049_nypd_VRAM = ADDR(.room_m049_nypd);
    .room_m049_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m049_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m049_nypd_RODATA_START = .;
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/room_m049_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/effect_sequence.c.o(.rodata);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_nop0.c.o(.rodata);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.rodata);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.rodata);
        . = ALIGN(., 4);
        room_m049_nypd_RODATA_END = .;
        room_m049_nypd_RODATA_SIZE = ABSOLUTE(room_m049_nypd_RODATA_END - room_m049_nypd_RODATA_START);
        room_m049_nypd_TEXT_START = .;
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_nop0.c.o(.text);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.text);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/effect_sequence.c.o(.text);
        . = ALIGN(., 4);
        room_m049_nypd_TEXT_END = .;
        room_m049_nypd_TEXT_SIZE = ABSOLUTE(room_m049_nypd_TEXT_END - room_m049_nypd_TEXT_START);
        room_m049_nypd_DATA_START = .;
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/room_m049_header.rodata.s.o(.data);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/effect_sequence.c.o(.data);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_nop0.c.o(.data);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.data);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192B5C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192B7C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192B9C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192BBC.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192BE8.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/g_EffectSequenceLimeGlowRamp.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/g_EffectSequenceAmberGlowRamp.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/g_EffectSequenceGlowSprite.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192C0C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192C10.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/g_EffectSequenceGlowSpriteSecondary.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192C1C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192C20.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/g_EffectSequenceFinalGlowSprite.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192C2C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192C30.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/g_EffectSequenceModel.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/g_EffectSequenceSparkleSprite.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192C3C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80192C40.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80193A44.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_8019688C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_80197594.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/room_m049_nypd/D_80197744.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_801C1D5C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/room_m049_nypd/D_801C2B88.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_801CB008.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_801CDFB8.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/room_m049_nypd/D_801CEFE8.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_801E031C.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_801E18B0.data.s.o(.data);
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/D_801E1F60.data.s.o(.data);
        . = ALIGN(., 4);
        room_m049_nypd_DATA_END = .;
        room_m049_nypd_DATA_SIZE = ABSOLUTE(room_m049_nypd_DATA_END - room_m049_nypd_DATA_START);
        room_m049_nypd_BSS_START = .;
        build/JP/overlays/room_m049_nypd/asm/JP/overlays/room_m049_nypd/data/room_m049_header.rodata.s.o(.bss);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/effect_sequence.c.o(.bss);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_nop0.c.o(.bss);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_plant_script.c.o(.bss);
        build/JP/overlays/room_m049_nypd/src-jp/overlays/room_lib/class_spawn.c.o(.bss);
        . = ALIGN(., 4);
        room_m049_nypd_BSS_END = .;
        room_m049_nypd_BSS_SIZE = ABSOLUTE(room_m049_nypd_BSS_END - room_m049_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m049_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m049_nypd_ROM_END = __romPos;
    room_m049_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
