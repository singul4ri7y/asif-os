#include <nuttle/paging.h>
#include <nuttle/status.h>
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

uint8_t validate_alignment(void* addr) {
    return ((uint32_t) addr % PAGING_SIZE) == 0u;
}

static int paging_get_indices(void* virtual_address, uint32_t* directory_index, uint32_t* table_index, uint32_t* offset) {
    int res = NUTTLE_ALL_OK;

    if(!validate_alignment(virtual_address)) {
        res = -EINVARG;

        goto out;
    }
    
    // The virtual address is going to point to some byte memory in our virtual memory, defined by
    // a directory entry and a table entry. Now each directory entry covers a single table size, which
    // is (PAGING_TOTAL_DIRECOTRY_ENTRIES * PAGING_SIZE). So to get the directory, we just need to divide
    // the virtual address with (PAGING_TOTAL_DIRECTORY-ENTRIES * PAGING_SIZE) and to get the table index,
    // as each table entry points to 4096 (or PAGING_SIZE) pages, we have to find the modulus of the previous
    // directory division and divide it with PAGING_SIZE.

    // As all the values and macros I use regarding paging are power of 2. So, we can use bitwise logic to do all
    // the stuff efficiently. The first 10 bits (2 ^ 10 = 1023) of the address is directory index, and next 10 bits
    // are the table index and rest 12 bits are offset.

    *directory_index = ((uint32_t) virtual_address >> 22) & 0x3ff;
    *table_index     = ((uint32_t) virtual_address >> 12) & 0x3ff;

    if(offset) *offset = (uint32_t) virtual_address & 0xfff;

out: 
    return res;
}

int paging_set(uint32_t* directory, void* virt_addr, void* phy_addr) {
    int res = NUTTLE_ALL_OK;

    if(!validate_alignment(virt_addr) || !validate_alignment(phy_addr)) {
        res = -EINVARG;

        goto out;
    }
    
    uint32_t directory_index, table_index;

    res = paging_get_indices(virt_addr, &directory_index, &table_index, nullptr);

    if(res < 0) goto out;

    // Get the directory entry using directory index.

    uint32_t entry = directory[directory_index];

    // Get the table the entry is pointing to.

    uint32_t* table = (uint32_t*) (entry & ~0xfff);        // ~0xfff = 0xfffff000, which indicates the first high 20 bits.

    table[table_index] = (uint32_t) phy_addr | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITTABLE | PAGING_IS_PRESENT;

out: 
    return res;
}