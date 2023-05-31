[BITS 32]

section .asm

global paging_load_directory

paging_load_directory: 
    push ebp
    mov ebp, esp

    ; We need to move the directory address to control register 3 (CR3).

    mov eax, [ebp + 0x8]
    mov cr3, eax

    pop ebp
    ret

global enable_paging

enable_paging: 
    push ebp
    mov ebp, esp

    mov eax, cr0
    or eax, 0x80000000        ; Enable the 32nd bit (Paging bit) on control register 0.
    mov cr0, eax

    pop ebp
    ret