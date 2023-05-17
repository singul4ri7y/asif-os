#include <nuttle/kernel.h>
#include <nuttle/idt.h>
#include <nuttle/io.h>
#include <nuttle/pic.h>
#include <nuttle/kheap.h>
#include <nuttle/paging.h>
#include <nuttle/disk.h>
#include <kernio.h>
#include <kernmem.h>

static PagingChunk* kernel_paging_4gb_chunk = 0;

void kernel_main() {
    // Intialize the Teletype Output screen.

    tty_init();

    // Initialize the heap.

    kheap_init();

    // Initialize the Interrupt Descriptor Table.

    idt_init();

    // Creating 4GB paging chunk.

    kernel_paging_4gb_chunk = paging_get_new_4gb_chunk(PAGING_IS_WRITTABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Set up the paging directory.

    paging_switch(paging_get_directory(kernel_paging_4gb_chunk));

    // Now enable paging.

    enable_paging();

    // Initialize the Intel 8259A PIC.

    // Intel recommends to reserve the first 32 (0x1f counting from 0) interrupt in IDT for 
    // exception handling.
    // So our new hardware ISR should start from 0x20.

    initialize_pic(0x20, 0x28);

    // Now enable all the interrupts.

    enable_interrupts();

    char buffer[512];

    disk_read_sectors(0, 1, buffer);

    putsk("AsifOS rocks!\n");
}