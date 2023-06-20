[BITS 32]

section .asm

global gdt_load

gdt_load: 
    ; Create frame.

    push ebp
    mov ebp, esp

    ; First stack element is the 'epb' register, second is the call frame (instruction pointer).
    ; Therefore the start address should be at ebp + 8.

    ; Reminder: C pushes the C function parameters into the stack in reverse order, so that the
    ; first parameter stays first in the stack when we travel backwards with ebp.

    mov eax, [ebp + 8]
    mov [gdt_descriptor + 2], eax

    mov eax, [ebp + 12]
    mov [gdt_descriptor], ax

    ; Now load the GDT.

    push edx

    ; Now check for the Interrupt bit in the flags register.

    pushf

    pop edx

    ; The interrupt flag is the 9th flag.

    and edx, 1 << 9

    ; Clear the interrupts.

    cli

    ; Load the GDT.

    lgdt [gdt_descriptor]

    and edx, 1 << 9
    jz .continue        ; If the IF was 0, means that the interrupts were turned off.

    sti                 ; Re-enable the interrupts.

.continue: 

    pop edx
    pop ebp
    ret

section .data

gdt_descriptor: 
    dw 0x00        ; Size of the GDT.
    dd 0x00        ; Starting address of the first entry in the GDT.