SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m034_sewer_ROM_START = __romPos;
    room_m034_sewer_VRAM = ADDR(.room_m034_sewer);
    .room_m034_sewer (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m034_sewer_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m034_sewer_RODATA_START = .;
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/room_m034_sewer.rodata.s.o(.rodata);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/glow_orb.c.o(.rodata);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/glow_orb_setup.c.o(.rodata);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_spawn.c.o(.rodata);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_register_active.c.o(.rodata);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_close_target.c.o(.rodata);
        . = ALIGN(., 4);
        room_m034_sewer_RODATA_END = .;
        room_m034_sewer_RODATA_SIZE = ABSOLUTE(room_m034_sewer_RODATA_END - room_m034_sewer_RODATA_START);
        room_m034_sewer_TEXT_START = .;
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/glow_orb_setup.c.o(.text);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_spawn.c.o(.text);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_register_active.c.o(.text);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_close_target.c.o(.text);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/glow_orb.c.o(.text);
        . = ALIGN(., 4);
        room_m034_sewer_TEXT_END = .;
        room_m034_sewer_TEXT_SIZE = ABSOLUTE(room_m034_sewer_TEXT_END - room_m034_sewer_TEXT_START);
        room_m034_sewer_DATA_START = .;
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/room_m034_sewer.rodata.s.o(.data);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/glow_orb.c.o(.data);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/glow_orb_setup.c.o(.data);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_spawn.c.o(.data);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_register_active.c.o(.data);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_close_target.c.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_8018FF34.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/RoomLib_TableC.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_8018FF5C.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_8018FF70.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_8018FF7C.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_8018FF98.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_80190028.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_80190048.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_80190050.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_80190060.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_80190070.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/room_m034_sewer/D_8019008C.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/room_m034_sewer/D_801A81F8.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/room_m034_sewer/D_801B81E4.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/D_801C0D28.data.s.o(.data);
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/room_m034_sewer/D_801C0D34.s.o(.data);
        . = ALIGN(., 4);
        room_m034_sewer_DATA_END = .;
        room_m034_sewer_DATA_SIZE = ABSOLUTE(room_m034_sewer_DATA_END - room_m034_sewer_DATA_START);
        room_m034_sewer_BSS_START = .;
        build/JP/overlays/room_m034_sewer/asm/JP/overlays/room_m034_sewer/data/room_m034_sewer.rodata.s.o(.bss);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/glow_orb.c.o(.bss);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/glow_orb_setup.c.o(.bss);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_spawn.c.o(.bss);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_register_active.c.o(.bss);
        build/JP/overlays/room_m034_sewer/src/jp/overlays/room_lib/class_close_target.c.o(.bss);
        . = ALIGN(., 4);
        room_m034_sewer_BSS_END = .;
        room_m034_sewer_BSS_SIZE = ABSOLUTE(room_m034_sewer_BSS_END - room_m034_sewer_BSS_START);
    }
    __romPos += SIZEOF(.room_m034_sewer);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m034_sewer_ROM_END = __romPos;
    room_m034_sewer_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
