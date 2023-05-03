/* Entry point of the kernel. */

ENTRY(kernel_init)

/* Output foramt. */

OUTPUT_FORMAT(binary)

/* Sections. */

SECTIONS {
    /* Load the kernel after 1MB of memory, with has address of 0x100000. */

    . = 1M;

    /** Sections are 4KB aligned for memory paging. */

    /* Put the bootstrap assembly code at the top. */

    .bootstrap BLOCK(4K) : ALIGN(4K) {
        *(.bootstrap)
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

    /**
     * Put the assembly codes at the end of the binary file, so that the asm files don't interfare
     * between the C object code.
     */

    .asm : ALIGN(4K) {
        *(.asm)
    }
}