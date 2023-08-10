#include <asifos.h>

void asifos_terminal_readline(char* buf, int max, int output_while_typing) {
    int i = 0;

    for(; i < max - 1; i++) {
        int ch = asifos_getkey();

        // If the key is a carriage return, then stop filling out the buffer.

        if(ch == 13) 
            break;
        
        // Handling Backspace.

        if(ch == 8) {
            if(i > 0) {
                // It will be +1 when we continue.

                i -= 2;

                asifos_rmchar();
            } else i -= 1;

            continue;
        }

        buf[i] = (char) ch;

        if(output_while_typing) 
            asifos_putchar(ch);
    }

    buf[i] = '\0';
}

void asifos_printc(const char* str, TermColor color) {
    while(*str) 
        asifos_putcharc(*str++, color);
}