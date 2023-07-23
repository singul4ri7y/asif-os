#include <nuttle/isr80h.h>
#include <nuttle/task/task.h>
#include <nuttle/keyboard/keyboard.h>
#include <kernio.h>

static void* isr80h_command0_print(NuttleInterruptFrame* frame) {
    // Take a buffer to print (1024 characters at most).

    char buffer[1024];

    task_copy_string(task_get_current(), (void*) frame -> esi, buffer, sizeof(buffer));

    putsk(buffer);

    return nullptr;
}

static void* isr80h_command1_getkey() {
    int x = keyboard_pop();

    return (void*) x;
}

static void* isr80h_command2_putchar(NuttleInterruptFrame* frame) {
    int ch = (int) frame -> esi;

    putck((char) ch);

    return nullptr;
}

void isr80h_define_commands() {
    isr80h_register_command(ISR80H_COMMAND0_PRINT, isr80h_command0_print);
    isr80h_register_command(ISR80H_COMMAND1_GETKEY, (ISRCommand) isr80h_command1_getkey);
    isr80h_register_command(ISR80H_COMMAND2_PUTCHAR, isr80h_command2_putchar);
}