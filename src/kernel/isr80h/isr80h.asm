[BITS 32]
section .asm

global isr80h_wrapper

extern isr80h_handler

isr80h_wrapper: 
    ; When an interrupt is called, the kernel register frame does not get restored. That means, all the 
    ; general purpose registers set at this very moment are values set by the userland. So, we are storing 
    ; them as the registers are going to be changed later on.

    pushad

    ; Now the stack pointer is pointing to the very last register pushed by the 'pushad' instruction.
    ; That means we can pass the ESP address to the C callee function which then later on going to be
    ; used as pointer to a structure.

    push esp

    ; The command number.

    push eax

    ; As default C calling convention passes all the parameters to the stack by pushing them on to the stack
    ; in reverse order, the callee function call would look like this: 
    ;    isr80h_handler(command [eax], interrupt_frame [esp]);

    call isr80h_handler

    add esp, 8

    mov dword [tmp_value], eax        ; Store the EAX register value temporarily.

    ; Restore the general purpose registers frame used by the userland.

    popad

    ; EAX register holds the return value.

    mov eax, [tmp_value]

    iret

section .data

tmp_value: dd 0