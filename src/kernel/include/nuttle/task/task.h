#ifndef __NUTTLE_TASK_H__
#define __NUTTLE_TASK_H__

#include <nuttle/paging.h>
#include <kernint.h>

typedef struct __struct_TaskRegisters {
    // General purpose registers.

    uint32_t eax, ecx, edx, ebx, esp, epb, esi, edi;

    // Flags.

    uint32_t flags;

    // Instruction pointer.

    uint32_t ip;

    // Segment registers.

    uint32_t cs, ds, ss, es;
} TaskRegisters;

// Forward delcaration of the process structure.

typedef struct __struct_NuttleProcess NuttleProcess;

// In my kernel implementation I am going to have only one task per process.
// So, I can bind a paging chunk in a single task, cause I don't need to task
// switching and sharing same resources between tasks sharing same process.

typedef struct __struct_NuttleTask {
    PagingChunk* chunk;
    TaskRegisters registers;

    // The process where the task is linked to.

    NuttleProcess* process;

    // Tasks are double linked list.

    struct __struct_NuttleTask* prev;
    struct __struct_NuttleTask* next;
} NuttleTask;

NuttleTask* task_new(NuttleProcess* process);
void        task_free(NuttleTask* task);
void        task_switch(NuttleTask* task);
int         task_run();

extern void task_restore_gpr(TaskRegisters* regs);
extern void task_jump_usermode(TaskRegisters* regs);

#endif    // __NUTTLE_TASK_H__