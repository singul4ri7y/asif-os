#include <nuttle/gdt.h>
#include <nuttle/kernel.h>

// Encodes a single GDT structured entry to a readable GDT entry.

void encodeGDTEntry(uint8_t* target, GDTEntryStructured source) {
    if(source.limit > 0xfffff) 
        kernel_panic("encodeGDTEntry: Invalid argument! 20-bit limit size exceeded 0xfffff (1048575)!");

    // Encode the limit.

    target[0] = source.limit & 0xff;             // Encode the first byte.
    target[1] = (source.limit >> 0x8) & 0xff;    // Encode the next byte.
    target[6] = (source.limit >> 0x10) & 0xf;    // Final 4 bits of the limit.

    // Encode the base.

    target[2] = source.base & 0xff;              // First byte of the base.
    target[3] = (source.base >> 0x8) & 0xff;     // Next byte of the base.
    target[4] = (source.base >> 0x10) & 0xff;    // Next byte of the base.
    target[7] = (source.base >> 0x18) & 0xff;    // Final byte of the base.

    // Encode the access byte. In source, the first byte of 'access_and_flags' is the access byte.

    target[5] = source.access_and_flags & 0xff;

    // Encode the FLAGS. Next 4 bits of the 'access_and_flags' are the type flags.

    target[6] |= (source.access_and_flags >> 0x8) << 0x4;
}

void gdt_entry_from_structured(GDTEntry* entry, GDTEntryStructured* structured, int total_entries) {
    for(int i = 0; i < total_entries; i++) 
        encodeGDTEntry((uint8_t*) (entry + i), structured[i]);
}