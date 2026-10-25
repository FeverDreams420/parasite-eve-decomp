SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m409_chrysler_ROM_START = __romPos;
    room_m409_chrysler_VRAM = ADDR(.room_m409_chrysler);
    .room_m409_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m409_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m409_chrysler_RODATA_START = .;
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/room_m409_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m409_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.rodata);
        . = ALIGN(., 4);
        room_m409_chrysler_RODATA_END = .;
        room_m409_chrysler_RODATA_SIZE = ABSOLUTE(room_m409_chrysler_RODATA_END - room_m409_chrysler_RODATA_START);
        room_m409_chrysler_TEXT_START = .;
        build/JP/overlays/room_m409_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m409_chrysler_TEXT_END = .;
        room_m409_chrysler_TEXT_SIZE = ABSOLUTE(room_m409_chrysler_TEXT_END - room_m409_chrysler_TEXT_START);
        room_m409_chrysler_DATA_START = .;
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/room_m409_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m409_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.data);
        build/JP/overlays/room_m409_chrysler/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/room_m409_chrysler/D_80192590.s.o(.data);
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/D_801B864C.data.s.o(.data);
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/room_m409_chrysler/D_801B8670.s.o(.data);
        . = ALIGN(., 4);
        room_m409_chrysler_DATA_END = .;
        room_m409_chrysler_DATA_SIZE = ABSOLUTE(room_m409_chrysler_DATA_END - room_m409_chrysler_DATA_START);
        room_m409_chrysler_BSS_START = .;
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/room_m409_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m409_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m409_chrysler/asm/JP/overlays/room_m409_chrysler/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.bss);
        . = ALIGN(., 4);
        room_m409_chrysler_BSS_END = .;
        room_m409_chrysler_BSS_SIZE = ABSOLUTE(room_m409_chrysler_BSS_END - room_m409_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m409_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m409_chrysler_ROM_END = __romPos;
    room_m409_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
