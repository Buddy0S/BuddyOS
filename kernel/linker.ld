ENTRY(_start)

SECTIONS
{
    /* Start the code at address 0x8000.
       Kevin adjust this to where U-Boot is set to actually load the kernel.
    */
    . = 0x80000000;

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }

    .stack : {
    . = ALIGN(4);
    __stack_start = .;
    . += 0x1000; /* Define stack size */
    __stack_end = .;
    }
}

