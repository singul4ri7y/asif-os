#include <nuttle/kernel.h>
#include <nuttle/idt.h>
#include <nuttle/io.h>
#include <nuttle/pic.h>
#include <nuttle/kheap.h>
#include <kernio.h>
#include <kernmem.h>

extern void enable();

void kernel_main() {
    // Intialize the Teletype Output screen.

    tty_init();

    // Initialize the heap.

    kheap_init();

    // Initialize the Interrupt Descriptor Table.

    idt_init();

    // Initialize the Intel 8259A PIC.

    // Intel recommends to reserve the first 32 (0x1f counting from 0) interrupt in IDT for 
    // exception handling.
    // So our new hardware ISR should start from 0x20.

    initialize_pic(0x20, 0x28);

    void* ptr1 = mallock(5000);
    void* ptr2 = mallock(50);

    freek(ptr1);

    ptr1 = mallock(42);

    if(ptr2) {}

    putsk("Hello world!\n");
}