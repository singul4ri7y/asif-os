#ifndef __ASIF_OS_H__
#define __ASIF_OS_H__

#include <stddef.h>

typedef enum __enum_TermColor {
    TERM_COLOR_BLUE       = 1,
    TERM_COLOR_GREEN      = 2,
    TERM_COLOR_RED        = 4, 
    TERM_COLOR_LIGHT_BLUE = 9,
    TERM_COLOR_WHITE      = 15,
    TERM_COLOR_BROWN      = 6,
    TERM_COLOR_CYAN       = 3
} TermColor;

extern void  asifos_print(const char* message);
extern int   asifos_getkey();
extern void  asifos_putchar(char ch);
extern void  asifos_putcharc(char ch, TermColor color);
extern void  asifos_rmchar();
extern void* asifos_malloc(size_t size);
extern void  asifos_free(void* addr);
extern void  asifos_clear();

void asifos_terminal_readline(char* buf, int max, int output_while_typing);
void asifos_printc(const char* message, TermColor color);

#endif    // __ASIF_OS_H__