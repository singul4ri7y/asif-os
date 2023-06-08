#include <nuttle/heap.h>
#include <nuttle/config.h>
#include <kernmem.h>

static int validate_heap_table(void* start, void* end, HeapTable* table) {
    return ((size_t) (end - start) / NUTTLE_HEAP_BLOCK_SIZE) == table -> size ? NUTTLE_ALL_OK : -EINVARG;
}

static int validate_ptr_alignment(void* ptr) {
    return ((uint32_t) ptr % NUTTLE_HEAP_BLOCK_SIZE) == 0 ? NUTTLE_ALL_OK : -EINVARG;
}

int heap_create(Heap* heap, void* start, void* end, HeapTable* table) {
    int res = NUTTLE_ALL_OK;

    // Argument validation.

    if(validate_ptr_alignment(start) < 0 || validate_ptr_alignment(end) < 0 || validate_heap_table(start, end, table) < 0) {
        res = -EINVARG;

        goto out;
    }

    heap -> table      = table;
    heap -> start_addr = start;

    memsetk(table -> entries, HEAP_TABLE_ENTRY_FREE, sizeof(HeapTableEntry) * table -> size);

out: 
    return res;
}

static uint32_t get_total_block(uint32_t size) {
    return (size / NUTTLE_HEAP_BLOCK_SIZE) + (size % NUTTLE_HEAP_BLOCK_SIZE != 0);
}

static int heap_get_start_block(HeapTable* table, int blocks) {
    HeapTableEntry* entries = table -> entries;

    int bs = -1, bc = 0;

    for(size_t i = 0; i < table -> size; i++) {
        if(ENTRY_TAKEN(entries[i])) {
            bs = -1;
            bc = 0;

            continue;
        }

        if(bs == -1) bs = i;
        else bc++;

        if(bc == blocks) break;
    }

    if(bs < 0) 
        return -ENOMEM;
    
    return bs;
}

static void* heap_calc_block_address(Heap* heap, uint32_t block_start) {
    return (void*) heap -> start_addr + (block_start * NUTTLE_HEAP_BLOCK_SIZE);
}

static int heap_calc_address_block(Heap* heap, uint32_t address) {
    return (int) (address - (uint32_t) heap -> start_addr) / NUTTLE_HEAP_BLOCK_SIZE;
}

static void heap_mark_block_taken(HeapTable* table, int start_block, int blocks) {
    HeapTableEntry* entries = table -> entries + start_block;

    if(blocks == 1) {
        *entries = (HeapTableEntry) HEAP_TABLE_IS_FIRST | HEAP_TABLE_ENTRY_TAKEN;

        return;
    }

    HeapTableEntry entry = (HeapTableEntry) HEAP_TABLE_ENTRY_TAKEN | HEAP_TABLE_DIRECTION_HAS_NEXT;

    // The first entry, which says there is a next block, this is first entry and this block is taken.

    *entries++ = entry | HEAP_TABLE_IS_FIRST;

    blocks -= 2;

    while(blocks--) 
        *entries++ = entry;    // All the regular entries.
    
    // The final entry, which says this entry is taken, no next block and is not the first block.
    // Which will denote this is the very last block of the allocation.

    *entries = (HeapTableEntry) HEAP_TABLE_ENTRY_TAKEN;
}

static void heap_mark_block_free(HeapTable* table, int block) {
    HeapTableEntry* entries = table -> entries + block;

    int last = 0;

    while(ENTRY_TAKEN(*entries)) {
        last = ENTRY_LAST(*entries);

        *entries++ = (HeapTableEntry) HEAP_TABLE_ENTRY_FREE;

        if(last) break;
    }
}

static void* heap_malloc_blocks(Heap* heap, uint32_t blocks) {
    void* address = 0x0;

    int start_block = heap_get_start_block(heap -> table, blocks);

    if(start_block < 0) 
        return address;
    
    heap_mark_block_taken(heap -> table, start_block, blocks);

    return heap_calc_block_address(heap, start_block);
}

void* heap_malloc(Heap* heap, size_t size) {
    if(size == 0u) 
        return 0x0;
    
    return heap_malloc_blocks(heap, get_total_block(size));
}

void heap_free(Heap* heap, void* ptr) {
    heap_mark_block_free(heap -> table, heap_calc_address_block(heap, (uint32_t) ptr));
}