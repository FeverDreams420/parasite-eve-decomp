SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m269_warehouse_ROM_START = __romPos;
    room_m269_warehouse_VRAM = ADDR(.room_m269_warehouse);
    .room_m269_warehouse (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m269_warehouse_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m269_warehouse_RODATA_START = .;
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/room_m269_warehouse_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m269_warehouse/src/jp/overlays/room_lib/grab_attack.c.o(.rodata);
        . = ALIGN(., 4);
        room_m269_warehouse_RODATA_END = .;
        room_m269_warehouse_RODATA_SIZE = ABSOLUTE(room_m269_warehouse_RODATA_END - room_m269_warehouse_RODATA_START);
        room_m269_warehouse_TEXT_START = .;
        build/JP/overlays/room_m269_warehouse/src/jp/overlays/room_lib/grab_attack.c.o(.text);
        . = ALIGN(., 4);
        room_m269_warehouse_TEXT_END = .;
        room_m269_warehouse_TEXT_SIZE = ABSOLUTE(room_m269_warehouse_TEXT_END - room_m269_warehouse_TEXT_START);
        room_m269_warehouse_DATA_START = .;
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/room_m269_warehouse_header.rodata.s.o(.data);
        build/JP/overlays/room_m269_warehouse/src/jp/overlays/room_lib/grab_attack.c.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_801921C8.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_801921E4.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_80192224.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_80192228.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_80192260.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_80192280.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_801922A8.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_801922E0.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_801922F8.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/RoomSlotA_8019218C.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_80192334.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_80192338.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/RoomLib_PairA.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/RoomLib_PairB.data.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/room_m269_warehouse/D_801981E8.s.o(.data);
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/D_801D01E8.data.s.o(.data);
        . = ALIGN(., 4);
        room_m269_warehouse_DATA_END = .;
        room_m269_warehouse_DATA_SIZE = ABSOLUTE(room_m269_warehouse_DATA_END - room_m269_warehouse_DATA_START);
        room_m269_warehouse_BSS_START = .;
        build/JP/overlays/room_m269_warehouse/asm/JP/overlays/room_m269_warehouse/data/room_m269_warehouse_header.rodata.s.o(.bss);
        build/JP/overlays/room_m269_warehouse/src/jp/overlays/room_lib/grab_attack.c.o(.bss);
        . = ALIGN(., 4);
        room_m269_warehouse_BSS_END = .;
        room_m269_warehouse_BSS_SIZE = ABSOLUTE(room_m269_warehouse_BSS_END - room_m269_warehouse_BSS_START);
    }
    __romPos += SIZEOF(.room_m269_warehouse);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m269_warehouse_ROM_END = __romPos;
    room_m269_warehouse_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
