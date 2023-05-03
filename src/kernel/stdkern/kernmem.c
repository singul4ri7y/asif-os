#include <nuttle/kheap.h>
#include <kernmem.h>

void* memsetk(void* ptr, int value, size_t size) {
    char* const bytes = (char* const) ptr;

    char val = (char) value;

    for(size_t i = 0; i < size; i++) 
        bytes[i] = val;
    
    return ptr;
}

void* mallock(size_t size) {
    return kheap_malloc(size);
}

void freek(void* ptr) {
    kheap_free(ptr);
}