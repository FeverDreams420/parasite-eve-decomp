SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m384_chrysler_ROM_START = __romPos;
    room_m384_chrysler_VRAM = ADDR(.room_m384_chrysler);
    .room_m384_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m384_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m384_chrysler_RODATA_START = .;
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/room_m384_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/RoomLib_HandlerCArgsData_801915FC.rodata.s.o(.rodata);
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/window_class.c.o(.rodata);
        . = ALIGN(., 4);
        room_m384_chrysler_RODATA_END = .;
        room_m384_chrysler_RODATA_SIZE = ABSOLUTE(room_m384_chrysler_RODATA_END - room_m384_chrysler_RODATA_START);
        room_m384_chrysler_TEXT_START = .;
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/window_class.c.o(.text);
        . = ALIGN(., 4);
        room_m384_chrysler_TEXT_END = .;
        room_m384_chrysler_TEXT_SIZE = ABSOLUTE(room_m384_chrysler_TEXT_END - room_m384_chrysler_TEXT_START);
        room_m384_chrysler_DATA_START = .;
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/room_m384_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/RoomLib_HandlerCArgsData_801915FC.rodata.s.o(.data);
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/window_class.c.o(.data);
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/actor_window_class_table.c.o(.data);
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/room_m384_chrysler/D_80193394.s.o(.data);
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/room_m384_chrysler/D_801A1668.s.o(.data);
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/D_801B1628.data.s.o(.data);
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/room_m384_chrysler/D_801B1640.s.o(.data);
        . = ALIGN(., 4);
        room_m384_chrysler_DATA_END = .;
        room_m384_chrysler_DATA_SIZE = ABSOLUTE(room_m384_chrysler_DATA_END - room_m384_chrysler_DATA_START);
        room_m384_chrysler_BSS_START = .;
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/room_m384_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m384_chrysler/asm/JP/overlays/room_m384_chrysler/data/RoomLib_HandlerCArgsData_801915FC.rodata.s.o(.bss);
        build/JP/overlays/room_m384_chrysler/src/jp/overlays/room_lib/window_class.c.o(.bss);
        . = ALIGN(., 4);
        room_m384_chrysler_BSS_END = .;
        room_m384_chrysler_BSS_SIZE = ABSOLUTE(room_m384_chrysler_BSS_END - room_m384_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m384_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m384_chrysler_ROM_END = __romPos;
    room_m384_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
