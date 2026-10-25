SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m116_nypd_ROM_START = __romPos;
    room_m116_nypd_VRAM = ADDR(.room_m116_nypd);
    .room_m116_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m116_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m116_nypd_RODATA_START = .;
        build/JP/overlays/room_m116_nypd/asm/JP/overlays/room_m116_nypd/data/room_m116_nypd.rodata.s.o(.rodata);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/line_burst.c.o(.rodata);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/script_slot.c.o(.rodata);
        . = ALIGN(., 4);
        room_m116_nypd_RODATA_END = .;
        room_m116_nypd_RODATA_SIZE = ABSOLUTE(room_m116_nypd_RODATA_END - room_m116_nypd_RODATA_START);
        room_m116_nypd_TEXT_START = .;
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/line_burst.c.o(.text);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/script_slot.c.o(.text);
        . = ALIGN(., 4);
        room_m116_nypd_TEXT_END = .;
        room_m116_nypd_TEXT_SIZE = ABSOLUTE(room_m116_nypd_TEXT_END - room_m116_nypd_TEXT_START);
        room_m116_nypd_DATA_START = .;
        build/JP/overlays/room_m116_nypd/asm/JP/overlays/room_m116_nypd/data/room_m116_nypd.rodata.s.o(.data);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/line_burst.c.o(.data);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m116_nypd/asm/JP/overlays/room_m116_nypd/data/D_801933D8.data.s.o(.data);
        build/JP/overlays/room_m116_nypd/asm/JP/overlays/room_m116_nypd/data/room_m116_nypd/RoomLib_SlotTable.s.o(.data);
        build/JP/overlays/room_m116_nypd/asm/JP/overlays/room_m116_nypd/data/room_m116_nypd/D_801A0E58.s.o(.data);
        build/JP/overlays/room_m116_nypd/asm/JP/overlays/room_m116_nypd/data/D_801D01E0.data.s.o(.data);
        build/JP/overlays/room_m116_nypd/asm/JP/overlays/room_m116_nypd/data/D_801D01F8.data.s.o(.data);
        . = ALIGN(., 4);
        room_m116_nypd_DATA_END = .;
        room_m116_nypd_DATA_SIZE = ABSOLUTE(room_m116_nypd_DATA_END - room_m116_nypd_DATA_START);
        room_m116_nypd_BSS_START = .;
        build/JP/overlays/room_m116_nypd/asm/JP/overlays/room_m116_nypd/data/room_m116_nypd.rodata.s.o(.bss);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/line_burst.c.o(.bss);
        build/JP/overlays/room_m116_nypd/src/jp/overlays/room_lib/script_slot.c.o(.bss);
        . = ALIGN(., 4);
        room_m116_nypd_BSS_END = .;
        room_m116_nypd_BSS_SIZE = ABSOLUTE(room_m116_nypd_BSS_END - room_m116_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m116_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m116_nypd_ROM_END = __romPos;
    room_m116_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
