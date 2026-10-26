SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m100_nypd_ROM_START = __romPos;
    room_m100_nypd_VRAM = ADDR(.room_m100_nypd);
    .room_m100_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m100_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m100_nypd_RODATA_START = .;
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/room_m100_nypd.rodata.s.o(.rodata);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m100_nypd_RODATA_END = .;
        room_m100_nypd_RODATA_SIZE = ABSOLUTE(room_m100_nypd_RODATA_END - room_m100_nypd_RODATA_START);
        room_m100_nypd_TEXT_START = .;
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        room_m100_nypd_TEXT_END = .;
        room_m100_nypd_TEXT_SIZE = ABSOLUTE(room_m100_nypd_TEXT_END - room_m100_nypd_TEXT_START);
        room_m100_nypd_DATA_START = .;
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/room_m100_nypd.rodata.s.o(.data);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/D_80193214.data.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/D_80193250.data.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/RoomSlotA_80193148.data.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/RoomSlotB_80193148.data.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/room_m100_nypd/RoomSlotC_80193148.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/room_m100_nypd/D_801A81E8.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/D_801B81EC.data.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/room_m100_nypd/D_801C01E8.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/D_801D1150.data.s.o(.data);
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/room_m100_nypd/D_801D1168.s.o(.data);
        . = ALIGN(., 4);
        room_m100_nypd_DATA_END = .;
        room_m100_nypd_DATA_SIZE = ABSOLUTE(room_m100_nypd_DATA_END - room_m100_nypd_DATA_START);
        room_m100_nypd_BSS_START = .;
        build/JP/overlays/room_m100_nypd/asm/JP/overlays/room_m100_nypd/data/room_m100_nypd.rodata.s.o(.bss);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m100_nypd/src-jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m100_nypd_BSS_END = .;
        room_m100_nypd_BSS_SIZE = ABSOLUTE(room_m100_nypd_BSS_END - room_m100_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m100_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m100_nypd_ROM_END = __romPos;
    room_m100_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
