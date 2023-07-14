[BITS 32]
section .asm
    global _start

_start: 
    mov eax, 0
    mov esi, message
    int 0x80
    push 100
    mov eax, [0x100]
    jmp $

section .data

message: db 'This is a message from userland!', 0