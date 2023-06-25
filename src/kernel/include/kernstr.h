#ifndef __NUTTLE_KERNSTR_H__
#define __NUTTLE_KERNSTR_H__

#include <kerndef.h>

size_t strlenk(const char* str);
size_t strnlenk(const char* str, size_t maxsiz);
int    strcmpk(const char* s1, const char* s2);
void   strlowerk(char* str);
void*  strcpyk(char* dest, const char* src);


#endif    // __NUTTLE_KERNSTR_H__