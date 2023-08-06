#include <asifos.h>
#include <stdlib.h>

void* malloc(size_t size) {
    return asifos_malloc(size);
}

void free(void* ptr) {
    asifos_free(ptr);
}