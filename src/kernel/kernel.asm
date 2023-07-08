[BITS 32]
section .asm

global cpu_halt

cpu_halt: 
    hlt
    ret        ; If the CPU don't halt for some reason, this instruction will
               ; execute, and the caller knows what to do with it next.

; Restore the kernel registers.

global restore_kernel_segment_registers

restore_kernel_segment_registers: 
    ; 0x10 is our kernel data segment.

    mov ax, 0x10

    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax

    ret