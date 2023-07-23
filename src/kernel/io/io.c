#include <nuttle/io.h>

// The microcontrollers are not the fastest chips to deal with.
// So this function will be used to write to an unused port to delay
// some time so that the microcontrollers can process. Linux kernel 
// does something similer.

void io_wait() {
    outb(UNUSED_PORT, 0);
}