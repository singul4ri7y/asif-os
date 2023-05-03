#ifndef __NUTTLE_HEAP_H__
#define __NUTTLE_HEAP_H__

#include <kernint.h>
#include <kerndef.h>
#include <nuttle/status.h>

/**
 * In my heap implementation, I have something called Heap Entry Table, of which
 * each entry corresponds to a single block, where a block size is 4KB. Entry entry
 * consists of 1 byte, denoting the correspoding blocks state.
 * 
 * From each byte, what we care is the last 4 bits and the first 2 bits. The first
 * 4 bits which denotes the type of the entry such as, is the block taken (part of allocation) 
 * or free to be allocated.
 * 
 * And the first 2 bits with respectively denote "Is the next block allocated" and "Is it the first block of an allocation".
 * 
*/

typedef enum __enum_HeapTableEntryType {
    HEAP_TABLE_ENTRY_TAKEN = 0xf,
    HEAP_TABLE_ENTRY_FREE  = 0x1
} HeapTableEntryType;

typedef enum __enum_HeapTableDirectionType {
    HEAP_TABLE_DIRECTION_HAS_NEXT = 0x80,
    HEAP_TABLE_IS_FIRST           = 0x40
} HeapTableDirectionType;

typedef uint8_t HeapTableEntry;

#define ENTRY_TAKEN(entry) ((entry & HEAP_TABLE_ENTRY_TAKEN) == HEAP_TABLE_ENTRY_TAKEN)
#define ENTRY_FREE(entry) ((entry & HEAP_TABLE_ENTRY_FREE) == HEAP_TABLE_ENTRY_FREE)
#define ENTRY_LAST(entry) ((entry & HEAP_TABLE_ENTRY_TAKEN) == HEAP_TABLE_ENTRY_TAKEN && (entry & 0xc0) != 0xc0)

typedef struct __struct_HeapTable {
    HeapTableEntry* entries;
    size_t size;
} HeapTable;

typedef struct __struct_Heap {
    HeapTable* table;
    void* start_addr;    // Address to start the heap blocks from.
} Heap;

int heap_create(Heap* heap, void* start, void* end, HeapTable* table);
void* heap_malloc(Heap* heap, size_t size);
void heap_free(Heap* heap, void* ptr);

#endif