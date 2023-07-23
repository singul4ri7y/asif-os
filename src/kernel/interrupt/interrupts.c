#include <nuttle/idt.h>
#include <nuttle/pic.h>
#include <nuttle/config.h>
#include <nuttle/isr80h.h>
#include <nuttle/status.h>
#include <nuttle/kernel.h>
#include <nuttle/task/task.h>
#include <kernio.h>
#include <kernmem.h>

// The external pointer table which holds the function pointer
// of the assembly interrupt subroutines, corresponding to 
// the index number.

// Defined in interrupt/idt.asm

extern void* interrupt_wrapper_entry[NUTTLE_TOTAL_INTERRUPTS];

static NuttleInterruptHandler interrupt_callbacks[NUTTLE_TOTAL_INTERRUPTS];

int interrupt_register_callback(int index, NuttleInterruptHandler callback) {
    int res = NUTTLE_ALL_OK;

    if(callback == nullptr || index < 0 || index >= NUTTLE_TOTAL_INTERRUPTS) {
        res = -EINVARG;

        goto out;
    }

    interrupt_callbacks[index] = callback;

out: 
    return res;
}

void interrupt_handler(int int_no, NuttleInterruptFrame* frame) {
    kernel_page();

    NuttleInterruptHandler callback = interrupt_callbacks[int_no];

    if(callback != nullptr) {
        task_store_frame((TaskRegisters*) frame);

        callback(frame);
    }

    acknowledge_int(int_no);

    task_page();
}

void interrupt_init() {
    memsetk(interrupt_callbacks, 0x00, sizeof(interrupt_callbacks));

    // Fill the IDT with default no interrupt handler.

    // Trap gates are useful to handle software interrupts and interrupt gates are 
    // useful for handling hardware interrupts, as it disables all types of interrupts while
    // processing an interrupt, making hardware interrupts more precedented over software 
    // interrupts. Kind of.

    for(int i = 0; i < NUTTLE_TOTAL_INTERRUPTS; i++) 
        idt_add_entry(i, NUTTLE_TRAP_GATE, interrupt_wrapper_entry[i]);

    idt_add_entry(0x80, NUTTLE_INTERRUPT_GATE, isr80h_wrapper);
}