#ifndef __NUTTLE_KEYBOARD_H__
#define __NUTTLE_KEYBOARD_H__

#include <nuttle/keyboard/ps2.h>
#include <nuttle/config.h>

typedef int  (*KeyboardInitFn)();
typedef void (*KeyboardPushFn)(char);

struct __struct_NuttleKeyboard {
    KeyboardInitFn init;
    KeyboardPushFn push;
    struct __struct_NuttleKeyboard* next;
    char name[30];
};

// This keyboard buffer is a data structure to store all the characters
// came straght from the keyboard into a process. This data-structure is
// a version of circular queue.

typedef struct __struct_NuttleKeyboardBuffer {
    int head;
    int tail;
    char buffer[NUTTLE_KEYBOARD_MAX_BUFFER];
} NuttleKeyboardBuffer;

void keyboard_init();
void keyboard_init_buffer(NuttleKeyboardBuffer* buf);
int  keyboard_insert(NuttleKeyboard* keyboard);
int  keyboard_push(char ch);
int  keyboard_backspace();
char keyboard_pop();

#endif    // __NUTTLE_KEYBAORD_H__