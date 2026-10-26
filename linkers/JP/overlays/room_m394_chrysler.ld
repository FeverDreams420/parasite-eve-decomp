SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m394_chrysler_ROM_START = __romPos;
    room_m394_chrysler_VRAM = ADDR(.room_m394_chrysler);
    .room_m394_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m394_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m394_chrysler_RODATA_START = .;
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/room_m394_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/three_particles.c.o(.rodata);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/line_burst.c.o(.rodata);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/script_slot.c.o(.rodata);
        . = ALIGN(., 4);
        room_m394_chrysler_RODATA_END = .;
        room_m394_chrysler_RODATA_SIZE = ABSOLUTE(room_m394_chrysler_RODATA_END - room_m394_chrysler_RODATA_START);
        room_m394_chrysler_TEXT_START = .;
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/three_particles.c.o(.text);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/line_burst.c.o(.text);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/script_slot.c.o(.text);
        . = ALIGN(., 4);
        room_m394_chrysler_TEXT_END = .;
        room_m394_chrysler_TEXT_SIZE = ABSOLUTE(room_m394_chrysler_TEXT_END - room_m394_chrysler_TEXT_START);
        room_m394_chrysler_DATA_START = .;
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/room_m394_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/three_particles.c.o(.data);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/line_burst.c.o(.data);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_80194334.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/RoomLib_TableA_8018F240.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_8019434C.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_80194358.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_80194360.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_8019437C.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_801943B4.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_8019446C.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_8019448C.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/RoomLib_SlotTable.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_80194548.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_8019454C.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_80194550.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_80194558.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_8019455C.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/room_m394_chrysler/D_80194560.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/D_801BDD38.data.s.o(.data);
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/room_m394_chrysler/D_801BDD5C.s.o(.data);
        . = ALIGN(., 4);
        room_m394_chrysler_DATA_END = .;
        room_m394_chrysler_DATA_SIZE = ABSOLUTE(room_m394_chrysler_DATA_END - room_m394_chrysler_DATA_START);
        room_m394_chrysler_BSS_START = .;
        build/JP/overlays/room_m394_chrysler/asm/JP/overlays/room_m394_chrysler/data/room_m394_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/three_particles.c.o(.bss);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/line_burst.c.o(.bss);
        build/JP/overlays/room_m394_chrysler/src-jp/overlays/room_lib/script_slot.c.o(.bss);
        . = ALIGN(., 4);
        room_m394_chrysler_BSS_END = .;
        room_m394_chrysler_BSS_SIZE = ABSOLUTE(room_m394_chrysler_BSS_END - room_m394_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m394_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m394_chrysler_ROM_END = __romPos;
    room_m394_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
