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

int strcmpk(const char* s1, const char* s2) {
    while(*s1 != 0 && *s2 != 0) {
        int diff = *s1++ - *s2++;

        if(diff) return diff;
    }

    if(*s1 != 0 || *s2 != 0) 
        return *s1 - *s2;
    
    return 0;
}

void strlowerk(char* str) {
    while(*str) {
        if(*str >= 'A' && *str <= 'Z') 
            *str ^= ' ';
        
        str++;
    }
}

void* strcpyk(char* dest, const char* src) {
    while(*src) 
        *dest++ = *src++;

    return dest;
}