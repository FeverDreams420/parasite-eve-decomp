SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m221_museum_ROM_START = __romPos;
    room_m221_museum_VRAM = ADDR(.room_m221_museum);
    .room_m221_museum (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m221_museum_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m221_museum_RODATA_START = .;
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/room_m221_museum.rodata.s.o(.rodata);
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/spark_ring_burst.c.o(.rodata);
        . = ALIGN(., 4);
        room_m221_museum_RODATA_END = .;
        room_m221_museum_RODATA_SIZE = ABSOLUTE(room_m221_museum_RODATA_END - room_m221_museum_RODATA_START);
        room_m221_museum_TEXT_START = .;
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/spark_ring_burst.c.o(.text);
        . = ALIGN(., 4);
        room_m221_museum_TEXT_END = .;
        room_m221_museum_TEXT_SIZE = ABSOLUTE(room_m221_museum_TEXT_END - room_m221_museum_TEXT_START);
        room_m221_museum_DATA_START = .;
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/room_m221_museum.rodata.s.o(.data);
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/spark_ring_burst.c.o(.data);
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/D_80194078.data.s.o(.data);
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/D_80194098.data.s.o(.data);
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/D_801940B8.data.s.o(.data);
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/D_801940F0.data.s.o(.data);
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/D_80194110.data.s.o(.data);
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/room_m221_museum/RoomSlotA_80193FC0.s.o(.data);
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/D_801B8EAC.data.s.o(.data);
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/room_m221_museum/D_801B8ED0.s.o(.data);
        . = ALIGN(., 4);
        room_m221_museum_DATA_END = .;
        room_m221_museum_DATA_SIZE = ABSOLUTE(room_m221_museum_DATA_END - room_m221_museum_DATA_START);
        room_m221_museum_BSS_START = .;
        build/JP/overlays/room_m221_museum/asm/JP/overlays/room_m221_museum/data/room_m221_museum.rodata.s.o(.bss);
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m221_museum/src/jp/overlays/room_lib/spark_ring_burst.c.o(.bss);
        . = ALIGN(., 4);
        room_m221_museum_BSS_END = .;
        room_m221_museum_BSS_SIZE = ABSOLUTE(room_m221_museum_BSS_END - room_m221_museum_BSS_START);
    }
    __romPos += SIZEOF(.room_m221_museum);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m221_museum_ROM_END = __romPos;
    room_m221_museum_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
