[BITS 32]

section .asm

global asifos_print:function

asifos_print: 
    push ebp
    push eax
    push esi

    mov ebp, esp

    mov esi, [ebp + 16]
    mov eax, 0    ; Kernel command print.
    int 0x80

    pop esi
    pop eax
    pop ebp
    ret

global asifos_getkey:function

asifos_getkey: 
    push ebp
    mov ebp, esp

.loop: 
    mov eax, 1
    int 0x80

    or eax, 0
    jz .loop

    pop ebp
    ret

global asifos_putchar:function

asifos_putchar:
    push ebp
    push eax
    push esi
    mov ebp, esp

    mov esi, [ebp + 16]
    mov eax, 2
    int 0x80

    pop esi
    pop eax
    pop ebp
    ret

global asifos_rmchar:function

asifos_rmchar: 
    push ebp
    push eax
    mov ebp, esp

    mov eax, 3
    int 0x80

    pop eax
    pop ebp
    ret

global asifos_malloc:function

asifos_malloc: 
    push ebp
    push esi

    mov ebp, esp

    mov esi, [ebp + 12]
    mov eax, 4
    int 0x80

    pop esi
    pop ebp
    ret

global asifos_free:function

asifos_free: 
    push ebp
    push esi
    push eax

    mov ebp, esp

    mov esi, [ebp + 16]
    mov eax, 5
    int 0x80

    pop eax
    pop esi
    pop ebp
    ret

global asifos_clear:function

asifos_clear: 
    push ebp
    push eax
    mov ebp, esp

    mov eax, 6
    int 0x80

    pop eax
    pop ebp
    ret