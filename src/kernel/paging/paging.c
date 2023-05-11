#include <nuttle/paging.h>
#include <kernmem.h>

static uint32_t* current_directory = 0x00;

PagingChunk* paging_get_new_4gb_chunk(uint16_t flags) {
    // Each Page Directory Entry is 4 byte long.

    uint32_t* directory = (uint32_t*) zmallock(PAGING_TOTAL_DIRECTORY_ENTRIES * sizeof(uint32_t));

    for(int i = 0; i < PAGING_TOTAL_DIRECTORY_ENTRIES; i++) {
        // Well this is how it works. I designed my whole system such a way that all the addresses are 4KB
        // aligned (Divisible by 4096 or a multiple of 4096), including our heap, which uses 4KB block size.
        // Now, if all the addresses are 4KB aligned, the last 12 bits of the address are always going to be
        // zero. As we already know they are going to be zero, then why don't we use it as flags. When we need
        // to access the physical address, we are gonna zero out the low 12 bits and access it.

        // Will return a table address 4KB aligned.

        uint32_t* entry = (uint32_t*) zmallock(PAGING_TOTAL_TABLE_ENTRIES * sizeof(uint32_t));

        for(int j = 0; j < PAGING_TOTAL_TABLE_ENTRIES; j++) 
            entry[j] = ((i * PAGING_TOTAL_TABLE_ENTRIES * PAGING_SIZE) + j * PAGING_SIZE) | flags;    // The flags are using the low 12 bits, which we know will be zeroes.
        
        // The table entries (just tables) are also created in our heap. So they are 4KB aligned as well.

        directory[i] = (uint32_t) entry | flags;
    }

    PagingChunk* chunk = mallock(sizeof(PagingChunk));

    chunk -> directory = directory;

    return chunk;
}

uint32_t* paging_get_directory(PagingChunk* chunk) {
    return chunk -> directory;
}

// Defined in paging.asm

extern void paging_load_directory(void*);

void paging_switch(uint32_t* directory) {
    paging_load_directory(directory);

    current_directory = directory;
}