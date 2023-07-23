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
    buf -> head = buf -> tail = 0;
}

static int keyboard_get_circular(int n) {
    n = n % NUTTLE_KEYBOARD_MAX_BUFFER;

    if(n < 0) 
        n += NUTTLE_KEYBOARD_MAX_BUFFER;
    
    return n;
}

// AsifOS only supports ASCII character set for now.
// Pushes the character gotten from keyboard to current process.

int keyboard_push(char ch) {
    int res = NUTTLE_ALL_OK;

    // Current processes keyboard buffer.

    NuttleKeyboardBuffer* kbd_buf = &process_current() -> buffer;

    // If the tail of the circular queue hits the head, suggests that the buffe is full.

    int _tail = keyboard_get_circular(kbd_buf -> tail + 1);

    if(_tail == kbd_buf -> head) {
        res = -ENOMEM;

        goto out;
    }

    kbd_buf -> buffer[_tail] = ch;
    kbd_buf -> tail = _tail;

out:    
    return res;
}

int keyboard_backspace() {
    int res = NUTTLE_ALL_OK;

    // Current processes keyboard buffer.

    NuttleKeyboardBuffer* kbd_buf = &process_current() -> buffer;
    
    // While backspacing we delete a character. If the tail of the buffer
    // is already pointing to the same slot as the head, that means we 
    // have no characters to backspace.

    if(kbd_buf -> head == kbd_buf -> tail) {
        res = -EEND;

        goto out;
    }

    kbd_buf -> tail = keyboard_get_circular(kbd_buf -> tail - 1);

out:    
    return res;
}

// Pops the keyboard character from the process of the current running task.

char keyboard_pop() {
    char ch = '\0';

    NuttleKeyboardBuffer* kbd_buf = &task_get_current() -> process -> buffer;

    // If the head and the tail points to the same slot, that means we have reached
    // the end of the queue and we can no longer pop. In this case, return the 
    // null/termination character.

    if(kbd_buf -> head == kbd_buf -> tail) 
        goto out;

    ch = kbd_buf -> buffer[kbd_buf -> head];

    kbd_buf -> head = keyboard_get_circular(kbd_buf -> head + 1);

out: 
    return ch;
}