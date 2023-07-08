#ifndef __NUTTLE_TASK_H__
#define __NUTTLE_TASK_H__

#include <nuttle/paging.h>
#include <kernint.h>

typedef struct __struct_TaskRegisters {
    // General purpose registers.

    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;

    // Flags.

    uint32_t eflags;

    // Instruction pointer.

    uint32_t eip;

    // Segment registers.

    uint32_t cs, ds, fs, gs, ss, es;
} __attribute__((packed)) TaskRegisters;

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
void        task_page();
void        task_store_frame(TaskRegisters* regs);

extern void task_restore_gpr(TaskRegisters* regs);
extern void task_jump_usermode(TaskRegisters* regs);
extern void task_restore_user_segment_registers();

#endif    // __NUTTLE_TASK_H__