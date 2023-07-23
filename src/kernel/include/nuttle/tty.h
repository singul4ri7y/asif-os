#ifndef __NUTTLE_TTY_H__
#define __NUTTLE_TTY_H__

#include <kernint.h>

#define VIDEO_MEM 0xb8000
#define VIDEO_COL 80
#define VIDEO_ROW 20
#define VIDEO_SIZ 1600

typedef enum __enum_TTYColor {
    TTY_BLUE       = 1,
    TTY_GREEN      = 2,
    TTY_LIGHT_BLUE = 9,
    TTY_WHITE      = 15,
    TTY_BROWN      = 6
} TTYColor;

#define DEF_COLOR 9

void tty_init();
void tty_putc(char ch, TTYColor);
void tty_clean();

#endif    // __NUTTLE_TTY_H__
