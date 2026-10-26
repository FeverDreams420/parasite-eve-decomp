SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m028_sewer_ROM_START = __romPos;
    room_m028_sewer_VRAM = ADDR(.room_m028_sewer);
    .room_m028_sewer (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m028_sewer_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m028_sewer_RODATA_START = .;
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/room_m028_sewer.rodata.s.o(.rodata);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m028_sewer_RODATA_END = .;
        room_m028_sewer_RODATA_SIZE = ABSOLUTE(room_m028_sewer_RODATA_END - room_m028_sewer_RODATA_START);
        room_m028_sewer_TEXT_START = .;
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        room_m028_sewer_TEXT_END = .;
        room_m028_sewer_TEXT_SIZE = ABSOLUTE(room_m028_sewer_TEXT_END - room_m028_sewer_TEXT_START);
        room_m028_sewer_DATA_START = .;
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/room_m028_sewer.rodata.s.o(.data);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/D_80193214.data.s.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/D_80193250.data.s.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/RoomSlotA_80193148.data.s.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/RoomSlotB_80193148.data.s.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/room_m028_sewer/RoomSlotC_80193148.s.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/room_m028_sewer/D_801A3F38.s.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/D_801B11F4.data.s.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/D_801B1200.data.s.o(.data);
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/room_m028_sewer/D_801B120C.s.o(.data);
        . = ALIGN(., 4);
        room_m028_sewer_DATA_END = .;
        room_m028_sewer_DATA_SIZE = ABSOLUTE(room_m028_sewer_DATA_END - room_m028_sewer_DATA_START);
        room_m028_sewer_BSS_START = .;
        build/JP/overlays/room_m028_sewer/asm/JP/overlays/room_m028_sewer/data/room_m028_sewer.rodata.s.o(.bss);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m028_sewer/src-jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m028_sewer_BSS_END = .;
        room_m028_sewer_BSS_SIZE = ABSOLUTE(room_m028_sewer_BSS_END - room_m028_sewer_BSS_START);
    }
    __romPos += SIZEOF(.room_m028_sewer);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m028_sewer_ROM_END = __romPos;
    room_m028_sewer_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
