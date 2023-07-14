#include <nuttle/task/process.h>
#include <nuttle/config.h>
#include <nuttle/status.h>
#include <nuttle/error.h>
#include <kerndef.h>
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

out: 
    fclosek(file);

    return res;
}

static int process_fill_data(const char* filename, NuttleProcess* process) {
    int res = NUTTLE_ALL_OK;

    // Check for binary types.

    if(ISERR(res = process_load_binary(filename, process))) 
        goto out;

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

static int process_map_memory(NuttleProcess* process) {
    int res = NUTTLE_ALL_OK;

    // Check for other types of applications, such as ELF.

    if(ISERR(res = process_map_binary(process))) 
        goto out;

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