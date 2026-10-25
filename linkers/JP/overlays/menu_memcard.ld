SECTIONS
{
    HIDDEN(__romPos = 0);
    menu_memcard_ROM_START = __romPos;
    menu_memcard_VRAM = ADDR(.menu_memcard);
    .menu_memcard (OVERLAY_WINDOW_MENU_MEMCARD + 0x0) : AT(menu_memcard_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        menu_memcard_RODATA_START = .;
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/video.c.o(.rodata);
        . = ALIGN(., 4);
        menu_memcard_RODATA_END = .;
        menu_memcard_RODATA_SIZE = ABSOLUTE(menu_memcard_RODATA_END - menu_memcard_RODATA_START);
        menu_memcard_TEXT_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/data/menu_memcard_header.data.s.o(.code_data);
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/video.c.o(.text);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/data/menu_memcard_data_0016F4.data.s.o(.code_data);
        . = ALIGN(., 4);
        menu_memcard_TEXT_END = .;
        menu_memcard_TEXT_SIZE = ABSOLUTE(menu_memcard_TEXT_END - menu_memcard_TEXT_START);
        menu_memcard_DATA_START = .;
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/video.c.o(.data);
        . = ALIGN(., 4);
        menu_memcard_DATA_END = .;
        menu_memcard_DATA_SIZE = ABSOLUTE(menu_memcard_DATA_END - menu_memcard_DATA_START);
        menu_memcard_BSS_START = .;
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/video.c.o(.bss);
        . = ALIGN(., 4);
        menu_memcard_BSS_END = .;
        menu_memcard_BSS_SIZE = ABSOLUTE(menu_memcard_BSS_END - menu_memcard_BSS_START);
    }
    __romPos += SIZEOF(.menu_memcard);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    menu_memcard_ROM_END = __romPos;
    menu_memcard_VRAM_END = .;

    menu_memcard_effects_ROM_START = __romPos;
    menu_memcard_effects_VRAM = ADDR(.menu_memcard_effects);
    .menu_memcard_effects (OVERLAY_WINDOW_TASK_PROGRAM + 0x60) : AT(menu_memcard_effects_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        menu_memcard_effects_RODATA_START = .;
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/glint_effects.c.o(.rodata);
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/image_effects.c.o(.rodata);
        . = ALIGN(., 4);
        menu_memcard_effects_RODATA_END = .;
        menu_memcard_effects_RODATA_SIZE = ABSOLUTE(menu_memcard_effects_RODATA_END - menu_memcard_effects_RODATA_START);
        menu_memcard_effects_TEXT_START = .;
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/glint_effects.c.o(.text);
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/image_effects.c.o(.text);
        . = ALIGN(., 4);
        menu_memcard_effects_TEXT_END = .;
        menu_memcard_effects_TEXT_SIZE = ABSOLUTE(menu_memcard_effects_TEXT_END - menu_memcard_effects_TEXT_START);
        menu_memcard_effects_DATA_START = .;
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/glint_effects.c.o(.data);
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/image_effects.c.o(.data);
        . = ALIGN(., 4);
        menu_memcard_effects_DATA_END = .;
        menu_memcard_effects_DATA_SIZE = ABSOLUTE(menu_memcard_effects_DATA_END - menu_memcard_effects_DATA_START);
        menu_memcard_effects_BSS_START = .;
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/glint_effects.c.o(.bss);
        build/JP/overlays/menu_memcard/src/jp/overlays/menu_memcard/image_effects.c.o(.bss);
        . = ALIGN(., 4);
        menu_memcard_effects_BSS_END = .;
        menu_memcard_effects_BSS_SIZE = ABSOLUTE(menu_memcard_effects_BSS_END - menu_memcard_effects_BSS_START);
    }
    __romPos += SIZEOF(.menu_memcard_effects);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    menu_memcard_effects_ROM_END = __romPos;
    menu_memcard_effects_VRAM_END = .;

    menu_memcard_screen_ROM_START = __romPos;
    menu_memcard_screen_VRAM = ADDR(.menu_memcard_screen);
    .menu_memcard_screen (OVERLAY_WINDOW_FX_SECTOR17 + 0x1EB90) : AT(menu_memcard_screen_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        menu_memcard_screen_RODATA_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/page_flip.s.o(.rodata);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/image_nodes.s.o(.rodata);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/list_screen.s.o(.rodata);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/video_playback.s.o(.rodata);
        . = ALIGN(., 4);
        menu_memcard_screen_RODATA_END = .;
        menu_memcard_screen_RODATA_SIZE = ABSOLUTE(menu_memcard_screen_RODATA_END - menu_memcard_screen_RODATA_START);
        menu_memcard_screen_TEXT_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/data/menu_memcard_data_0063A0.data.s.o(.code_data);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/page_flip.s.o(.text);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/image_nodes.s.o(.text);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/list_screen.s.o(.text);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/video_playback.s.o(.text);
        . = ALIGN(., 4);
        menu_memcard_screen_TEXT_END = .;
        menu_memcard_screen_TEXT_SIZE = ABSOLUTE(menu_memcard_screen_TEXT_END - menu_memcard_screen_TEXT_START);
        menu_memcard_screen_DATA_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/page_flip.s.o(.data);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/image_nodes.s.o(.data);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/list_screen.s.o(.data);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/video_playback.s.o(.data);
        . = ALIGN(., 4);
        menu_memcard_screen_DATA_END = .;
        menu_memcard_screen_DATA_SIZE = ABSOLUTE(menu_memcard_screen_DATA_END - menu_memcard_screen_DATA_START);
        menu_memcard_screen_BSS_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/page_flip.s.o(.bss);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/image_nodes.s.o(.bss);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/list_screen.s.o(.bss);
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/video_playback.s.o(.bss);
        . = ALIGN(., 4);
        menu_memcard_screen_BSS_END = .;
        menu_memcard_screen_BSS_SIZE = ABSOLUTE(menu_memcard_screen_BSS_END - menu_memcard_screen_BSS_START);
    }
    __romPos += SIZEOF(.menu_memcard_screen);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    menu_memcard_screen_ROM_END = __romPos;
    menu_memcard_screen_VRAM_END = .;

    menu_memcard_fade_ROM_START = __romPos;
    menu_memcard_fade_VRAM = ADDR(.menu_memcard_fade);
    .menu_memcard_fade (OVERLAY_WINDOW_ROOM + 0x3F64) : AT(menu_memcard_fade_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        menu_memcard_fade_RODATA_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/fade.s.o(.rodata);
        . = ALIGN(., 4);
        menu_memcard_fade_RODATA_END = .;
        menu_memcard_fade_RODATA_SIZE = ABSOLUTE(menu_memcard_fade_RODATA_END - menu_memcard_fade_RODATA_START);
        menu_memcard_fade_TEXT_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/fade.s.o(.text);
        . = ALIGN(., 4);
        menu_memcard_fade_TEXT_END = .;
        menu_memcard_fade_TEXT_SIZE = ABSOLUTE(menu_memcard_fade_TEXT_END - menu_memcard_fade_TEXT_START);
        menu_memcard_fade_DATA_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/fade.s.o(.data);
        . = ALIGN(., 4);
        menu_memcard_fade_DATA_END = .;
        menu_memcard_fade_DATA_SIZE = ABSOLUTE(menu_memcard_fade_DATA_END - menu_memcard_fade_DATA_START);
        menu_memcard_fade_BSS_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/fade.s.o(.bss);
        . = ALIGN(., 4);
        menu_memcard_fade_BSS_END = .;
        menu_memcard_fade_BSS_SIZE = ABSOLUTE(menu_memcard_fade_BSS_END - menu_memcard_fade_BSS_START);
    }
    __romPos += SIZEOF(.menu_memcard_fade);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    menu_memcard_fade_ROM_END = __romPos;
    menu_memcard_fade_VRAM_END = .;

    menu_memcard_tail_ROM_START = __romPos;
    menu_memcard_tail_VRAM = ADDR(.menu_memcard_tail);
    .menu_memcard_tail (OVERLAY_WINDOW_BOOT_DISPLAY + 0x8804) : AT(menu_memcard_tail_ROM_START) SUBALIGN(4)
    {
        FILL(0x00000000);
        menu_memcard_tail_TEXT_START = .;
        build/JP/overlays/menu_memcard/asm/JP/overlays/menu_memcard/data/menu_memcard_data.data.s.o(.code_data);
        . = ALIGN(., 4);
        menu_memcard_tail_TEXT_END = .;
        menu_memcard_tail_TEXT_SIZE = ABSOLUTE(menu_memcard_tail_TEXT_END - menu_memcard_tail_TEXT_START);
    }
    __romPos += SIZEOF(.menu_memcard_tail);
    __romPos = ALIGN(__romPos, 4);
    . = ALIGN(., 4);
    menu_memcard_tail_ROM_END = __romPos;
    menu_memcard_tail_VRAM_END = .;

    /DISCARD/ :
    {
        *(*);
    }
}
