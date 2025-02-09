ENTRY(_start)

SECTIONS {
    . = 0x80000000; /* Load at 2GB (default for realview-pb-a8) */

    .text : {
        *(.text)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss COMMON)
    }
}

