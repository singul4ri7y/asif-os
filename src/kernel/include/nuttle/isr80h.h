#ifndef __NUTTLE_ISR80H__
#define __NUTTLE_ISR80H__

#include <kernint.h>

typedef struct __sruct_NuttleInterruptFrame {
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
} __attribute__((packed)) NuttleInterruptFrame;

typedef void* (*ISRCommand)(NuttleInterruptFrame* frame);

typedef enum __enum_ISR80hKernelCommands {
    ISR80H_COMMAND0_SUM
} ISR80hKernelCommand;

void isr80h_init_kernel_commands();
int  isr80h_register_command(int command, ISRCommand routine);

extern void isr80h_wrapper();

#endif    // __NUTTLE_ISR80H__