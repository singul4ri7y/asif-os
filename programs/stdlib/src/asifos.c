#include <asifos.h>

void asifos_terminal_readline(char* buf, int max, int output_while_typing) {
    int i = 0;

    for(; i < max - 1; i++) {
        int ev = asifos_getkey();

        // We don't care about the release event.

        if((ev >> 8) > 0) {
            i--;

            continue;
        }

        // Now just process the character.

        ev &= 0xff;

        // If the key is a carriage return, then stop filling out the buffer.

        if(ev == 13) 
            break;
        
        // Handling Backspace.

        if(ev == 8) {
            if(i > 0) {
                // It will be +1 when we continue.

                i -= 2;

                asifos_rmchar();
            } else i -= 1;

            continue;
        }

        buf[i] = (char) ev;

        if(output_while_typing) 
            asifos_putchar(ev);
    }

    buf[i] = '\0';
}

void asifos_printc(const char* str, TermColor color) {
    while(*str) 
        asifos_putcharc(*str++, color);
}