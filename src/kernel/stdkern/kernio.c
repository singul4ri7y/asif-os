#include <kernio.h>
#include <nuttle/tty.h>

void putsk(const char* str) {
    while(*str) 
        tty_putc(*str++);
}