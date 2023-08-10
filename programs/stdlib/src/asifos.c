#include <asifos.h>

void asifos_terminal_readline(char* buf, int max, int output_while_typing) {
    int i = 0;

    for(; i < max - 1; i++) {
        int ch = asifos_getkey();

        // If the key is a carriage return, then stop filling out the buffer.

        if(ch == 13) 
            break;
        else if(ch == 8 && i > 0) {
            // I will be +1 when we continue.

            i -= 2;

            asifos_rmchar();

            continue;
        }

        buf[i] = (char) ch;

        if(output_while_typing) 
            asifos_putchar(ch);
    }

    buf[i] = '\0';
}