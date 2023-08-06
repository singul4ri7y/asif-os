#include <nuttle/kernel.h>
#include <nuttle/task/process.h>
#include <nuttle/config.h>
#include <nuttle/status.h>
#include <nuttle/elf/loader.h>
#include <nuttle/error.h>
#include <kernmem.h>
#include <kernio.h>
#include <kernstr.h>

static NuttleProcess* current = nullptr;
static NuttleProcess* processes[NUTTLE_MAX_PROCESSES] = { 0 };

NuttleProcess* process_current() {
    return current;
}

static int process_load_binary(const char* filename, NuttleProcess* process) {
    int res = NUTTLE_ALL_OK;

    int file = fopenk(filename, "r");

    if(!fvalidk(file)) {
        res = -EIO;

        goto out;
    }

    // Stat the file to get the file size (in bytes).

    NuttleFileStat stat;

    file_stat(file, &stat);

    // Allocate memory for the binary file.

    void* bin = mallock(stat.file_size);

    if(ISERRP(bin)) {
        res = -ENOMEM;

        goto out;
    }

    if((int) stat.file_size != freadk(bin, 1, stat.file_size, file)) {
        res = -EIO;

        goto out;
    }

    process -> ptr  = bin;
    process -> size = stat.file_size;
    
    // Set the process type.

    process -> type = PROCESS_PROGRAM_TYPE_BINARY;

out: 
    fclosek(file);

    return res;
}

static int process_load_elf(const char* filename, NuttleProcess* process) {
    int res = NUTTLE_ALL_OK;

    NuttleELFFile* elf_file = nullptr;

    if(ISERR(res = elf_load(filename, &elf_file))) 
        goto out;

    process -> ptr  = elf_file;
    process -> size = elf_file -> in_memory_size;

    // Set the process type.

    process -> type = PROCESS_PROGRAM_TYPE_ELF;
    
out: 
    return res;
}

static int process_fill_data(const char* filename, NuttleProcess* process) {
    int res = process_load_elf(filename, process);

    if(res == NUTTLE_ALL_OK) 
        goto loaded;

    // Check for binary types.

    if(ISERR(res = process_load_binary(filename, process))) 
        goto out;

loaded: 
    // Set the filename.

    strcpyk(process -> filename, filename);

out: 
    return res;
}

static int process_map_binary(NuttleProcess* process) {
    void* start = process -> ptr,
         *end   = paging_align_addr(start + process -> size);
    
    return paging_map_to(process -> task -> chunk, (void*) NUTTLE_USER_PROGRAM_VIRTUAL_ADDR, start, end, 
        PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);
}

static int process_map_elf(NuttleProcess* process) {
    int res = NUTTLE_ALL_OK;

    NuttleELFFile* elf_file = (NuttleELFFile*) process -> ptr;

    // Now map all the program headers.

    for(int i = 0; i < elf_file -> pheader_size; i++) {
        NuttleELF32ProgramHeader* pheader = elf_file -> pheaders[i];

        // The physical address the program header is been mapped to 
        // has already been set while loading the elf binary.

        PageFlags flags = PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL;

        if(pheader -> p_flags & PF_W) 
            flags |= PAGING_IS_WRITABLE;
        
        // In ELF binary types, it is ensured that all the virtual and physical address will be 
        // 4KB aligned.

        void* start = (void*) pheader -> p_paddr,
               *end = paging_align_addr((void*) pheader -> p_paddr + pheader -> p_memsiz);

        if(ISERR(res = paging_map_to(process -> task -> chunk, (void*) pheader -> p_vaddr, start, end, flags))) 
            goto out;
    }

out: 
    return res;
}

static int process_map_memory(NuttleProcess* process) {
    int res = NUTTLE_ALL_OK;

    switch(process -> type) {
        case PROCESS_PROGRAM_TYPE_ELF: 
            if(ISERR(res = process_map_elf(process)))
                goto out;
            
            break;
        
        case PROCESS_PROGRAM_TYPE_BINARY: 
            if(ISERR(res = process_map_binary(process))) 
                goto out;
            
            break;
        
        default: 
            kernel_panic("process_map_memory(): Invalid process type!");
    }

    // Now map the stack.

    void* start = process -> stack, 
           *end = paging_align_addr(process -> stack + NUTTLE_USER_STACK_SIZE);

    res = paging_map_to(process -> task -> chunk, (void*) NUTTLE_USER_STACK_VIRTUAL_ADDR_END, start, end, PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE | PAGING_IS_PRESENT);

out: 
    return res;
}

