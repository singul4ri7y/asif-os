#ifndef __NUTTLE_PIC_H__
#define __NUTTLE_PIC_H__

#include <kernint.h>

#define PICM_COMMAND_PORT  0x20
#define PICM_DATA_PORT     0x21
#define PICS_COMMAND_PORT  0xa0
#define PICS_DATA_PORT     0xa1

#define UNUSED_PORT        0x80

typedef enum __enum_PIC_ICW1 {
    ICW1_EICW4     = 0x1,        // Indicates whether ICW4 should be present.
    ICW1_SINGLE    = 0x2,        // Disables cascade mode (not recommended).
    ICW1_INTERVAL4 = 0x4,        // Sets call address interval to 8.
    ICW1_LEVEL     = 0x8,        // Sets the triggered mode. See Intel 8259A PIC datasheet for more info.
    ICW1_INIT      = 0x10        // The main initialization bit.
} PIC_ICW1;

typedef enum __enum_PIC_ICW4 {
    ICW4_8086	    = 0x01,    // Intel 8086 mode.
    ICW4_AUTO	    = 0x02,    // Auto End of Interrupt mode.
    ICW4_BUF_SLAVE	= 0x08,    // Buffered mode slave.
    ICW4_BUF_MASTER	= 0x0C,    // Buffered mode master.
    ICW4_SFNM	    = 0x10     // Special fully nested mode.
} PIC_ICW4;

void initialize_pic(uint8_t master_offset, uint8_t slave_offset);
void acknowledge_int(uint8_t int_no);

#endif    // __NUTTLE_PIC_H__