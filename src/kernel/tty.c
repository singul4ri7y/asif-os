#include <nuttle/tty.h>

static uint16_t* tty_video_mem = (uint16_t*) VIDEO_MEM;
static uint16_t  tty_char_idx  = 0u;

static uint16_t tty_make_char(char ch, uint8_t color) {
    return (uint16_t) (color << 8) | ch;
}

void tty_clean() {
    for(int i = 0; i < VIDEO_SIZ; i++) 
        tty_video_mem[i] = tty_make_char(' ', 0);
}

void tty_init() {
    tty_clean();
}

void tty_putc(char ch, TTYColor color) {
    if(tty_char_idx == VIDEO_SIZ) {
        tty_clean();

        tty_char_idx = 0;
    }

    if(ch == '\n') {
        tty_char_idx += VIDEO_COL - (tty_char_idx % VIDEO_COL);

        return;
    }

    tty_video_mem[tty_char_idx++] = tty_make_char(ch, color);
}
