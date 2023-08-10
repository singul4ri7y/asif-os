#ifndef __ASIF_OS_H__
#define __ASIF_OS_H__

#include <stddef.h>

extern void  asifos_print(const char* message);
extern int   asifos_getkey();
extern void  asifos_putchar(char ch);
extern void  asifos_rmchar();
extern void* asifos_malloc(size_t size);
extern void  asifos_free(void* addr);
extern void  asifos_clear();

void asifos_terminal_readline(char* buf, int max, int output_while_typing);

#endif    // __ASIF_OS_H__