#ifndef __NUTTLE_ELF_LOADER_H__
#define __NUTTLE_ELF_LOADER_H__

#include <nuttle/config.h>
#include <nuttle/elf/elf.h>

typedef struct __struct_NuttleELFFile {
    // The path where the ELF file is located.

    char filename[NUTTLE_MAX_PATH_SIZE];

    // The memory allocated for the ELF file.

    void* elf_mem;

    // Size of the ELF file while it is loaded into memory.

    int in_memory_size;

    // Number of program headers.

    int pheader_size;

    // Allocated memories to load 'NOBITS' sections/segments. These type of sections
    // does not have any size in the ELF file.

    void* no_bits_allocs[NUTTLE_MAX_NOBITS_ALLOC];

    // Number of NOBITS section allocation in the memory.

    int no_bits_num; 

    // All the program headers.

    NuttleELF32ProgramHeader** pheaders;

} NuttleELFFile;

int  elf_load(const char* filename, NuttleELFFile** elf_file);
void elf_free(NuttleELFFile* elf_file);

#endif    // __NUTTLE_ELF_LOADER_H__