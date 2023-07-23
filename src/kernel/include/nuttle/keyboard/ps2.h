#ifndef __NUTTLE_PS2_KBD__
#define __NUTTLE_PS2_KBD__

#define PS2_DATA_PORT       0x60
#define PS2_STATUS_PORT     0x64    // When reading.
#define PS2_COMMAND_PORT    0x64    // When writing.

#define PS2_CONTROLLER_TEST_OK    0x55
#define PS2_DEVICE_TEST_OK        0x00

// Keyboard/Device specific macros.

#define PS2_KEYBOARD_ENABLE_SCANNING   0xf4
#define PS2_KEYBOARD_DISABLE_SCANNING  0xf5
#define PS2_KEYBOARD_ACK               0xfa
#define PS2_KEYBOARD_RESEND            0xfe
#define PS2_KEYBOARD_RESET             0xff
#define PS2_KEYBOARD_SELF_TEST_SUCC    0xaa
#define PS2_KEYBOARD_IRQ               0x21
#define PS2_KEYBOARD_SCANCODE_RELEASE  0x80    // PS/2 keyboard sends scan codes, which indicates press event or release event of a key.

#define PS2_DEVICE_IDENTIFY    0xf2

// Forward delcaration.

typedef struct __struct_NuttleKeyboard NuttleKeyboard;

NuttleKeyboard* ps2_get_keyboard();

#endif    // __NUTTLE_PS2_KBD__