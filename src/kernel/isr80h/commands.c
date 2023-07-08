#include <nuttle/isr80h.h>
#include <kernio.h>

static void* isr80h_command0_sum(NuttleInterruptFrame* frame) {
    frame += 0;

    putsk("Yesss!!");

    return 0;
}

void isr80h_define_commands() {
    isr80h_register_command(ISR80H_COMMAND0_SUM, isr80h_command0_sum);
}