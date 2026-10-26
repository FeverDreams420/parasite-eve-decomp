SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m087_central_park_ROM_START = __romPos;
    room_m087_central_park_VRAM = ADDR(.room_m087_central_park);
    .room_m087_central_park (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m087_central_park_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m087_central_park_RODATA_START = .;
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/room_m087_central_park.rodata.s.o(.rodata);
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/three_particles.c.o(.rodata);
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/RoomLib_HandlerCArgsData_801925A0.rodata.s.o(.rodata);
        . = ALIGN(., 4);
        room_m087_central_park_RODATA_END = .;
        room_m087_central_park_RODATA_SIZE = ABSOLUTE(room_m087_central_park_RODATA_END - room_m087_central_park_RODATA_START);
        room_m087_central_park_TEXT_START = .;
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/three_particles.c.o(.text);
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/actor_classes.c.o(.text);
        . = ALIGN(., 4);
        room_m087_central_park_TEXT_END = .;
        room_m087_central_park_TEXT_SIZE = ABSOLUTE(room_m087_central_park_TEXT_END - room_m087_central_park_TEXT_START);
        room_m087_central_park_DATA_START = .;
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/room_m087_central_park.rodata.s.o(.data);
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/three_particles.c.o(.data);
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/RoomLib_HandlerCArgsData_801925A0.rodata.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_801934EC.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/RoomLib_TableA_8018F22C.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_80193504.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_80193510.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_80193518.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_80193534.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_8019356C.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_80193624.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_80193628.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_8019362C.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_80193630.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_80193638.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_8019363C.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/room_m087_central_park/D_80193640.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/D_801BA0C4.data.s.o(.data);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/room_m087_central_park/D_801BA0DC.s.o(.data);
        . = ALIGN(., 4);
        room_m087_central_park_DATA_END = .;
        room_m087_central_park_DATA_SIZE = ABSOLUTE(room_m087_central_park_DATA_END - room_m087_central_park_DATA_START);
        room_m087_central_park_BSS_START = .;
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/room_m087_central_park.rodata.s.o(.bss);
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/three_particles.c.o(.bss);
        build/JP/overlays/room_m087_central_park/src-jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m087_central_park/asm/JP/overlays/room_m087_central_park/data/RoomLib_HandlerCArgsData_801925A0.rodata.s.o(.bss);
        . = ALIGN(., 4);
        room_m087_central_park_BSS_END = .;
        room_m087_central_park_BSS_SIZE = ABSOLUTE(room_m087_central_park_BSS_END - room_m087_central_park_BSS_START);
    }
    __romPos += SIZEOF(.room_m087_central_park);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m087_central_park_ROM_END = __romPos;
    room_m087_central_park_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
