#ifndef __NUTTLE_CONFIG_H__
#define __NUTTLE_CONFIG_H__

#define NUTTLE_CONFIG_TOTAL_INTERRUPTS  512

// In my bootloader, I have specified my Code Segment Selector as 0x08.

#define NUTTLE_CODE_SEGMENT_SELECTOR    0x08

// I am gonna have a static 100MB heap.

#define NUTTLE_HEAP_SIZE_IN_BYTES       104857600

// 4KB block size will suffice.

#define NUTTLE_HEAP_BLOCK_SIZE          4096

// Memory addres where the heap table will reside.

#define NUTTLE_HEAP_TABLE_ADDR          0x7e00

// Default sector size for our disk.

#define NUTTLE_DEFAULT_DISK_SECTOR_SIZE 512

// Maximum path size allowed by the kernel.

#define NUTTLE_MAX_PATH_SIZE            128

#endif    // __NUTTLE_CONFIG_H__