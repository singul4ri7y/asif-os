#include <nuttle/task/process.h>
#include <nuttle/status.h>
#include <kerndef.h>

static NuttleKeyboard* head = nullptr;
static NuttleKeyboard* tail = nullptr;

void keyboard_init() {
    // Insert the generic PS/2 keyboard driver.

    keyboard_insert(ps2_get_keyboard());
    
    // Initialize all the keyboards.

    NuttleKeyboard* kbd = head;

    while(kbd != nullptr) {
        kbd -> init();

        kbd = kbd -> next;
    }
}

int keyboard_insert(NuttleKeyboard* keyboard) {
    int res = NUTTLE_ALL_OK;

    if(keyboard == nullptr || keyboard -> init == nullptr) {
        res = -EINVARG;

        goto out;
    }

    if(head == nullptr) {
        head = tail = keyboard;

        goto out;
    }
    
    tail -> next = keyboard;
    tail = keyboard;

out: 
    return res;
}

void keyboard_init_buffer(NuttleKeyboardBuffer* buf) {
    buf -> head = buf -> tail = buf -> size = 0;
}

static int keyboard_get_circular(int n) {
    n = n % NUTTLE_KEYBOARD_MAX_BUFFER;

    if(n < 0) 
        n += NUTTLE_KEYBOARD_MAX_BUFFER;
    
    return n;
}

// AsifOS only supports ASCII character set for now.
// Pushes the character and keybord event type in another byte to the keyboard 
// buffer.

int keyboard_push(uint16_t event) {
    int res = NUTTLE_ALL_OK;

    if(event == 0x00) {
        res = -EINVARG;

        goto out;
    }

    // Current processes keyboard buffer.

    NuttleKeyboardBuffer* kbd_buf = &process_current() -> buffer;

    // Check whether we can push to the queue.

    if(kbd_buf -> size == NUTTLE_KEYBOARD_MAX_BUFFER) {
        res = -ENOMEM;

        goto out;
    }

    // Now fill the current slot pointed by tail.

    kbd_buf -> buffer[kbd_buf -> tail] = event;

    // Now update the tail.

    kbd_buf -> tail = keyboard_get_circular(kbd_buf -> tail + 1);

    // Update the size.

    kbd_buf -> size++;

out:    
    return res;
}

int keyboard_backspace() {
    int res = NUTTLE_ALL_OK;

    // Current processes keyboard buffer.

    NuttleKeyboardBuffer* kbd_buf = &process_current() -> buffer;
    
    // Check whether we can backspace a character.

    if(kbd_buf -> size == 0) {
        res = -EEND;

        goto out;
    }

    kbd_buf -> tail = keyboard_get_circular(kbd_buf -> tail - 1);

out:    
    return res;
}

// Pops the keyboard character from the process of the current running task.

uint16_t keyboard_pop() {
    uint16_t event = 0;

    NuttleKeyboardBuffer* kbd_buf = &task_get_current() -> process -> buffer;

    // Check whether we can pop character from the queue.

    if(kbd_buf -> size == 0) 
        goto out;
    
    // Return the character we have in head.

    event = kbd_buf -> buffer[kbd_buf -> head];

    // Update the head.

    kbd_buf -> head = keyboard_get_circular(kbd_buf -> head + 1);

    // Now update the size.

    kbd_buf -> size--;

out: 
    return event;
}