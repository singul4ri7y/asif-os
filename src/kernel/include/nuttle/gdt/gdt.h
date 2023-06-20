#ifndef __NUTTLE_GDT_H__
#define __NUTTLE_GDT_H__

#include <kernint.h>

typedef struct __struct_GDTEntry {
    uint16_t limit_low_16_bits;

    // First 24 bits of base.

    uint16_t base_16_bits;
    uint8_t  base_8_bits;

    // Access byte which holds Present bit, Privilage bit, type bit etc.

    uint8_t access_byte;

    // High 4 bits of flags and final (high) 4 bits of limit.

    uint8_t flags_and_limit;

    // High 8 bits of the base.

    uint8_t base_high_8_bits;
} GDTEntry;

// A simplified version of over complicated GDT entry.

typedef struct __struct_GDTEntryStructured {
    uint32_t base;
    uint32_t limit;
    uint16_t access_and_flags;
} GDTEntryStructured;

void gdt_load(GDTEntry* entry_start, int total_entries);
void gdt_entry_from_structured(GDTEntry* entries, GDTEntryStructured* structured, int total_entries);

#endif    // __NUTTLE_GDT_H__