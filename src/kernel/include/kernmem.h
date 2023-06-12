#ifndef __NUTTLE_KERNMEM_H__
#define __NUTTLE_KERNMEM_H__

#include <kerndef.h>

void* memsetk(void* ptr, int value, size_t size);
void* mallock(size_t size);
void* zmallock(size_t size);
void  freek(void* ptr);
int   memcmpk(void* ptr1, void* ptr2, size_t count);
void* memcpyk(void* ptr1, void* ptr2, size_t size);

#endif    // __NUTTLE_KERNMEM_H__