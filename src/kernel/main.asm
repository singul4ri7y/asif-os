[BITS 32]

; My kernel consists of Assembly and pure C code and is likned together.
; But in the linker script we defined that all our assembly code without any
; explicit section would be put at the very last of the binary. But as this
; Assembly file is the entry point to the kernel, we can't have that at the 
; very back. So, I am creating a custom section to put all the bootstrap 
; assembly code and to put it at the top of the binary file also putting all
; the rest of the assembly file at the end.

section .bootstrap

; Writing a kernel in Assembly language is kinda pain. So, I will call the kernel
; C entry point function and start writing the whole kernel from there.

extern kernel_main

global kernel_init

kernel_init: 
    ; Call the C entry point function.

    call kernel_main

end: 
    ; Halt the CPU.

    hlt

; If in some case the processor core doesn't stop executing, create a infinite
; jump loop to reduce further code execution.

jmp $