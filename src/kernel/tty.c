#include <nuttle/tty.h>
#include <nuttle/io.h>

static uint16_t* tty_video_mem = (uint16_t*) VIDEO_MEM;
static uint16_t  tty_char_idx  = 0u;

static uint16_t tty_make_char(char ch, uint8_t color) {
    return (uint16_t) (color << 8) | ch;
}

void tty_clean() {
    for(int i = 0; i < VIDEO_SIZ; i++) 
        tty_video_mem[i] = tty_make_char(' ', TTY_WHITE);
}

void tty_init() {
    tty_clean();
}

void tty_reset() {
    tty_char_idx = 0;
}

void tty_putc(char ch, TTYColor color) {
    if(tty_char_idx == VIDEO_SIZ) {
        tty_clean();

        tty_char_idx = 0;
    }

    if(ch == '\n') {
        tty_char_idx += VIDEO_COL - (tty_char_idx % VIDEO_COL);

        goto out;
    }

    tty_video_mem[tty_char_idx++] = tty_make_char(ch, color);

out: 
    tty_update_cursor(tty_char_idx);
}

void tty_rmc() {
    if(tty_char_idx == 0) 
        return;
    
    tty_char_idx--;

    tty_video_mem[tty_char_idx] = tty_make_char(' ', TTY_WHITE);

    // Update the cursor position.

    tty_update_cursor(tty_char_idx);
}

void tty_enable_cursor(uint8_t cursor_start, uint8_t cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);

    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void tty_disable_cursor() {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void tty_update_cursor(uint16_t pos) {
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
