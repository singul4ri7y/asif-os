#include <asifos.h>
#include <stdlib.h>

int main() {
    char* ptr = malloc(512);

    ptr[0] = 'A';
    ptr[1] = 'B';
    ptr[2] = 'C';
    ptr[3] = '\n';
    ptr[4] = 0;

    asifos_print(ptr);

    free(ptr);

    asifos_print("Start typing: ");

    while(1) {
        char ch = asifos_getkey();

        if(ch != 0) 
            asifos_putchar(ch);
    }

    return 0;
}