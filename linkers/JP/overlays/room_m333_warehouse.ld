SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m333_warehouse_ROM_START = __romPos;
    room_m333_warehouse_VRAM = ADDR(.room_m333_warehouse);
    .room_m333_warehouse (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m333_warehouse_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m333_warehouse_RODATA_START = .;
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/room_m333_warehouse.rodata.s.o(.rodata);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/orbit_flare.c.o(.rodata);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/line_burst.c.o(.rodata);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/script_args.c.o(.rodata);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/script_slot.c.o(.rodata);
        . = ALIGN(., 4);
        room_m333_warehouse_RODATA_END = .;
        room_m333_warehouse_RODATA_SIZE = ABSOLUTE(room_m333_warehouse_RODATA_END - room_m333_warehouse_RODATA_START);
        room_m333_warehouse_TEXT_START = .;
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/orbit_flare.c.o(.text);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/script_args.c.o(.text);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/line_burst.c.o(.text);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/script_slot.c.o(.text);
        . = ALIGN(., 4);
        room_m333_warehouse_TEXT_END = .;
        room_m333_warehouse_TEXT_SIZE = ABSOLUTE(room_m333_warehouse_TEXT_END - room_m333_warehouse_TEXT_START);
        room_m333_warehouse_DATA_START = .;
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/room_m333_warehouse.rodata.s.o(.data);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/orbit_flare.c.o(.data);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/line_burst.c.o(.data);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/script_slot.c.o(.data);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/D_8019405C.data.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/D_80194098.data.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/D_801940D0.data.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/D_801940F0.data.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/RoomSlotA_8019315C.data.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/RoomSlotB_8019315C.data.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/RoomSlotC_8019315C.data.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/room_m333_warehouse/RoomLib_SlotTable.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/D_801B716C.data.s.o(.data);
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/room_m333_warehouse/D_801B719C.s.o(.data);
        . = ALIGN(., 4);
        room_m333_warehouse_DATA_END = .;
        room_m333_warehouse_DATA_SIZE = ABSOLUTE(room_m333_warehouse_DATA_END - room_m333_warehouse_DATA_START);
        room_m333_warehouse_BSS_START = .;
        build/JP/overlays/room_m333_warehouse/asm/JP/overlays/room_m333_warehouse/data/room_m333_warehouse.rodata.s.o(.bss);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/orbit_flare.c.o(.bss);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/line_burst.c.o(.bss);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/script_args.c.o(.bss);
        build/JP/overlays/room_m333_warehouse/src-jp/overlays/room_lib/script_slot.c.o(.bss);
        . = ALIGN(., 4);
        room_m333_warehouse_BSS_END = .;
        room_m333_warehouse_BSS_SIZE = ABSOLUTE(room_m333_warehouse_BSS_END - room_m333_warehouse_BSS_START);
    }
    __romPos += SIZEOF(.room_m333_warehouse);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m333_warehouse_ROM_END = __romPos;
    room_m333_warehouse_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
