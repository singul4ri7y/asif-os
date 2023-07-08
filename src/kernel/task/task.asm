[BITS 32]
section .asm

global task_restore_gpr
global task_jump_usermode
global task_restore_user_segment_registers


; void task_jump_usermode(TaskRegisters* regs);

task_jump_usermode: 
    ; The iret instruction works in the following way: 
    ;   It requires 5 elements to be at stack in order respectively, EIP, CS, EFLAGS, ESP and SS.
    ;   The iret instruction pops the EIP, CS, EFLAGS and ESP from the stack, but not the SS.
    ;   The iret instruction set's the values of EIP, CS, EFLAGS and ESP respectively and looks
    ;   up to the stack for the SS (without poping it). The SS and CS are set simultaneously, so
    ;   there's no need to set the SS explicitly. But the SS doesn't get popped, for god knows why -_-.

    mov ebp, esp

    ; Get the structure passed to us.

    mov esi, [ebp + 4]

    ; Push the Stack Segment.

    push dword [esi + 56]

    ; Push the stack pointer (Here I will push the user stack pointer that is created for every task).

    push dword [esi + 12]

    ; Push the flags, enabling the interrupts. I will not enable the interrupts using the STI instruction, 
    ; cause it will also enable the interrupts for the kernel. Interrupts for the kernel may or may not be 
    ; enabled at this point, depends on the kernel state. But we need the interrupts turned on for the 
    ; usermode, period.

    pushfd
    pop eax

    ; The 10th flag is the interrupt flag (IF).

    or eax, 1 << 9

    push eax

    ; Now push the Code Segment.

    push dword [esi + 40]

    ; Push the Intruction Pointer (EIP).

    push dword [esi + 36]

    ; Finally restore the previous task execution frame, if the task ran before.

    push dword [ebp + 4]
    call task_restore_gpr
    add esp, 4

    ; We are ready to go the the usermode!

    iret

; void task_restore_gpr(TaskRegisters* regs);

task_restore_gpr: 
    ; No need to create any frame using ebp, cause it's going to be changed anyway.

    mov ebp, esp

    mov edi, [ebp + 4]

    mov eax, [edi + 28]
    mov ecx, [edi + 24]
    mov edx, [edi + 20]
    mov ebx, [edi + 16]
    mov ebp, [edi + 8]
    mov esi, [edi + 4]

    ; The segment registers.

    mov ds, [edi + 44]
    mov fs, [edi + 48]
    mov gs, [edi + 52]
    mov es, [edi + 60]

    mov edi, [edi]

    ret

task_restore_user_segment_registers: 
    ; Our user data segment register GDT offset is 32 (0x20).
    ; So the segment value should be with RPL 35 (0x23).

    mov ax, 0x23

    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax

    ret