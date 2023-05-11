#include <nuttle/pic.h>
#include <nuttle/io.h>

/**
 * Reinitialize the PIC controller and giving them specific control vector offset
 * of interrupt in IDT than BIOS default.
*/

// the Intel 8259A PIC is not the fastest chip to deal with.
// So this function will be used to write to an unused port to delay
// some time so that the PIC can process. Linux kernel does something 
// similer.

static void io_wait() {
    outb(UNUSED_PORT, 0);
}

// Initilaizes the PIC with Intializaiton Command Words.

void initialize_pic(uint8_t master_offset, uint8_t slave_offset) {
    // For more info, see: https://wiki.osdev.org/8259_PIC.

    // In uninitialized state, the PIC's data register holds all the interrupt
    // masks, read from the IMR (Interrupt Mask Register).

    // Store all the interrupt masks.

    uint8_t master_mask = inb(PICM_DATA_PORT);
    uint8_t slave_mask  = inb(PICS_DATA_PORT);

    // Initialization Command Word 1.

    outb(PICM_COMMAND_PORT, ICW1_INIT | ICW1_EICW4);
    io_wait();
    outb(PICS_COMMAND_PORT, ICW1_INIT | ICW1_EICW4);
    io_wait();

    // Now after writing to the PIC's comamnd port, the PIC is now in initialization
    // state.
    // That means I can now write to the data port to configure the chip.

    // Initialization Command Word 2.

    // The offset of interrupt in the IDT.

    outb(PICM_DATA_PORT, master_offset);
    io_wait();
    outb(PICS_DATA_PORT, slave_offset);
    io_wait();

    // Intialization Command Word 3.

    // Configure the cascading for master PIC.

    outb(PICM_DATA_PORT, 0b00000100);    // The slave PIC interrupt pin is connected to the IRQ2 (3rd interrupt pin).
    io_wait();
    
    // Configure the cascading for slave PIC.

    // What we should care about is the last 3 bits which represent number from 0-7, correspoding to 
    // in which master pin the slave PIC is connected to. It's 0 based index, so in our case the number will be 
    // 2.

    outb(PICS_DATA_PORT, 0b00000010);
    io_wait();

    // Initialization Control Word 4.

    outb(PICM_DATA_PORT, ICW4_8086);
    io_wait();
    outb(PICS_DATA_PORT, ICW4_8086);
    io_wait();

    // Restore all the interrupt masks.
    // After the PIC intialization is completed, writing to the data port of
    // the PIC sets the IMR.

    outb(PICM_DATA_PORT, master_mask);
    io_wait();
    outb(PICS_DATA_PORT, slave_mask);
    io_wait();
}

// Acknowledge interrupt.

void acknowledge_int(uint8_t int_no) {
    // If the IRQ is from slave PIC, both master and slave PIC
    // has to be acknowledged.

    if(int_no > 7) 
        outb(PICS_COMMAND_PORT, 0x20);
    
    outb(PICM_COMMAND_PORT, 0x20);
}