SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m404_chrysler_ROM_START = __romPos;
    room_m404_chrysler_VRAM = ADDR(.room_m404_chrysler);
    .room_m404_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m404_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m404_chrysler_RODATA_START = .;
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/room_m404_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/RoomLib_HandlerCArgsData_801915F4.rodata.s.o(.rodata);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_m404_chrysler/m404.c.o(.rodata);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/script_arg.c.o(.rodata);
        . = ALIGN(., 4);
        room_m404_chrysler_RODATA_END = .;
        room_m404_chrysler_RODATA_SIZE = ABSOLUTE(room_m404_chrysler_RODATA_END - room_m404_chrysler_RODATA_START);
        room_m404_chrysler_TEXT_START = .;
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_m404_chrysler/m404.c.o(.text);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/script_arg.c.o(.text);
        . = ALIGN(., 4);
        room_m404_chrysler_TEXT_END = .;
        room_m404_chrysler_TEXT_SIZE = ABSOLUTE(room_m404_chrysler_TEXT_END - room_m404_chrysler_TEXT_START);
        room_m404_chrysler_DATA_START = .;
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/room_m404_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/RoomLib_HandlerCArgsData_801915F4.rodata.s.o(.data);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_m404_chrysler/m404.c.o(.data);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/script_arg.c.o(.data);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/D_80193EDC.data.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/D_80193EFC.data.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/D_80193F1C.data.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/D_80193F54.data.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/D_80193F68.data.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/D_80193F88.data.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/room_m404_chrysler/RoomSlotA_80193E24.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/D_801D0160.data.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/room_m404_chrysler/D_801D0184.s.o(.data);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/D_801D01E0.data.s.o(.data);
        . = ALIGN(., 4);
        room_m404_chrysler_DATA_END = .;
        room_m404_chrysler_DATA_SIZE = ABSOLUTE(room_m404_chrysler_DATA_END - room_m404_chrysler_DATA_START);
        room_m404_chrysler_BSS_START = .;
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/room_m404_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m404_chrysler/asm/JP/overlays/room_m404_chrysler/data/RoomLib_HandlerCArgsData_801915F4.rodata.s.o(.bss);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_m404_chrysler/m404.c.o(.bss);
        build/JP/overlays/room_m404_chrysler/src-jp/overlays/room_lib/script_arg.c.o(.bss);
        . = ALIGN(., 4);
        room_m404_chrysler_BSS_END = .;
        room_m404_chrysler_BSS_SIZE = ABSOLUTE(room_m404_chrysler_BSS_END - room_m404_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m404_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m404_chrysler_ROM_END = __romPos;
    room_m404_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
