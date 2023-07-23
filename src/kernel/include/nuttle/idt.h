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

// The interrupt frame holding data regarding the registers
// when the interrupt has been called.

typedef struct __sruct_NuttleInterruptFrame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} __attribute__((packed)) NuttleInterruptFrame;

typedef void (*NuttleInterruptHandler)(NuttleInterruptFrame* frame);

extern void idt_load_descriptor(const IDTDescriptor*);

void idt_init();
void idt_add_entry(int interrupt_no, NuttleGateType type, void* address);
void interrupt_init();
int interrupt_register_callback(int index, NuttleInterruptHandler callback);

#endif    // __NUTTLE_IDT_H__