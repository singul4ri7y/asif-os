#include <nuttle/kernel.h>
#include <nuttle/keyboard/keyboard.h>
#include <nuttle/task/task.h>
#include <nuttle/status.h>
#include <nuttle/io.h>
#include <nuttle/idt.h>
#include <nuttle/error.h>

static int ps2_init();
static void ps2_keyboard_interrupt_handler();

// Keyboard scan set. I am using the scan set one.

uint8_t keyboard_scan_set[] = {
    0x00, 0x1B, '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    0x08, '\t', 'Q', 'W', 'E', 'R', 'T',
    'Y', 'U', 'I', 'O', 'P', '[', ']',
    0x0d, 0x00, 'A', 'S', 'D', 'F', 'G',
    'H', 'J', 'K', 'L', ';', '\'', '`', 
    0x00, '\\', 'Z', 'X', 'C', 'V', 'B',
    'N', 'M', ',', '.', '/', 0x00, '*',
    0x00, 0x20, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, '7', '8', '9', '-', '4', '5',
    '6', '+', '1', '2', '3', '0', '.'
};

int scan_code_size = 0;

static NuttleKeyboard classic_ps2 = {
    .name = "Classic PS/2 Keyboard",
    .init = ps2_init
};

NuttleKeyboard* ps2_get_keyboard() {
    return &classic_ps2;
}

static int ps2_wait_for_ack(uint8_t command) {
    int count = 3, res = NUTTLE_ALL_OK;

    uint8_t byte = 0;

    while(count--) {
        byte = inb(PS2_DATA_PORT);

        if(byte == PS2_KEYBOARD_ACK) 
            goto out;
        else if(byte == PS2_KEYBOARD_RESEND) {
            outb(PS2_DATA_PORT, command);

            continue;
        }
        
        break;
    }

    if(inb(PS2_DATA_PORT) != PS2_KEYBOARD_ACK) 
        res = -EIO;

out: 
    return res;
}

static int ps2_keyboard_reset() {
    int res = NUTTLE_ALL_OK;

    // This command resets the keyboard and enables self-testing mode.

    outb(PS2_DATA_PORT, PS2_KEYBOARD_RESET);

    int count = 3;

    uint8_t byte = 0;

    while(count--) {
        byte = inb(PS2_DATA_PORT);

        // If the byte is equal to 0xFC or 0xFD, that means the testing failed.

        if(byte == 0xfc || byte == 0xfd) {
            res = -EIO;

            goto out;
        }
        else if(byte == PS2_KEYBOARD_RESEND) {
            outb(PS2_DATA_PORT, PS2_KEYBOARD_RESET);

            continue;
        }
        
        if(byte == PS2_KEYBOARD_SELF_TEST_SUCC) 
            goto out;
    }

out:    
    return res;
}

static int ps2_init() {
    int res = NUTTLE_ALL_OK;

    // Initialize the scancode size.

    scan_code_size = sizeof(keyboard_scan_set) / sizeof(uint8_t);

    // The PS/2 controller intialization.

    // Disable the first and second PS/2 port so that it doesn't sent data in middle of 
    // the intialization process.

    outb(PS2_COMMAND_PORT, 0xad);    // First PS/2 port.
    outb(PS2_COMMAND_PORT, 0xa7);    // Second PS/2 port.

    // Now flush the output buffer. Now all the PS/2 devices are disabled, so the devices
    // can't send more data. Now, we are going to clear the output buffer by reading from
    // the data port until the output buffer is empty (The bit 0 of the status register is
    // set to 0).

    while(inb(PS2_STATUS_PORT) & 0x1) 
        inb(PS2_DATA_PORT);
    
    // Now test the PS/2 controller and associated devices. I am not going to use the second
    // PS/2 device (mouse) in my kernel.

    outb(PS2_COMMAND_PORT, 0xaa);

    if(inb(PS2_DATA_PORT) != PS2_CONTROLLER_TEST_OK) {
        res = -EIO;

        goto out;
    }

    outb(PS2_COMMAND_PORT, 0xab);

    if(inb(PS2_DATA_PORT) != PS2_DEVICE_TEST_OK) {
        res = -EIO;

        goto out;
    }

    // Now enable the first PS/2 device, which is in our case the keyboard.

    outb(PS2_COMMAND_PORT, 0xae);

    // Now check whether the connected device is actually a keboard.
    // The way to send data to first PS/2 device is to write to the 
    // PS/2 data port directly.

    // Disable scanning, so that the device doesn't send scan codes.

    outb(PS2_DATA_PORT, PS2_KEYBOARD_DISABLE_SCANNING);

    if(ISERR(res = ps2_wait_for_ack(PS2_KEYBOARD_DISABLE_SCANNING))) 
        goto out;
    
    // Now send the identify (0xf2) command to the device.

    outb(PS2_DATA_PORT, PS2_DEVICE_IDENTIFY);

    if(ISERR(res = ps2_wait_for_ack(PS2_DEVICE_IDENTIFY))) 
        goto out;
    
    // Now receive 2 bytes describing the device type.

    uint8_t b1 = inb(PS2_DATA_PORT);
    io_wait();

    uint8_t b2 = inb(PS2_DATA_PORT);

    // All the PS/2 keyboard types. See: https://wiki.osdev.org/%228042%22_PS/2_Controller#Detecting_PS.2F2_Device_Types

    if(b1 != 0xab || 
    (b2 != 0x85 && b2 != 0x86 && b2 != 0x41 && 
     b2 != 0xc1 && b2 != 0x54)) {
        res = -EIO;

        goto out;
    }

    // At this point, we are sure that the device is indeed a keyboard.
    // Now reset the keyboard.

    if(ISERR(res = ps2_keyboard_reset())) 
        goto out;

    // Enable the device scanning.

    outb(PS2_DATA_PORT, PS2_KEYBOARD_ENABLE_SCANNING);

    // Finally set the interrupt handler to handle the keyboard inputs.

    interrupt_register_callback(PS2_KEYBOARD_IRQ, ps2_keyboard_interrupt_handler);

out: 
    return res;
}

static char ps2_keyboard_scancode_to_char(int scancode) {
    if(scancode >= scan_code_size) 
        return '\0';
    
    return keyboard_scan_set[scancode];
}

static void ps2_keyboard_interrupt_handler() {
    kernel_page();

    // Get the scancode.

    uint8_t scancode = inb(PS2_DATA_PORT);

    while(inb(PS2_STATUS_PORT) & 0x1) 
        inb(PS2_DATA_PORT);
    
    // Check if it was a release key event. This OS doesn't yet support key press or release
    // event, it just supports input of raw characters.

    if(scancode & PS2_KEYBOARD_SCANCODE_RELEASE) 
        return;
    
    char ch = ps2_keyboard_scancode_to_char(scancode);

    if(ch != 0) 
        keyboard_push(ch);

    task_page();
}