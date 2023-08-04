#include <nuttle/elf/loader.h>
#include <nuttle/status.h>
#include <nuttle/error.h>
#include <kernint.h>
#include <kernio.h>
#include <kernmem.h>
#include <kernstr.h>

uint8_t elf_signature[] = { 0x7f, 'E', 'L', 'F' };

static int elf_loader_validate_signature(void* buffer) {
    return !memcmpk(buffer, (void*) elf_signature, sizeof(elf_signature));
}

static NuttleELF32Header* elf_loader_get_elf_header(NuttleELFFile* elf_file) {
    return (NuttleELF32Header*) elf_file -> elf_mem;
}

static int elf_loader_has_program_header(NuttleELF32Header* elf_header) {
    return elf_header -> e_phoff != 0;
}

// AsifOS does not support ELF binaries with Big Endian (MSB), for now.

static int elf_loader_validate_encoding(NuttleELF32Header* elf_header) {
    int encoding = elf_header -> e_ident[EI_DATA];

    return encoding == ELFDATANONE || encoding == ELFDATA2LSB;
}

static int elf_loader_validate_class(NuttleELF32Header* elf_header) {
    // Only 32-bit ELF binaries are supported.

    int klass = elf_header -> e_ident[EI_CLASS];

    return klass == ELFCLASS32;
}

static int elf_loader_validate_elf(NuttleELF32Header* elf_header) {
    return elf_loader_validate_signature(elf_header) &&
           elf_loader_validate_class(elf_header) &&
           elf_loader_validate_encoding(elf_header) &&
           elf_loader_has_program_header(elf_header) ? NUTTLE_ALL_OK : -EINVFORMAT;
}

static NuttleELF32ProgramHeader* elf_loader_get_program_header(NuttleELF32Header* elf_header, int index) {
    return (NuttleELF32ProgramHeader*) ((uint32_t) elf_header + elf_header -> e_phoff) + index;
}

static int elf_loader_process_pheader(NuttleELFFile* elf_file) {
    int res = NUTTLE_ALL_OK;

    NuttleELF32Header* header = elf_loader_get_elf_header(elf_file);

    // Allocate memory to store program headers.

    elf_file -> pheaders = mallock(header -> e_phentnum * sizeof(NuttleELF32ProgramHeader*));

    // Reset the program header count.

    elf_file -> pheader_size = 0;

    for(ELF32Off i = 0; i < header -> e_phentnum; i++) {
        NuttleELF32ProgramHeader* pheader = elf_loader_get_program_header(header, i);

        switch(pheader -> p_type) {
            case PT_LOAD: {
                pheader -> p_paddr = (ELF32Addr) elf_file -> elf_mem + pheader -> p_offset;

                elf_file -> pheaders[elf_file -> pheader_size++] = pheader;

                break;
            }
        }
    }

    return res;
}

static int elf_loader_load(NuttleELFFile* elf_file) {
    int res = NUTTLE_ALL_OK;

    NuttleELF32Header* header = elf_loader_get_elf_header(elf_file);

    if(ISERR(res = elf_loader_validate_elf(header))) 
        goto out;
    
    res = elf_loader_process_pheader(elf_file);

out: 
    return res;
}

int elf_load(const char* filename, NuttleELFFile** elf_file) {
    int res = NUTTLE_ALL_OK;

    NuttleELFFile* e_file = zmallock(sizeof(NuttleELFFile));

    if(ISERRP(e_file)) {
        res = -ENOMEM;

        goto out;
    }

    strcpyk(e_file -> filename, filename);

    // Now try opening the file.

    int file = fopenk(filename, "r");

    if(!fvalidk(file)) {
        res = -EIO;

        goto out;
    }

    NuttleFileStat stat;

    file_stat(file, &stat);

    // Now allocate to load the ELF file.

    e_file -> elf_mem = mallock(stat.file_size);

    // Set the size of the loaded ELF file of the memory (not the file, though they are the same).

    e_file -> in_memory_size = stat.file_size;

    if(ISERR(res = freadk(e_file -> elf_mem, 1u, stat.file_size, file))) 
        goto out;
    
    if(ISERR(res = elf_loader_load(e_file))) 
        goto out;
    
    *elf_file = e_file;

out: 
    if(ISERR(res)) 
        elf_free(e_file);

    return res;
}

void elf_free(NuttleELFFile* elf_file) {
    if(elf_file == nullptr) 
        return;
    
    if(elf_file -> elf_mem != nullptr) 
        freek(elf_file -> elf_mem);
    
    if(elf_file -> pheaders) 
        freek(elf_file -> pheaders);
    
    freek(elf_file);
}