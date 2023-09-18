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
    TTY_RED        = 4, 
    TTY_LIGHT_BLUE = 9,
    TTY_WHITE      = 15,
    TTY_BROWN      = 6,
    TTY_CYAN       = 3
} TTYColor;

void tty_init();
void tty_putc(char ch, TTYColor);
void tty_clean();
void tty_rmc();
void tty_reset();
void tty_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);
void tty_disable_cursor();
void tty_update_cursor(uint16_t pos);

#endif    // __NUTTLE_TTY_H__
