#include <nuttle/kernel.h>
#include <nuttle/idt.h>
#include <nuttle/io.h>
#include <nuttle/pic.h>
#include <nuttle/kheap.h>
#include <nuttle/paging.h>
#include <nuttle/isr80h.h>
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

static void kernel_print_ok(const char* msg) {
    putsk("[ ");
    putsck("OK", TTY_GREEN);
    putsk(" ]: ");
    putsk(msg);
    putsk("\n");
}

void kernel_panic(const char* msg) {
    putsk(msg);

    // Now halt the kernel.

    cpu_halt();

    // If in case the CPU doesn't halt, forcefully halt it using infinite loop.

    while(1) {}
}

void kernel_page() {
    restore_kernel_segment_registers();
    paging_switch(kernel_paging_4gb_chunk);
}

void kernel_main() {

    // Intialize the Teletype Output screen.

    tty_init();
    kernel_print_ok("Teletype text mode enabled, now ready to print text");

    putsk("\nBooting ");
    putsck("AsifOS v0.1.0 (alpha)\n\n", TTY_LIGHT_BLUE);

    // Initialize the GDT.

    memsetk(gdt_entries, 0x00, sizeof(gdt_entries));

    // Encode our structured GDT entries to real gdt entries.

    gdt_entry_from_structured(gdt_entries, gdt_entries_structured, NUTTLE_MAX_GDT_ENTRIES);

    // Now load the GDT.

    gdt_load(gdt_entries, sizeof(gdt_entries));
    kernel_print_ok("Kernel GDT initialized, now memory reading and execution is privilaged");

    // Initialize the heap.

    kheap_init();
    kernel_print_ok("Kernel heap initialized, now ready to manage memory");

    // Initialze the filesystems.

    fs_init();
    kernel_print_ok("Filesystem initialized, FAT16 is now ready to be used");

    // Initialize the Interrupt Descriptor Table.

    idt_init();
    kernel_print_ok("IDT initialized, now ready to handle interrupt routines");

    // Initialize the TSS.

    tss_init(&tss);

    // Load the TSS.

    tss_load(0x28);
    kernel_print_ok("TSS initialized, hardware level task-switching is now enabled");

    // Initialize the disks.

    disk_all_init();
    kernel_print_ok("Primary disk drive mounted on disk '0:'");

    // Creating 4GB paging chunk.

    kernel_paging_4gb_chunk = paging_get_new_4gb_chunk(PAGING_IS_WRITABLE | PAGING_IS_PRESENT);

    // Set up the paging directory.

    paging_switch(kernel_paging_4gb_chunk);

    // Now enable paging.

    enable_paging();
    kernel_print_ok("Enabled and initialized kernel memory paging");

    // Initialize the Intel 8259A PIC.

    // Intel recommends to reserve the first 32 (0x1f counting from 0) interrupt in IDT for 
    // exception handling.
    // So our new hardware ISR should start from 0x20 (0x28 for slave).

    initialize_pic(0x20, 0x28);
    kernel_print_ok("8259A PIC initialized, now ready to handle hardware level interrupts");

    // Now enable all the interrupts.

    // enable_interrupts();

    // Register all the kernel commands.

    isr80h_init_kernel_commands();
    kernel_print_ok("Kernel commands initialized, now ready to handle user programs");

    putsk("\n\n");

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