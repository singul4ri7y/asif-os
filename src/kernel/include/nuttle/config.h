#ifndef __NUTTLE_CONFIG_H__
#define __NUTTLE_CONFIG_H__

// Total number of interrupts supported by Nuttle.

#define NUTTLE_TOTAL_INTERRUPTS              512

// In my bootloader, I have specified my Kernel Code Segment Selector as 0x08.
// Also in the C implementation of the GDT.

// Use these code selector macros with Requested Privilage Level (RPL), 
// like NUTTLE_KERNEL_CODE_SEGMENT_SELECTOR | 0 (RPL is 0 or kernel ring).
// We can do this because the selector registers are aligned in 8 bit boundary and
// last 3 bits are free to be used.

#define NUTTLE_KERNEL_CODE_SEGMENT_SELECTOR  0x08

// I have specified my Kernel Data Segment Selector 0x10.

#define NUTTLE_KERNEL_DATA_SEGMENT_SELECTOR  0x10

// The User Code Segment Selector, which is specified after the Kernel Data 
// Segment in 'kernel.c'.

#define NUTTLE_USER_CODE_SEGMENT_SELECTOR    0x18

// The User Data Segment Selector.

#define NUTTLE_USER_DATA_SEGMENT_SELECTOR   0x20

// I am gonna have a static 100MB heap.

#define NUTTLE_HEAP_SIZE_IN_BYTES            104857600

// 4KB block size will suffice.

#define NUTTLE_HEAP_BLOCK_SIZE               4096

// Memory addres where the heap table will reside.

#define NUTTLE_HEAP_TABLE_ADDR               0x7e00

// Default sector size for our disk.

#define NUTTLE_DEFAULT_DISK_SECTOR_SIZE      512

// Maximum path size allowed by the kernel.

#define NUTTLE_MAX_PATH_SIZE                 128

// Maximum number of filesystem allowed.

#define NUTTLE_MAX_FILESYSTEMS               16

// Maximum number of file descriptors.

#define NUTTLE_MAX_FILE_DESCRIPTORS          512

// Size of a single FAT16 table entry.

#define NUTTLE_FAT16_TABLE_ENTRY_SIZE        2

// Maximum number of GDT entries.

#define NUTTLE_MAX_GDT_ENTRIES               10

// The virtual address indicating the start of the user program.

#define NUTTLE_USER_PROGRAM_VIRTUAL_ADDR     0x400000

// The size of the stack. For this educational OS, 32KB stack size will 
// suffice.

#define NUTTLE_USER_STACK_SIZE               32768

// The start of the user stack virtual address.
// These are kinda arbitrary addresses.

#define NUTTLE_USER_STACK_VIRTUAL_ADDR_START 0x3ff000

// In x86 architecture, the stack progresses in reverse.

#define NUTTLE_USER_STACK_VIRTUAL_ADDR_END   (NUTTLE_USER_STACK_VIRTUAL_ADDR_START - NUTTLE_USER_STACK_SIZE)

// The maximum number of allocations a user program can do.

#define NUTTLE_MAX_USER_ALLOCATIONS          1024

// Maximum number of processes the kernel can handle.

#define NUTTLE_MAX_PROCESSES                 16

// Maximum number of kernel commands.

#define NUTTLE_ISR80H_MAX_COMMANDS           8

// The maximum number of characters a keyboard buffer can hold.

#define NUTTLE_KEYBOARD_MAX_BUFFER          1024

#endif    // __NUTTLE_CONFIG_H__