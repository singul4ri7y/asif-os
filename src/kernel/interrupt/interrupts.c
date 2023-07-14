#include <nuttle/idt.h>
#include <nuttle/pic.h>
#include <nuttle/io.h>
#include <nuttle/config.h>
#include <nuttle/kernel.h>
#include <nuttle/isr80h.h>
#include <kernio.h>

// No interrupt handler, defined in interrupt/idt.asm

extern void noint();

void kbd_int_handler() {
    putsk("Key pressed!!\n");
    acknowledge_int(0);
}

void noint_handler() {
    acknowledge_int(0);
}

void general_protection_fault_handler() {
    kernel_panic("General protection fault!!\n");
}

void paging_fault_handler() {
    kernel_panic("Paging fault!!\n");
}

extern void kbd_int();
extern void general_protection_fault();
extern void paging_fault();

void interrupt_init() {
    // Fill the IDT with default no interrupt handler.

    // Trap gates are useful to handle software interrupts and interrupt gates are 
    // useful for handling hardware interrupts, as it disables all types of interrupts while
    // processing an interrupt, making hardware interrupts more precedented over software 
    // interrupts. Kind of.

    for(int i = 0; i < NUTTLE_CONFIG_TOTAL_INTERRUPTS; i++) 
        idt_add_entry(i, NUTTLE_TRAP_GATE, noint);

    idt_add_entry(0x21, NUTTLE_INTERRUPT_GATE, kbd_int);
    idt_add_entry(0xd, NUTTLE_TRAP_GATE, general_protection_fault);
    idt_add_entry(0x80, NUTTLE_INTERRUPT_GATE, isr80h_wrapper);
    idt_add_entry(0xe, NUTTLE_INTERRUPT_GATE, paging_fault);
}