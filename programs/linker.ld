/* Entry point of the program. */

ENTRY(_start)

/* Output foramt. */

OUTPUT_FORMAT(binary)

/* Sections. */

SECTIONS {
    /* Load the program after at virtual memory address 0x400000. */

    . = 0x400000;

    /* Text section. */

    .text : ALIGN(4K) {
        *(.text)
    }

    /* Read only data section. */

    .rodata : ALIGN(4K) {
        *(.rodata)
    }

    /* Data section (initialized). */

    .data : ALIGN(4K) {
        *(.data)
    }

    /* Data Section (uninitialized). */

    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
    }

    /**
     * Put the assembly codes at the end of the binary file, so that the asm files don't interfare
     * between the C object code.
     */

    .asm : ALIGN(4K) {
        *(.asm)
    }
}