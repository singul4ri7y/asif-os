#ifndef __NUTTLE_IO_H__
#define __NUTTLE_IO_H__

#include <kernint.h>

extern uint8_t inb(uint16_t port);
extern uint16_t inw(uint16_t port);

extern void outb(uint16_t port, uint8_t value);
extern void outw(uint16_t port, uint16_t value);

#endif    // __NUTTLE_IO_H__