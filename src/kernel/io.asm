section .asm
    global inb
    global inw
    global outb
    global outw

; Reads a byte from the I/O bus sent by provided port address.
; 

inb: 
    push ebp
    mov ebp, esp

    xor eax, eax            ; 0 out the EAX register.

    mov edx, [ebp + 0x8]    ; The port address in first parameter.

    in al, dx               ; Read 8 bits from provided port. EAX/AL is the return value.

    pop ebp
    ret

; Reads a word from the I/O bus sent by provided port address.
; 

inw: 
    push ebp
    mov ebp, esp

    xor eax, eax            ; 0 out the EAX register.

    mov edx, [ebp + 0x8]    ; The port address in first parameter.

    in ax, dx               ; Read 16 bits from provided port. EAX/AL is the return value.

    pop ebp
    ret

; Writes a byte to the I/O bus and sends it to provided port.
;

; C pushes function parameter into the stack in inverted manner. E.g.
; 
;     void add(int a, int b);
; 
; Calling this function will push 'b' into the stack first and then 'a'.

outb: 
    push ebp
    mov ebp, esp

    mov edx, [ebp + 0x8]    ; The port address, first parameter.
    mov eax, [ebp + 0xc]    ; The byte value, second parameter.

    out dx, al              ; Send byte to the I/O port.

    pop ebp
    ret

; Writes a word to the I/O bus and sends it to provided port.
; 

outw: 
    push ebp
    mov ebp, esp

    mov edx, [ebp + 0x8]    ; The prot addresss, first parameter.
    mov eax, [ebp + 0xc]    ; The word value, second parameter.

    out dx, ax              ; Send word to the I/O port.

    pop ebp
    ret