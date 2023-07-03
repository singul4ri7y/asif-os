[BITS 32]
section .asm

global task_restore_gpr
global task_jump_usermode

; vodi task_jump_usermode(TaskRegisters* regs);

task_jump_usermode: 
    mov ebp, esp

    ; Access the strucure passed to us.

    mov esi, [ebp + 4]

    ; Push the data selector.

    push dword [esi + 44]

    ; Push the stack pointer defined by the user task.

    push dword [esi + 40]

    ; Push the flags.

    pushfd

    ; We need the interrupts to be enabled in usermode even if it's disabled in kernel mode.

    pop eax

    ; Set the 10th bit to enable the interrupts.

    or eax, 0x200

    push eax

    ; Push the instruction pointer of the task.

    push dword [edi + 36]

    ; Set all the general purpose and segment registers to get back to the
    ; previous state of the task if it exists.

    push dword [ebp + 4]

    call task_restore_gpr

    add esp, 4

    ; Now let's go to the usermode!

    iretd

; void task_restore_gpr(TaskRegisters* regs);

task_restore_gpr: 
    ; No need to create any frame using ebp, cause it's going to be chagne anyway.

    mov ebp, esp

    mov edi, [ebp + 4]
    mov eax, [edi]
    mov ecx, [edi + 4]
    mov edx, [edi + 8]
    mov ebx, [edi + 12]
    mov ebp, [edi + 20]
    mov esi, [edi + 24]

    ; The segment registers.

    mov ds, [edi + 44]
    mov ss, [edi + 48]
    mov es, [edi + 52]

    mov edi, [edi + 28]

    ret