#ifndef __NUTTLE_KEYBOARD_H__
#define __NUTTLE_KEYBOARD_H__

#include <nuttle/keyboard/ps2.h>
#include <nuttle/config.h>
#include <kernint.h>

typedef enum __enum_CapsLockState {
    CAPSLOCK_STATE_ON,
    CAPSLOCK_STATE_OFF
} CapsLockState;

typedef enum __enum_ShiftState {
    SHIFT_RELEASED,
    SHIFT_PRESSED
} ShiftState;

typedef int (*KeyboardInitFn)();

struct __struct_NuttleKeyboard {
    CapsLockState cl_state;
    ShiftState sh_state;
    KeyboardInitFn init;
    struct __struct_NuttleKeyboard* next;
    char name[30];
};

// This keyboard buffer is a data structure to store all the characters
// came straght from the keyboard into a process. This data-structure is
// a version of circular queue.

typedef struct __struct_NuttleKeyboardBuffer {
    int head;
    int tail;
    int size;

    // Buffer to store keyboard events. The first byte is the character related to
    // the event (uppercase or lowercase), the second byte indicates details regarding
    // the event, like was the key pressed or released etc. Bytes are in LSB order.

    uint16_t buffer[NUTTLE_KEYBOARD_MAX_BUFFER];
} NuttleKeyboardBuffer;

void      keyboard_init();
void      keyboard_init_buffer(NuttleKeyboardBuffer* buf);
int       keyboard_insert(NuttleKeyboard* keyboard);
int       keyboard_push(uint16_t ch);
int       keyboard_backspace();
uint16_t  keyboard_pop();

#endif    // __NUTTLE_KEYBAORD_H__