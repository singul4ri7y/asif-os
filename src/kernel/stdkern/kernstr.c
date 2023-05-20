#include <kernstr.h>

size_t strlenk(const char* str) {
    size_t size = 0u;

    while(*str++) size++;

    return size;
}

size_t strnlenk(const char* str, size_t maxsiz) {
    size_t size = strlenk(str);

    return size > maxsiz ? maxsiz : size;
}