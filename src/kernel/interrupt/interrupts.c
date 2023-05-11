#include <nuttle/idt.h>
#include <nuttle/pic.h>
#include <nuttle/io.h>
#include <nuttle/config.h>
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

extern void kbd_int();

void interrupt_init() {
    // Fill the IDT with default no interrupt handler.

    // Trap gates are useful to handle software interrupts and interrupt gates are 
    // useful for handling hardware interrupts, as it disables all types of interrupts while
    // processing an interrupt, making hardware interrupts more precedented over software 
    // interrupts. Kind of.

    for(int i = 0; i < NUTTLE_CONFIG_TOTAL_INTERRUPTS; i++) 
        idt_add_entry(i, NUTTLE_TRAP_GATE, noint);

    idt_add_entry(0, NUTTLE_TRAP_GATE, kbd_int);
    idt_add_entry(0x21, NUTTLE_INTERRUPT_GATE, kbd_int);
}