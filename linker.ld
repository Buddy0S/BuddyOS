ENTRY(_start)

SECTIONS
{
    /* Start the code at address 0x8000.
       Kevin adjust this to where U-Boot is set to actually load the kernel.
    */
    . = 0x8000;

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
}

