SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m121_nypd_ROM_START = __romPos;
    room_m121_nypd_VRAM = ADDR(.room_m121_nypd);
    .room_m121_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m121_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m121_nypd_RODATA_START = .;
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/room_m121_nypd.rodata.s.o(.rodata);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m121_nypd_RODATA_END = .;
        room_m121_nypd_RODATA_SIZE = ABSOLUTE(room_m121_nypd_RODATA_END - room_m121_nypd_RODATA_START);
        room_m121_nypd_TEXT_START = .;
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        room_m121_nypd_TEXT_END = .;
        room_m121_nypd_TEXT_SIZE = ABSOLUTE(room_m121_nypd_TEXT_END - room_m121_nypd_TEXT_START);
        room_m121_nypd_DATA_START = .;
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/room_m121_nypd.rodata.s.o(.data);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/D_80193214.data.s.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/D_80193250.data.s.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/RoomSlotA_80193148.data.s.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/RoomSlotB_80193148.data.s.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/room_m121_nypd/RoomSlotC_80193148.s.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/room_m121_nypd/D_801A7198.s.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/D_801D32A0.data.s.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/D_801D32AC.data.s.o(.data);
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/room_m121_nypd/D_801D32B8.s.o(.data);
        . = ALIGN(., 4);
        room_m121_nypd_DATA_END = .;
        room_m121_nypd_DATA_SIZE = ABSOLUTE(room_m121_nypd_DATA_END - room_m121_nypd_DATA_START);
        room_m121_nypd_BSS_START = .;
        build/JP/overlays/room_m121_nypd/asm/JP/overlays/room_m121_nypd/data/room_m121_nypd.rodata.s.o(.bss);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m121_nypd/src/jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m121_nypd_BSS_END = .;
        room_m121_nypd_BSS_SIZE = ABSOLUTE(room_m121_nypd_BSS_END - room_m121_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m121_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m121_nypd_ROM_END = __romPos;
    room_m121_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
