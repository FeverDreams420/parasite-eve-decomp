SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m234_museum_ROM_START = __romPos;
    room_m234_museum_VRAM = ADDR(.room_m234_museum);
    .room_m234_museum (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m234_museum_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m234_museum_RODATA_START = .;
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/room_m234_museum.rodata.s.o(.rodata);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/sound_burst.c.o(.rodata);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/spark_ring_burst.c.o(.rodata);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m234_museum_RODATA_END = .;
        room_m234_museum_RODATA_SIZE = ABSOLUTE(room_m234_museum_RODATA_END - room_m234_museum_RODATA_START);
        room_m234_museum_TEXT_START = .;
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/sound_burst.c.o(.text);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/script_args.c.o(.text);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/spark_ring_burst.c.o(.text);
        . = ALIGN(., 4);
        room_m234_museum_TEXT_END = .;
        room_m234_museum_TEXT_SIZE = ABSOLUTE(room_m234_museum_TEXT_END - room_m234_museum_TEXT_START);
        room_m234_museum_DATA_START = .;
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/room_m234_museum.rodata.s.o(.data);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/sound_burst.c.o(.data);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/spark_ring_burst.c.o(.data);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_801947C8.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_801947EC.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_80194824.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_80194844.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_80194864.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_8019489C.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_801948BC.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/RoomSlotA_80192C68.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/RoomSlotB_80192C68.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/RoomSlotC_80192C68.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/room_m234_museum/RoomSlotA_80194710.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_801D01E0.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/room_m234_museum/D_801D01F8.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/D_801E5AD0.data.s.o(.data);
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/room_m234_museum/D_801E5B00.s.o(.data);
        . = ALIGN(., 4);
        room_m234_museum_DATA_END = .;
        room_m234_museum_DATA_SIZE = ABSOLUTE(room_m234_museum_DATA_END - room_m234_museum_DATA_START);
        room_m234_museum_BSS_START = .;
        build/JP/overlays/room_m234_museum/asm/JP/overlays/room_m234_museum/data/room_m234_museum.rodata.s.o(.bss);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/sound_burst.c.o(.bss);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/spark_ring_burst.c.o(.bss);
        build/JP/overlays/room_m234_museum/src-jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m234_museum_BSS_END = .;
        room_m234_museum_BSS_SIZE = ABSOLUTE(room_m234_museum_BSS_END - room_m234_museum_BSS_START);
    }
    __romPos += SIZEOF(.room_m234_museum);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m234_museum_ROM_END = __romPos;
    room_m234_museum_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
