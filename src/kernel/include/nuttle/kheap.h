#ifndef __NUTTLE_KHEAP_H__
#define __NUTTLE_KHEAP_H__

#include <kerndef.h>

void kheap_init();
void* kheap_malloc(size_t size);
void kheap_free(void* ptr);

#endif    // __NUTTLE_KHEAP_H__