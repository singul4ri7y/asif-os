#include <stdio.h>
#include <stdbool.h>
#include <asifos.h>

int main() {
    // Clear the terminal screen.

    asifos_clear();

    asifos_printc("AsifOS v0.1.1 (negative alpha)\n\n", TERM_COLOR_LIGHT_BLUE);

    char buffer[1024];

    while(true) {
        asifos_printc("asifos# ", TERM_COLOR_CYAN);

        gets(buffer, sizeof(buffer));

        printf("\n");

        printf("%s\n", buffer);
    }
}