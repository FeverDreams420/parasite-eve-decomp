SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m263_warehouse_ROM_START = __romPos;
    room_m263_warehouse_VRAM = ADDR(.room_m263_warehouse);
    .room_m263_warehouse (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m263_warehouse_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m263_warehouse_RODATA_START = .;
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/room_m263_warehouse.rodata.s.o(.rodata);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/line_burst.c.o(.rodata);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/script_args.c.o(.rodata);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/script_slot.c.o(.rodata);
        . = ALIGN(., 4);
        room_m263_warehouse_RODATA_END = .;
        room_m263_warehouse_RODATA_SIZE = ABSOLUTE(room_m263_warehouse_RODATA_END - room_m263_warehouse_RODATA_START);
        room_m263_warehouse_TEXT_START = .;
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/script_args.c.o(.text);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/line_burst.c.o(.text);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/script_slot.c.o(.text);
        . = ALIGN(., 4);
        room_m263_warehouse_TEXT_END = .;
        room_m263_warehouse_TEXT_SIZE = ABSOLUTE(room_m263_warehouse_TEXT_END - room_m263_warehouse_TEXT_START);
        room_m263_warehouse_DATA_START = .;
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/room_m263_warehouse.rodata.s.o(.data);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/line_burst.c.o(.data);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/D_8019405C.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/D_80194098.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/D_801940D0.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/D_801940F0.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/RoomSlotA_8019315C.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/RoomSlotB_8019315C.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/RoomSlotC_8019315C.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/room_m263_warehouse/RoomLib_SlotTable.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/room_m263_warehouse/D_801AB4E8.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/room_m263_warehouse/D_801C01E0.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/D_801CC9F0.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/D_801CCA08.data.s.o(.data);
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/room_m263_warehouse/D_801CCA20.s.o(.data);
        . = ALIGN(., 4);
        room_m263_warehouse_DATA_END = .;
        room_m263_warehouse_DATA_SIZE = ABSOLUTE(room_m263_warehouse_DATA_END - room_m263_warehouse_DATA_START);
        room_m263_warehouse_BSS_START = .;
        build/JP/overlays/room_m263_warehouse/asm/JP/overlays/room_m263_warehouse/data/room_m263_warehouse.rodata.s.o(.bss);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/line_burst.c.o(.bss);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/script_args.c.o(.bss);
        build/JP/overlays/room_m263_warehouse/src-jp/overlays/room_lib/script_slot.c.o(.bss);
        . = ALIGN(., 4);
        room_m263_warehouse_BSS_END = .;
        room_m263_warehouse_BSS_SIZE = ABSOLUTE(room_m263_warehouse_BSS_END - room_m263_warehouse_BSS_START);
    }
    __romPos += SIZEOF(.room_m263_warehouse);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m263_warehouse_ROM_END = __romPos;
    room_m263_warehouse_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
