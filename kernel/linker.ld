ENTRY(_start)

SECTIONS
{
    /* Start execution at 0x80000000 */
    . = 0x80000000;

    /* Ensure the text segment (code) is aligned to 4KB */
    .text ALIGN(4K) : {
        *(.text*)
    }

    /* Read-only data segment */
    .rodata ALIGN(4K) : {
        *(.rodata*)
    }

    /* Initialized data segment */
    .data ALIGN(4K) : {
        *(.data*)
    }

    /* Uninitialized data segment */
    .bss ALIGN(4K) : {
        _bss_start = .;
        *(.bss*)
        _bss_end = .;
    }

    /* Stack Section */
    .stack ALIGN(4) : {
        _stack_bottom = .;
        . += 0x1000; /* Allocate 4KB stack */
        _stack_top = .;
    }
}

