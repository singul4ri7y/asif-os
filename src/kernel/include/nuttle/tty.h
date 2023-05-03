#ifndef __NUTTLE_TTY_H__
#define __NUTTLE_TTY_H__

#include <kernint.h>

#define VIDEO_MEM 0xb8000
#define VIDEO_COL 80
#define VIDEO_ROW 20
#define VIDEO_SIZ 1600
#define DEF_COLOR 15

void tty_init();
void tty_putc(char ch);

#endif    // __NUTTLE_TTY_H__
