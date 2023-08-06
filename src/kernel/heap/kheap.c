#include <nuttle/kheap.h>
#include <nuttle/heap.h>
#include <nuttle/config.h>
#include <kernmem.h>
#include <kernio.h>

static Heap heap;
static HeapTable heap_table;

void kheap_init() {
    // First initialize the heap table.

    heap_table.entries = (HeapTableEntry*) NUTTLE_HEAP_TABLE_ADDR;
    heap_table.size    = NUTTLE_HEAP_SIZE_IN_BYTES / NUTTLE_HEAP_BLOCK_SIZE;

    // We will start our heap from 0x1000000, which is the next chunk of free memory
    // after 0x100000.

    void* start = (void*) NUTTLE_KERNEL_HEAP_START, *end = start + NUTTLE_HEAP_SIZE_IN_BYTES;

    int result = heap_create(&heap, start, end, &heap_table);

    if(result < 0) 
        putsk("Error: Failed to create heap!\n");
}

void* kheap_malloc(size_t size) {
    return heap_malloc(&heap, size);
}

void kheap_free(void* ptr) {
    heap_free(&heap, ptr);
}