section .asm
    global idt_load_descriptor
    global noint
    global kbd_int
    global enable

extern kbd_int_handler
extern noint_handler

idt_load_descriptor: 
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]
    lidt [ebx]

    pop ebp
    ret

; This function corresponds to no interrupt.

noint: 
    pushad
    call noint_handler
    popad
    iret

enable:
    sti
    ret

kbd_int: 
    pushad

    call kbd_int_handler

    popad
    iret