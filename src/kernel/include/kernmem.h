#ifndef __NUTTLE_KERNMEM_H__
#define __NUTTLE_KERNMEM_H__

#include <kerndef.h>

void* memsetk(void* ptr, int value, size_t size);
void* mallock(size_t size);
void freek(void* ptr);

#endif    // __NUTTLE_KERNMEM_H__