SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m398_chrysler_ROM_START = __romPos;
    room_m398_chrysler_VRAM = ADDR(.room_m398_chrysler);
    .room_m398_chrysler (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m398_chrysler_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m398_chrysler_RODATA_START = .;
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/room_m398_chrysler.rodata.s.o(.rodata);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/homing_flash.c.o(.rodata);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/script_args.c.o(.rodata);
        . = ALIGN(., 4);
        room_m398_chrysler_RODATA_END = .;
        room_m398_chrysler_RODATA_SIZE = ABSOLUTE(room_m398_chrysler_RODATA_END - room_m398_chrysler_RODATA_START);
        room_m398_chrysler_TEXT_START = .;
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/homing_flash.c.o(.text);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/script_args.c.o(.text);
        . = ALIGN(., 4);
        room_m398_chrysler_TEXT_END = .;
        room_m398_chrysler_TEXT_SIZE = ABSOLUTE(room_m398_chrysler_TEXT_END - room_m398_chrysler_TEXT_START);
        room_m398_chrysler_DATA_START = .;
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/room_m398_chrysler.rodata.s.o(.data);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/homing_flash.c.o(.data);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/script_args.c.o(.data);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/D_801931E4.data.s.o(.data);
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/D_80193234.data.s.o(.data);
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/D_80193254.data.s.o(.data);
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/RoomSlotA_80193118.data.s.o(.data);
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/RoomSlotB_80193118.data.s.o(.data);
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/room_m398_chrysler/RoomSlotC_80193118.s.o(.data);
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/D_801BF230.data.s.o(.data);
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/room_m398_chrysler/D_801BF254.s.o(.data);
        . = ALIGN(., 4);
        room_m398_chrysler_DATA_END = .;
        room_m398_chrysler_DATA_SIZE = ABSOLUTE(room_m398_chrysler_DATA_END - room_m398_chrysler_DATA_START);
        room_m398_chrysler_BSS_START = .;
        build/JP/overlays/room_m398_chrysler/asm/JP/overlays/room_m398_chrysler/data/room_m398_chrysler.rodata.s.o(.bss);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/homing_flash.c.o(.bss);
        build/JP/overlays/room_m398_chrysler/src/jp/overlays/room_lib/script_args.c.o(.bss);
        . = ALIGN(., 4);
        room_m398_chrysler_BSS_END = .;
        room_m398_chrysler_BSS_SIZE = ABSOLUTE(room_m398_chrysler_BSS_END - room_m398_chrysler_BSS_START);
    }
    __romPos += SIZEOF(.room_m398_chrysler);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m398_chrysler_ROM_END = __romPos;
    room_m398_chrysler_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
