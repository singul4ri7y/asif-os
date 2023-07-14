#include <nuttle/isr80h.h>
#include <nuttle/task/task.h>
#include <kernio.h>

static void* isr80h_command0_print(NuttleInterruptFrame* frame) {
    // Take a buffer to print (1024 characters at most).

    char buffer[1024];

    task_copy_string(task_get_current(), (void*) frame -> esi, buffer, sizeof(buffer));

    putsk(buffer);

    return nullptr;
}

void isr80h_define_commands() {
    isr80h_register_command(ISR80H_COMMAND0_PRINT, isr80h_command0_print);
}