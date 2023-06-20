[BITS 32]

section .asm

global tss_load

tss_load: 
    ; Store frame.

    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]

    ; Load the TSS.

    ltr ax

    pop ebp
    ret