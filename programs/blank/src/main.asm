[BITS 32]
section .asm
    global _start

_start: 
    mov eax, 0
    mov esi, message
    int 0x80

    mov eax, 0
    mov esi, type
    int 0x80

    xor ecx, ecx
    
.loop: 
    call getkey
    cmp eax, 0x08
    jne .continue

    cmp ecx, 0
    je .loop
    
    dec ecx
    
    mov eax, 3
    int 0x80
    jmp .loop

.continue: 
    inc ecx

    mov esi, eax

    mov eax, 2
    int 0x80
    jmp .loop

    jmp $

getkey: 
    mov eax, 1
    int 0x80
    or eax, 0
    jz getkey
    ret

section .data

message: db 'This is a message from userland!!', 10, 0
type:    db 'Start typing: ', 0