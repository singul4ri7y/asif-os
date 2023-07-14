section .asm
    global idt_load_descriptor
    global noint
    global kbd_int
    global general_protection_fault
    global paging_fault

extern kbd_int_handler
extern noint_handler
extern general_protection_fault_handler
extern paging_fault_handler

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

kbd_int: 
    pushad

    call kbd_int_handler

    popad
    iret

general_protection_fault: 
    pushad 

    call general_protection_fault_handler

    popad
    iret

paging_fault: 
    pushad

    call paging_fault_handler

    popad
    iret