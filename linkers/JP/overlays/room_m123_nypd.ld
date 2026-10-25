SECTIONS
{
    HIDDEN(__romPos = 0);
    room_m123_nypd_ROM_START = __romPos;
    room_m123_nypd_VRAM = ADDR(.room_m123_nypd);
    .room_m123_nypd (OVERLAY_WINDOW_ROOM + 0x0) : AT(room_m123_nypd_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        room_m123_nypd_RODATA_START = .;
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/room_m123_nypd.rodata.s.o(.rodata);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.rodata);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/RoomLib_HandlerCArgsData_801915D0.rodata.s.o(.rodata);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_m123_nypd/m123.c.o(.rodata);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/script_task_var.c.o(.rodata);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_m123_nypd/RoomM123_SelectSlotPointer.c.o(.rodata);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/script_arg.c.o(.rodata);
        . = ALIGN(., 4);
        room_m123_nypd_RODATA_END = .;
        room_m123_nypd_RODATA_SIZE = ABSOLUTE(room_m123_nypd_RODATA_END - room_m123_nypd_RODATA_START);
        room_m123_nypd_TEXT_START = .;
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.text);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_m123_nypd/m123.c.o(.text);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/script_task_var.c.o(.text);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_m123_nypd/RoomM123_SelectSlotPointer.c.o(.text);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/script_arg.c.o(.text);
        . = ALIGN(., 4);
        room_m123_nypd_TEXT_END = .;
        room_m123_nypd_TEXT_SIZE = ABSOLUTE(room_m123_nypd_TEXT_END - room_m123_nypd_TEXT_START);
        room_m123_nypd_DATA_START = .;
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/room_m123_nypd.rodata.s.o(.data);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/RoomLib_HandlerCArgsData_801915D0.rodata.s.o(.data);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_m123_nypd/m123.c.o(.data);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/script_task_var.c.o(.data);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_m123_nypd/RoomM123_SelectSlotPointer.c.o(.data);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/script_arg.c.o(.data);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/actor_class_table.c.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_8019548C.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801954BC.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801954E4.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_8019550C.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_8019553C.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_80195574.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_80195594.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801955B4.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801955D4.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_8019560C.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_80195624.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_8019564C.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/g_RoomScriptRecord.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_80195688.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_80195690.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_80195694.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_80195698.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_8019569C.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801956A0.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801956A4.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801956A8.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801956B0.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801956B4.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801956B8.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/room_m123_nypd/RoomSlotA_801953D4.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/room_m123_nypd/D_801A81E0.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801B81E8.data.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/room_m123_nypd/D_801C01E0.s.o(.data);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/D_801D01E0.data.s.o(.data);
        . = ALIGN(., 4);
        room_m123_nypd_DATA_END = .;
        room_m123_nypd_DATA_SIZE = ABSOLUTE(room_m123_nypd_DATA_END - room_m123_nypd_DATA_START);
        room_m123_nypd_BSS_START = .;
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/room_m123_nypd.rodata.s.o(.bss);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/actor_classes.c.o(.bss);
        build/JP/overlays/room_m123_nypd/asm/JP/overlays/room_m123_nypd/data/RoomLib_HandlerCArgsData_801915D0.rodata.s.o(.bss);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_m123_nypd/m123.c.o(.bss);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/script_task_var.c.o(.bss);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_m123_nypd/RoomM123_SelectSlotPointer.c.o(.bss);
        build/JP/overlays/room_m123_nypd/src/jp/overlays/room_lib/script_arg.c.o(.bss);
        . = ALIGN(., 4);
        room_m123_nypd_BSS_END = .;
        room_m123_nypd_BSS_SIZE = ABSOLUTE(room_m123_nypd_BSS_END - room_m123_nypd_BSS_START);
    }
    __romPos += SIZEOF(.room_m123_nypd);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    room_m123_nypd_ROM_END = __romPos;
    room_m123_nypd_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
