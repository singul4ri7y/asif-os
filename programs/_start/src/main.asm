[BITS 32]

global _start
extern main

section .bootstrap_asm

_start: 
    call main

    jmp $

    ret