#include <asifos.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static char* itoa(int n) {
    static char priv_mem[14];
    
    int loc = 13;

    priv_mem[loc] = '\0';
    
    int neg = n < 0;

    if(neg) n = -n;

    while(n) {
        priv_mem[--loc] = (n % 10) + '0';

        n /= 10;
    }

    if(neg) priv_mem[--loc] = '-';

    return priv_mem + loc;
}

int printf(const char* format, ...) {
    int count = 0;

    int* argp = (int*) &format;

    for(; *format; format++) {
        if(*format != '%') {
            asifos_putchar(*format);

            count++;

            continue;
        }

        switch(*++format) {
            case 's': {
                char* str = *(char**) ++argp;

                count += strlen(str);

                asifos_print(str);

                break;
            }

            case 'd': {
                int x = *++argp;

                char* str = itoa(x);

                count += strlen(str);

                asifos_print(str);

                break;
            }
        }
    }

    return count;
}

int putchar(int ch) {
    asifos_putchar((char) ch);

    return ch;
}

int getchar() {
    return asifos_getkey();
}

char* gets(char* buf, int max) {
    if(buf == NULL) 
        return NULL;
    
    asifos_terminal_readline(buf, max, true);

    return buf;
}