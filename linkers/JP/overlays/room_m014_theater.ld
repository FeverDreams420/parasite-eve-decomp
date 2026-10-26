SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m014_theater_ROM_START = __romPos;
    room_m014_theater_VRAM = ADDR(.room_m014_theater);
    .room_m014_theater (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m014_theater_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m014_theater_RODATA_START = .;
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/room_m014_theater.rodata.s.o(.rodata);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m014_theater_RODATA_END = .;
        room_m014_theater_RODATA_SIZE = ABSOLUTE(room_m014_theater_RODATA_END - room_m014_theater_RODATA_START);
        room_m014_theater_TEXT_START = .;
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        room_m014_theater_TEXT_END = .;
        room_m014_theater_TEXT_SIZE = ABSOLUTE(room_m014_theater_TEXT_END - room_m014_theater_TEXT_START);
        room_m014_theater_DATA_START = .;
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/room_m014_theater.rodata.s.o(.data);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/D_80193214.data.s.o(.data);
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/D_80193250.data.s.o(.data);
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/RoomSlotA_80193148.data.s.o(.data);
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/RoomSlotB_80193148.data.s.o(.data);
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/room_m014_theater/RoomSlotC_80193148.s.o(.data);
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/D_801AF644.data.s.o(.data);
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/room_m014_theater/D_801AF65C.s.o(.data);
        . = ALIGN(., 4);
        room_m014_theater_DATA_END = .;
        room_m014_theater_DATA_SIZE = ABSOLUTE(room_m014_theater_DATA_END - room_m014_theater_DATA_START);
        room_m014_theater_BSS_START = .;
        build/JP/overlays/room_m014_theater/asm/JP/overlays/room_m014_theater/data/room_m014_theater.rodata.s.o(.bss);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m014_theater/src-jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m014_theater_BSS_END = .;
        room_m014_theater_BSS_SIZE = ABSOLUTE(room_m014_theater_BSS_END - room_m014_theater_BSS_START);
    }
    __romPos += SIZEOF(.room_m014_theater);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m014_theater_ROM_END = __romPos;
    room_m014_theater_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
