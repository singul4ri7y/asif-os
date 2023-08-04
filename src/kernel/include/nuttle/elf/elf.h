#ifndef __NUTTLE_ELF_H__
#define __NUTTLE_ELF_H__

#include <kernint.h>

typedef uint32_t ELF32Addr;
typedef uint32_t ELF32Word;
typedef int32_t  ELF32Off;
typedef int32_t  ELF32SWord;
typedef uint16_t ELF32Half;
typedef uint8_t  ELF32Char;

// ELF Identification header.

#define EI_MAG0        0    // File identification
#define EI_MAG1        1    // File identification
#define EI_MAG2        2    // File identification
#define EI_MAG3        3    // File identification
#define EI_CLASS       4    // File class
#define EI_DATA        5    // Data encoding
#define EI_VERSION     6    // File version
#define EI_PAD         7    // Start of padding bytes
#define EI_NIDENT     16    // ELF Identification header size.

// ELF class.

#define ELFCLASSNONE     0    // Invalid class
#define ELFCLASS32       1    // 32-bit objects
#define ELFCLASS64       2    // 64-bit objects

// ELF Data Endocding.

#define ELFDATANONE     0    // Invalid data encoding
#define ELFDATA2LSB     1    // Least Significant Byte (Little Endian)
#define ELFDATA2MSB     2    // Most Significant Byte (Big Endian)

typedef struct __struct_NuttleELF32Header {
    ELF32Char e_ident[EI_NIDENT];
    ELF32Half e_type;
    ELF32Half e_machine;
    ELF32Word e_version;
    ELF32Addr e_entry;
    ELF32Off  e_phoff;
    ELF32Off  e_shoff;
    ELF32Word e_flags;
    ELF32Half e_ehsiz;
    ELF32Half e_phentsiz;
    ELF32Half e_phentnum;
    ELF32Half e_shentsiz;
    ELF32Half e_shentnum;
    ELF32Half e_shstridx;
} __attribute__((packed)) NuttleELF32Header;

// Program header types.

#define PT_NULL       0
#define PT_LOAD       1
#define PT_DYNAMIC    2
#define PT_ITNERP     3
#define PT_NOTE       4
#define PT_SHLIB      5
#define PT_PHDR       6
#define PT_LOPROC     0x70000000
#define PT_HIPROC     0x7fffffff

// Program header flags.

#define PF_X    0x01
#define PF_W    0x02
#define PF_R    0x04

typedef struct __struct_NuttleELF32ProgramHeader {
    ELF32Word p_type;
    ELF32Off  p_offset;
    ELF32Addr p_vaddr;
    ELF32Addr p_paddr;
    ELF32Word p_filesiz;
    ELF32Word p_memsiz;
    ELF32Word p_flags;
    ELF32Word p_align;
} __attribute__((packed)) NuttleELF32ProgramHeader;

#endif    // __NUTTLE_ELF_H__