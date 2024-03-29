#ifndef __NUTTLE_ISR80H__
#define __NUTTLE_ISR80H__

#include <nuttle/idt.h>
#include <kernint.h>

typedef void* (*ISRCommand)(NuttleInterruptFrame* frame);

typedef enum __enum_ISR80hKernelCommands {
    ISR80H_COMMAND0_PRINT,
    ISR80H_COMMAND1_GETKEY,
    ISR80H_COMMAND2_PUTCHAR,
    ISR80H_COMMAND3_RMCHAR,
    ISR80H_COMMAND4_MALLOC,
    ISR80H_COMMAND5_FREE,
    ISR80H_COMMAND6_CLEAR,
    ISR80H_COMMAND7_PUTCHARC
} ISR80hKernelCommand;

void isr80h_init_kernel_commands();
int  isr80h_register_command(int command, ISRCommand routine);

extern void isr80h_wrapper();

#endif    // __NUTTLE_ISR80H__