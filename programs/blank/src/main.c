#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asifos.h>

int main() {
    char str[50];

    asifos_terminal_readline(str, 49, 1);

    printf(str);

    return 0;
}