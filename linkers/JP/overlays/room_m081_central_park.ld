SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m081_central_park_ROM_START = __romPos;
    room_m081_central_park_VRAM = ADDR(.room_m081_central_park);
    .room_m081_central_park (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m081_central_park_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m081_central_park_RODATA_START = .;
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/room_m081_central_park.rodata.s.o(.rodata);
        build/JP/overlays/room_m081_central_park/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.rodata);
        . = ALIGN(., 4);
        room_m081_central_park_RODATA_END = .;
        room_m081_central_park_RODATA_SIZE = ABSOLUTE(room_m081_central_park_RODATA_END - room_m081_central_park_RODATA_START);
        room_m081_central_park_TEXT_START = .;
        build/JP/overlays/room_m081_central_park/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m081_central_park_TEXT_END = .;
        room_m081_central_park_TEXT_SIZE = ABSOLUTE(room_m081_central_park_TEXT_END - room_m081_central_park_TEXT_START);
        room_m081_central_park_DATA_START = .;
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/room_m081_central_park.rodata.s.o(.data);
        build/JP/overlays/room_m081_central_park/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.data);
        build/JP/overlays/room_m081_central_park/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/room_m081_central_park/D_80192590.s.o(.data);
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/D_801A520C.data.s.o(.data);
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/room_m081_central_park/D_801A5218.s.o(.data);
        . = ALIGN(., 4);
        room_m081_central_park_DATA_END = .;
        room_m081_central_park_DATA_SIZE = ABSOLUTE(room_m081_central_park_DATA_END - room_m081_central_park_DATA_START);
        room_m081_central_park_BSS_START = .;
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/room_m081_central_park.rodata.s.o(.bss);
        build/JP/overlays/room_m081_central_park/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m081_central_park/asm/JP/overlays/room_m081_central_park/data/RoomLib_HandlerCArgsData_8019159C.rodata.s.o(.bss);
        . = ALIGN(., 4);
        room_m081_central_park_BSS_END = .;
        room_m081_central_park_BSS_SIZE = ABSOLUTE(room_m081_central_park_BSS_END - room_m081_central_park_BSS_START);
    }
    __romPos += SIZEOF(.room_m081_central_park);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m081_central_park_ROM_END = __romPos;
    room_m081_central_park_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
