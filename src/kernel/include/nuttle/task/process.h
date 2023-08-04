#ifndef __NUTTLE_PROCESS_H__
#define __NUTTLE_PROCESS_H__

#include <nuttle/config.h>
#include <nuttle/task/task.h>
#include <nuttle/keyboard/keyboard.h>
#include <kernint.h>

typedef enum __enum_ProcessProgramType {
    PROCESS_PROGRAM_TYPE_BINARY,
    PROCESS_PROGRAM_TYPE_ELF
} ProcessProgramType;

// A process is the kernel/operating system representation of an application/binary
// instance in the memory.

struct __struct_NuttleProcess {
    // Process id.

    uint16_t pid;

    // Path of the application binary the process derived from.

    char filename[NUTTLE_MAX_PATH_SIZE];

    // In my kernel implementation, for simplicity, I am only going to have one task per process.

    NuttleTask* task;

    // To keep track of the heap allocated memories by the user program,
    // so that if the user program forgets to free the memory, the kernel
    // can handle it to prevent memory leaking.

    void* allocations[NUTTLE_MAX_USER_ALLOCATIONS];

    // The type of program loaded by the process.

    ProcessProgramType type;

    // Physical address to the application program code. Will point to a raw bianry if it's loaded as a 
    // raw binary, or else to the NuttleELFFile structure.

    void* ptr;

    // Physical address to the stack memory of the process.

    void* stack;

    // The size of the data pointed to by the 'ptr'.

    uint32_t size;

    // Buffer for our keyboard.

    NuttleKeyboardBuffer buffer;
};

int            process_load(const char* filename, NuttleProcess** process);
NuttleProcess* process_get(int pid);
NuttleProcess* process_current();
void           process_free(NuttleProcess* process);
void           process_switch(NuttleProcess* process);
int            process_load_and_switch(const char* filename, NuttleProcess** process);

#endif    // __NUTTLE_PROCESS_H__