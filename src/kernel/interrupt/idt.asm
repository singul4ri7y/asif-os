section .asm
    global idt_load_descriptor
    global interrupt_wrapper_entry

extern interrupt_handler

idt_load_descriptor: 
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]
    lidt [ebx]

    pop ebp
    ret

; A simple macro which will be used to create every single assembly routine 
; to handle interrupts.

%macro interrupt 1
    global int%1_wrapper

    int%1_wrapper: 
        ; Push all the general purpose registers.
        ; Which will be later used to pass interrupt frame to the handler.

        pushad

        ; The stack pointer is pointing to the very last register pushed by the previous
        ; instruction, which is at the very top of the stack.

        push esp

        ; Now push the interrupt number.

        push dword %1

        ; Now call the interrupt handler.

        call interrupt_handler

        add esp, 8

        ; Pop all the gneeral purpose registers.

        popad

        iret

%endmacro

; Now create all the assembly interrupt handler subroutine.

%assign i 0

%rep 512
    interrupt i
    %assign i i + 1
%endrep

section .rodata

%macro interrupt_entry 1
    dd int%1_wrapper
%endmacro

interrupt_wrapper_entry: 

%assign i 0
%rep 512
    interrupt_entry i

    %assign i i + 1
%endrep