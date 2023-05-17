#ifndef __NUTTLE_PAGING_H__
#define __NUTTLE_PAGING_H__

#include <kernint.h>

typedef enum __enum_PageFlags {
    PAGING_IS_PRESENT         = 0x001,
    PAGING_IS_WRITTABLE       = 0x002,
    PAGING_ACCESS_FROM_ALL    = 0x004,
    PAGING_WRITE_THROUG_CAHCE = 0x008,
    PAGING_CACHE_DISABLED     = 0x010,
    PAGING_WAS_ACCESSED       = 0x020,
    PAGING_IS_DIRTY           = 0x040,        // To determine whether a page has been written to (Only for 4MB page sizes, if PS bit is enabled).
    PAGING_PAGE_SIZE_4MB      = 0x080,
    PAGING_NO_INVALIDATE      = 0x100,        // Tell the microprocessor not to invalidate the Table Lookaside Buffer (TLB) cache.
    PAGING_ENABLE_PAT         = 0x200         // Use the Page Attribute Table (PAT) caching if supported.
} PageFlags;

#define PAGING_SIZE                    4096        // A single page size, out PS bit in PDE is going to be 0.
#define PAGING_TOTAL_DIRECTORY_ENTRIES 1024        // Number of Page Directory entries.
#define PAGING_TOTAL_TABLE_ENTRIES     1024        // Number of Page Table entries.

typedef struct __struct_PagingChunk {
    uint32_t* directory;
} PagingChunk;

PagingChunk* paging_get_new_4gb_chunk(uint16_t flags);
uint32_t*    paging_get_directory(PagingChunk* chunk);
void         paging_switch(uint32_t* directory);
int          paging_set(uint32_t* directory, void* virt_addr, void* phy_addr);

extern void  enable_paging();

#endif    // __NUTTLE_PAGING_H__