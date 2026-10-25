SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m023_theater_ROM_START = __romPos;
    room_m023_theater_VRAM = ADDR(.room_m023_theater);
    .room_m023_theater (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m023_theater_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m023_theater_RODATA_START = .;
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/room_m023_header.rodata.s.o(.rodata);
        build/JP/overlays/room_m023_theater/src/jp/overlays/room_m023_theater/m023.c.o(.rodata);
        build/JP/overlays/room_m023_theater/src/jp/overlays/room_lib/script_task_var.c.o(.rodata);
        . = ALIGN(., 4);
        room_m023_theater_RODATA_END = .;
        room_m023_theater_RODATA_SIZE = ABSOLUTE(room_m023_theater_RODATA_END - room_m023_theater_RODATA_START);
        room_m023_theater_TEXT_START = .;
        build/JP/overlays/room_m023_theater/src/jp/overlays/room_m023_theater/m023.c.o(.text);
        build/JP/overlays/room_m023_theater/src/jp/overlays/room_lib/script_task_var.c.o(.text);
        . = ALIGN(., 4);
        room_m023_theater_TEXT_END = .;
        room_m023_theater_TEXT_SIZE = ABSOLUTE(room_m023_theater_TEXT_END - room_m023_theater_TEXT_START);
        room_m023_theater_DATA_START = .;
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/room_m023_header.rodata.s.o(.data);
        build/JP/overlays/room_m023_theater/src/jp/overlays/room_m023_theater/m023.c.o(.data);
        build/JP/overlays/room_m023_theater/src/jp/overlays/room_lib/script_task_var.c.o(.data);
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/D_80190670.data.s.o(.data);
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/D_801906F0.data.s.o(.data);
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/D_80190720.data.s.o(.data);
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/g_RoomScriptRecord.data.s.o(.data);
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/D_80190760.data.s.o(.data);
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/room_m023_theater/D_801981F8.s.o(.data);
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/room_m023_theater/D_801D01F0.s.o(.data);
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/D_801E01EC.data.s.o(.data);
        . = ALIGN(., 4);
        room_m023_theater_DATA_END = .;
        room_m023_theater_DATA_SIZE = ABSOLUTE(room_m023_theater_DATA_END - room_m023_theater_DATA_START);
        room_m023_theater_BSS_START = .;
        build/JP/overlays/room_m023_theater/asm/JP/overlays/room_m023_theater/data/room_m023_header.rodata.s.o(.bss);
        build/JP/overlays/room_m023_theater/src/jp/overlays/room_m023_theater/m023.c.o(.bss);
        build/JP/overlays/room_m023_theater/src/jp/overlays/room_lib/script_task_var.c.o(.bss);
        . = ALIGN(., 4);
        room_m023_theater_BSS_END = .;
        room_m023_theater_BSS_SIZE = ABSOLUTE(room_m023_theater_BSS_END - room_m023_theater_BSS_START);
    }
    __romPos += SIZEOF(.room_m023_theater);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m023_theater_ROM_END = __romPos;
    room_m023_theater_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
