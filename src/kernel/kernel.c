#include <nuttle/kernel.h>
#include <nuttle/idt.h>
#include <nuttle/io.h>
#include <nuttle/pic.h>
#include <nuttle/kheap.h>
#include <nuttle/paging.h>
#include <nuttle/disk/stream.h>
#include <nuttle/gdt.h>
#include <nuttle/config.h>
#include <nuttle/task/tss.h>
#include <nuttle/task/process.h>
#include <kernio.h>
#include <kernmem.h>

// Our Task State Segment.

NuttleTSS tss;

// GDT entries.

GDTEntry gdt_entries[NUTTLE_MAX_GDT_ENTRIES];
GDTEntryStructured gdt_entries_structured[NUTTLE_MAX_GDT_ENTRIES] = {
    { .base = 0x00, .limit = 0x00, .access_and_flags = 0x00 },                    // Null segment.
    { .base = 0x00, .limit = 0xfffff, .access_and_flags = 0b110010011000 },       // Kernel Code Segment.
    { .base = 0x00, .limit = 0xfffff, .access_and_flags = 0b110010010010 },       // Kernel Data Segment.
    { .base = 0x00, .limit = 0xfffff, .access_and_flags = 0b110011111000 },       // User code segment.
    { .base = 0x00, .limit = 0xfffff, .access_and_flags = 0b110011110010 },       // User data segment.
    { .base = (uint32_t) &tss, .limit = sizeof(tss), .access_and_flags = 0b000011101001 }
};

static PagingChunk* kernel_paging_4gb_chunk = 0;

void kernel_panic(const char* msg) {
    putsk(msg);

    // Now halt the kernel.

    cpu_halt();

    // If in case the CPU doesn't halt, forcefully halt it using infinite loop.

    while(1) {}
}

void kernel_main() {
    // Intialize the Teletype Output screen.

    tty_init();

    // Initialize the GDT.

    memsetk(gdt_entries, 0x00, sizeof(gdt_entries));

    // Encode our structured GDT entries to real gdt entries.

    gdt_entry_from_structured(gdt_entries, gdt_entries_structured, NUTTLE_MAX_GDT_ENTRIES);

    // Now load the GDT.

    gdt_load(gdt_entries, sizeof(gdt_entries));

    // Initialize the heap.

    kheap_init();

    // Initialze the filesystems.

    fs_init();

    // Initialize the Interrupt Descriptor Table.

    idt_init();

    // Initialize the TSS.

    tss_init(&tss);

    // Load the TSS.

    tss_load(0x28);

    // Initialize the disks.

    disk_all_init();

    // Creating 4GB paging chunk.

    kernel_paging_4gb_chunk = paging_get_new_4gb_chunk(PAGING_IS_WRITABLE | PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL);

    // Set up the paging directory.

    paging_switch(kernel_paging_4gb_chunk);

    // Now enable paging.

    enable_paging();

    // Initialize the Intel 8259A PIC.

    // Intel recommends to reserve the first 32 (0x1f counting from 0) interrupt in IDT for 
    // exception handling.
    // So our new hardware ISR should start from 0x20 (0x28 for slave).

    initialize_pic(0x20, 0x28);

    // Now enable all the interrupts.

    // enable_interrupts();

    // Create a process.

    NuttleProcess* process;

    if(ISERR(process_load("0:/BLANK.BIN", &process))) {
        kernel_panic("Could not load program: BLANK.BIN\n");
    }

    process_switch(process);

    task_run();

    process_free(process);

    putsk("AsifOS rocks!\n");
}