#include <stdio.h>
#include <stdbool.h>

int main() {
    printf("AsifOS v0.1.1 (negative alpha)");

    char buffer[1024];

    while(true) {
        printf("asifos# ");

        gets(buffer, sizeof(buffer));

        printf("\n");

        printf("%s\n", buffer);
    }
}