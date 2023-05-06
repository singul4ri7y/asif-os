#ifndef __NUTTLE_IDT_H__
#define __NUTTLE_IDT_H__

#include <nuttle/interrupts.h>
#include <kernint.h>

typedef enum __enum_NuttleGateType {
    NUTTLE_INTERRUPT_GATE = 0xe,    // 32-bit interrupt gate.
    NUTTLE_TRAP_GATE                // 32-bit trap gate.
} NuttleGateType;

// Holds the description of each interrupt entry in the IDT.

typedef struct __struct_IDTEntry {
    uint16_t base_low;     // Low 16-bits of the base interrupt subroutine address.
    uint16_t selector;     // The selector register to jump to the interrupt with. I will use the Code Segment selector.
    uint8_t  reserved;     // Resevered byte.
    uint8_t  flags;        // Descriptor flags, such as present, gate type and privilate levels.
    uint16_t base_high;    // High 16-bits of the base address.
} __attribute__((packed)) IDTEntry;

// The IDT descriptor, telling the CPU where to find the descriptor table.

typedef struct __struct_IDTDescriptor {
    uint16_t  limit;    // Size of the IDT table decremented by 1.
    IDTEntry* table;    // The address to the table.
} __attribute__((packed)) IDTDescriptor;

extern void idt_load_descriptor(const IDTDescriptor*);

void idt_init();
void idt_add_entry(int interrupt_no, NuttleGateType type, void* address);
void interrupt_init();

#endif    // __NUTTLE_IDT_H__