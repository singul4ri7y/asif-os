#include <nuttle/isr80h.h>
#include <nuttle/kernel.h>
#include <nuttle/task/task.h>
#include <nuttle/config.h>
#include <nuttle/status.h>
#include <kernmem.h>

static ISRCommand commands[NUTTLE_ISR80H_MAX_COMMANDS];

// Defined in isr80h/commands.c

extern void isr80h_define_commands();

void isr80h_init_kernel_commands() {
    memsetk(commands, 0x00, sizeof(commands));

    isr80h_define_commands();
}

static void* isr80h_handle_command(int command, NuttleInterruptFrame* frame) {
    void* res = 0;

    if(command < 0 || command >= NUTTLE_ISR80H_MAX_COMMANDS) 
        goto out;

    ISRCommand cmd_func = commands[command];

    if(cmd_func == nullptr) 
        goto out;

    res = cmd_func(frame);

out: 
    return res;
}

int isr80h_register_command(int command, ISRCommand routine) {
    int res = NUTTLE_ALL_OK;

    if(command < 0 || command >= NUTTLE_ISR80H_MAX_COMMANDS) {
        res = -EINVARG;

        goto out;
    }

    if(commands[command] != nullptr) {
        res = -ETAKEN;

        goto out;
    }

    if(routine == nullptr) {
        res = -EINVARG;

        goto out;
    }

    commands[command] = routine;

out: 
    return res;
}

void* isr80h_handler(int command, NuttleInterruptFrame* frame) {
    void* res = 0;

    // Restore the kernel page.

    kernel_page();

    // Store the current task frame.

    task_store_frame((TaskRegisters*) frame);

    // Now call the command handler.

    res = isr80h_handle_command(command, frame);

    // Now go back to the user task page.

    task_page();

    return res;
}