static int process_load_to_slot(const char* filename, NuttleProcess** process, int slot) {
    int res = NUTTLE_ALL_OK;

    // Allocate the process.

    NuttleProcess* _process = zmallock(sizeof(NuttleProcess));

    if(ISERRP(_process)) {
        res = -ENOMEM;

        goto out;
    }

    if(ISERR(res = process_fill_data(filename, _process))) 
        goto out;
    
    // Set the program stack pointer.

    _process -> stack = zmallock(NUTTLE_USER_STACK_SIZE);

    if(ISERRP(_process -> stack)) {
        res = -ENOMEM;

        goto out;
    }

    // Create a new task.

    NuttleTask* task = task_new(_process);

    if(ISERRP(task)) {
        res = ERROR_I(task);

        goto out;
    }

    // Set the task.

    _process -> task = task;

    // Map the process physical memory to virtual addresses.

    if(ISERR(res = process_map_memory(_process))) 
        return res;
    
    // Initialize the keyboard buffer.

    keyboard_init_buffer(&_process -> buffer);
    
    // Now, as we are good to go, setup the process.

    *process = _process;

    processes[slot] = _process;

out: 
    if(ISERR(res) && _process != nullptr) 
        process_free(_process);

    return res;
}

static int process_get_free_slot() {
    int res = -ENOMEM;

    for(int i = 0; i < NUTTLE_MAX_PROCESSES; i++) {
        if(processes[i] == 0x00) {
            res = i;

            break;
        }
    }

    return res;
}

NuttleProcess* process_get(int pid) {
    NuttleProcess* process = nullptr;

    if(pid < 0 || pid >= NUTTLE_MAX_PROCESSES) {
        process = ERROR_P(-EINVARG);

        goto out;
    }
    
    process = processes[pid];

out: 
    return process;
}

int process_load(const char* filename, NuttleProcess** process) {
    int res = 0;

    int slot = process_get_free_slot();

    if(slot < 0) {
        res = -ENOMEM;

        goto out;
    }

    res = process_load_to_slot(filename, process, slot);

out: 
    return res;
}

void process_free(NuttleProcess* process) {
    if(ISERRP(process)) 
        goto out;
    
    // Free the task.

    if(process -> task) 
        task_free(process -> task);

    // Free the program and stack.

    if(process -> stack) 
        freek(process -> stack);

    if(process -> ptr) 
        freek(process -> ptr);
    
    // Finally free the process.

    freek(process);

out: 
    return;
}

void process_switch(NuttleProcess* process) {
    current = process;

    task_switch(process -> task);
}

int process_load_and_switch(const char* filename, NuttleProcess** process) {
    int res = process_load(filename, process);

    if(ISERR(res)) 
        goto out;
    
    process_switch(*process);

out: 
    return res;
}

static int process_get_free_allocation_slot(NuttleProcess* process) {
    int res = -ENOMEM;

    for(int i = 0; i < NUTTLE_MAX_USER_ALLOCATIONS; i++) {
        if(process -> allocations[i].addr == 0) {
            res = i;

            goto out;
        }
    }

out: 
    return res;
}

static int process_find_allocation_slot(NuttleProcess* process, void* virt) {
    int res = -EINVARG;

    for(int i = 0; i < NUTTLE_MAX_USER_ALLOCATIONS; i++) {
        if(process -> allocations[i].addr == virt) {
            res = i;

            goto out;
        }
    }

out: 
    return res;
}

static int process_calculate_pages(size_t size) {
    int res = size / PAGING_SIZE;

    if(size % PAGING_SIZE) 
        res++;

    return res; 
}

void* process_alloc_malloc(NuttleProcess* process, size_t size) {
    void* ptr = nullptr;

    // First see that the process can allocate more memory, in other words
    // there is a free spot in the allocation table if the process.

    int slot = process_get_free_allocation_slot(process);

    if(slot < 0) 
        goto out;
    
    ptr = mallock(size);

    if(ISERRP(ptr)) 
        goto out;

    void* heap_virt = (void*) NUTTLE_USER_HEAP_VIRT_ADDR + ((uint32_t) ptr - NUTTLE_KERNEL_HEAP_START);

    // Now map the allocated heap memory to process page.

    paging_map_to(process -> task -> chunk, heap_virt, ptr, paging_align_addr(ptr + size), PAGING_IS_PRESENT | PAGING_ACCESS_FROM_ALL | PAGING_IS_WRITABLE);

    // Now set and return the virtual address.

    ptr = heap_virt;

    // Add it to the allocation table.

    process -> allocations[slot].addr  = ptr;
    process -> allocations[slot].pages = process_calculate_pages(size);

out: 
    return ptr;
}

void process_alloc_free(NuttleProcess* process, void* virt_addr) {
    // Check whether the memory was initially created by the process.

    int slot = process_find_allocation_slot(process, virt_addr);

    if(slot < 0) 
        return;

    // Now get the physical address of the memory and free the memory.

    freek(paging_get_physical_addr(process -> task -> chunk, virt_addr));
    
    // Now remove mapping of the heap allocated memory.

    paging_map_to(process -> task -> chunk, virt_addr, virt_addr, virt_addr + process -> allocations[slot].pages * PAGING_SIZE, PAGING_IS_PRESENT | PAGING_IS_WRITABLE);

    // Update the allocation table.

    process -> allocations[slot].addr = 0x0;
}