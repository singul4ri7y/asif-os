#include <nuttle/kheap.h>
#include <kernmem.h>
#include <kernint.h>

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

void* zmallock(size_t size) {
    void* ptr = mallock(size);

    if(ptr) memsetk(ptr, 0, size);

    return ptr;
}

void freek(void* ptr) {
    kheap_free(ptr);
}

int memcmpk(void* ptr1, void* ptr2, size_t count) {
    int8_t* mem1 = (int8_t*) ptr1, *mem2 = (int8_t*) ptr2;

    int diff = 0;

    while(count--) {
        diff = *mem1++ - *mem2++;

        if(diff != 0) return diff;
    }

    return 0;
}