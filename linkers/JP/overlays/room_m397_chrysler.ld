SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m397_chrysler_ROM_START = __romPos;
    room_m397_chrysler_VRAM = ADDR(.room_m397_chrysler);
    .room_m397_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m397_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m397_chrysler_RODATA_START = .;
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/room_m397_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/trail_burst_attacks.c.o(.rodata);
        . = ALIGN(., 4);
        room_m397_chrysler_RODATA_END = .;
        room_m397_chrysler_RODATA_SIZE = ABSOLUTE(room_m397_chrysler_RODATA_END - room_m397_chrysler_RODATA_START);
        room_m397_chrysler_TEXT_START = .;
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/trail_burst_attacks.c.o(.text);
        . = ALIGN(., 4);
        room_m397_chrysler_TEXT_END = .;
        room_m397_chrysler_TEXT_SIZE = ABSOLUTE(room_m397_chrysler_TEXT_END - room_m397_chrysler_TEXT_START);
        room_m397_chrysler_DATA_START = .;
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/room_m397_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/trail_burst_attacks.c.o(.data);
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801956AC.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801956F8.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_80195720.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_80195758.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801957BC.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801957E4.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_8019581C.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801959AC.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801959B0.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801959B4.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801959B8.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801959D8.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_80195B68.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_80195B6C.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_80195B70.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/room_m397_chrysler/D_80195B74.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/D_801CDE14.data.s.o(.data);
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/room_m397_chrysler/D_801CDE44.s.o(.data);
        . = ALIGN(., 4);
        room_m397_chrysler_DATA_END = .;
        room_m397_chrysler_DATA_SIZE = ABSOLUTE(room_m397_chrysler_DATA_END - room_m397_chrysler_DATA_START);
        room_m397_chrysler_BSS_START = .;
        build/JP/overlays/room_m397_chrysler/asm/JP/overlays/room_m397_chrysler/data/room_m397_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m397_chrysler/src-jp/overlays/room_lib/trail_burst_attacks.c.o(.bss);
        . = ALIGN(., 4);
        room_m397_chrysler_BSS_END = .;
        room_m397_chrysler_BSS_SIZE = ABSOLUTE(room_m397_chrysler_BSS_END - room_m397_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m397_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m397_chrysler_ROM_END = __romPos;
    room_m397_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
