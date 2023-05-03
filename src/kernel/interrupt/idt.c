#include <nuttle/idt.h>
#include <nuttle/config.h>
#include <kernmem.h>

static IDTEntry idt[NUTTLE_CONFIG_TOTAL_INTERRUPTS];
static IDTDescriptor idt_desc;

void idt_init() {
    // Fill all the IDT entries with 0s.

    memsetk(idt, 0, sizeof(idt));

    // Initialize the table descriptor.

    idt_desc = (IDTDescriptor) { sizeof(idt) - 1, idt };

    idt_load_descriptor(&idt_desc);

    // Now setup all the Interrupt Service Routine (ISR).

    interrupt_init();
}

void idt_add_entry(int interrupt_no, NuttleGateType type, void* address) {
    IDTEntry* entry = idt + interrupt_no;

    entry -> base_low  = (uint32_t) address & 0x0000ffff;
    entry -> selector  = NUTTLE_CODE_SEGMENT_SELECTOR;
    entry -> reserved  = 0;

    // Add the gate type provided.

    entry -> flags     = (0b1110 << 4) | type;

    entry -> base_high = (uint32_t) address >> 16;
}