SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m005_theater_ROM_START = __romPos;
    room_m005_theater_VRAM = ADDR(.room_m005_theater);
    .room_m005_theater (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m005_theater_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m005_theater_RODATA_START = .;
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/room_m005_theater.rodata.s.o(.rodata);
        build/JP/overlays/room_m005_theater/src/jp/overlays/room_m005_theater/m005.c.o(.rodata);
        build/JP/overlays/room_m005_theater/src/jp/overlays/room_lib/script_task_var.c.o(.rodata);
        . = ALIGN(., 4);
        room_m005_theater_RODATA_END = .;
        room_m005_theater_RODATA_SIZE = ABSOLUTE(room_m005_theater_RODATA_END - room_m005_theater_RODATA_START);
        room_m005_theater_TEXT_START = .;
        build/JP/overlays/room_m005_theater/src/jp/overlays/room_m005_theater/m005.c.o(.text);
        build/JP/overlays/room_m005_theater/src/jp/overlays/room_lib/script_task_var.c.o(.text);
        . = ALIGN(., 4);
        room_m005_theater_TEXT_END = .;
        room_m005_theater_TEXT_SIZE = ABSOLUTE(room_m005_theater_TEXT_END - room_m005_theater_TEXT_START);
        room_m005_theater_DATA_START = .;
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/room_m005_theater.rodata.s.o(.data);
        build/JP/overlays/room_m005_theater/src/jp/overlays/room_m005_theater/m005.c.o(.data);
        build/JP/overlays/room_m005_theater/src/jp/overlays/room_lib/script_task_var.c.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190A98.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/RoomM005_Rec2.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190AF4.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190B44.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/g_RoomScriptRecord.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190B84.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190B88.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190B8C.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190B94.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190B98.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190BA0.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190BA4.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190BA8.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_80190BAC.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/room_m005_theater/D_801981E0.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/D_801BEB0C.data.s.o(.data);
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/room_m005_theater/D_801BEB18.s.o(.data);
        . = ALIGN(., 4);
        room_m005_theater_DATA_END = .;
        room_m005_theater_DATA_SIZE = ABSOLUTE(room_m005_theater_DATA_END - room_m005_theater_DATA_START);
        room_m005_theater_BSS_START = .;
        build/JP/overlays/room_m005_theater/asm/JP/overlays/room_m005_theater/data/room_m005_theater.rodata.s.o(.bss);
        build/JP/overlays/room_m005_theater/src/jp/overlays/room_m005_theater/m005.c.o(.bss);
        build/JP/overlays/room_m005_theater/src/jp/overlays/room_lib/script_task_var.c.o(.bss);
        . = ALIGN(., 4);
        room_m005_theater_BSS_END = .;
        room_m005_theater_BSS_SIZE = ABSOLUTE(room_m005_theater_BSS_END - room_m005_theater_BSS_START);
    }
    __romPos += SIZEOF(.room_m005_theater);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m005_theater_ROM_END = __romPos;
    room_m005_theater_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
