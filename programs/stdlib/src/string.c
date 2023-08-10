#include <string.h>

size_t strlen(const char* str) {
    size_t size = 0u;

    while(*str++) size++;

    return size;
}