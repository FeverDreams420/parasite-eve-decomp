SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m089_central_park_ROM_START = __romPos;
    room_m089_central_park_VRAM = ADDR(.room_m089_central_park);
    .room_m089_central_park (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m089_central_park_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m089_central_park_RODATA_START = .;
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/room_m089_central_park.rodata.s.o(.rodata);
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_8018F1CC.rodata.s.o(.rodata);
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_m089_central_park/m089.c.o(.rodata);
        . = ALIGN(., 4);
        room_m089_central_park_RODATA_END = .;
        room_m089_central_park_RODATA_SIZE = ABSOLUTE(room_m089_central_park_RODATA_END - room_m089_central_park_RODATA_START);
        room_m089_central_park_TEXT_START = .;
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_m089_central_park/m089.c.o(.text);
        . = ALIGN(., 4);
        room_m089_central_park_TEXT_END = .;
        room_m089_central_park_TEXT_SIZE = ABSOLUTE(room_m089_central_park_TEXT_END - room_m089_central_park_TEXT_START);
        room_m089_central_park_DATA_START = .;
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/room_m089_central_park.rodata.s.o(.data);
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_8018F1CC.rodata.s.o(.data);
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_m089_central_park/m089.c.o(.data);
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_801940CC.data.s.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_801940F8.data.s.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_80194128.data.s.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_80194160.data.s.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_80194164.data.s.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_80194168.data.s.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/room_m089_central_park/D_8019416C.s.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_801EBE48.data.s.o(.data);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/room_m089_central_park/D_801EBE60.s.o(.data);
        . = ALIGN(., 4);
        room_m089_central_park_DATA_END = .;
        room_m089_central_park_DATA_SIZE = ABSOLUTE(room_m089_central_park_DATA_END - room_m089_central_park_DATA_START);
        room_m089_central_park_BSS_START = .;
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/room_m089_central_park.rodata.s.o(.bss);
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m089_central_park/asm/JP/overlays/room_m089_central_park/data/D_8018F1CC.rodata.s.o(.bss);
        build/JP/overlays/room_m089_central_park/src/jp/overlays/room_m089_central_park/m089.c.o(.bss);
        . = ALIGN(., 4);
        room_m089_central_park_BSS_END = .;
        room_m089_central_park_BSS_SIZE = ABSOLUTE(room_m089_central_park_BSS_END - room_m089_central_park_BSS_START);
    }
    __romPos += SIZEOF(.room_m089_central_park);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m089_central_park_ROM_END = __romPos;
    room_m089_central_park_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
