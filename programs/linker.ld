/* Entry point of the program. */

ENTRY(_start)

/* Output foramt. */

OUTPUT_FORMAT(elf32-i386)

/* Sections. */

SECTIONS {
    /* Load the program after at virtual memory address 0x400000. */

    . = 0x400000;

    /** 
     * Set all the bootstrap assembly code at the beginning.
     */

    .bootstrap_asm : ALIGN(4K) {
        *(.bootstrap_asm)
    }

    /* Regular assembly section. */

    .asm : ALIGN(4K) {
        *(.asm)
    }

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
}