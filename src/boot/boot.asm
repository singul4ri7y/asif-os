[ORG 0x7c00]
[BITS 16]

%define KERNEL_ADDR 0x00100000

; Offset of the 32-bit protected mode code and data segments.

CODE_SEG: equ gdt_code - gdt_start
DATA_SEG: equ gdt_data - gdt_start

; BIOS Parameter Block.

; Jump to the code section, so that the processor doesn't execute the boot header,
; which is not code.
; 

jmp short start
nop

times 33 db 0

; Global Descriptor Table.
; 

gdt_start: 
gdt_null: 
    ; The first entry in GDT should always be null.

    dq 0            ; First 8 bytes.

gdt_code:           ; Descriptor for the code segment.
    dw 0xffff       ; First 16 bits of the segment limit.
    
    ; First 24 bits of base.

    dw 0x0
    db 0x0

    db 0b10011010   ; Present, Privilage, Type and Type flags.
    db 0b11001111   ; Other 4 bit flags and last 4 bit of the segment limit.
    db 0x0          ; Last 8 bits of the base.

gdt_data:           ; Descriptor for the data segment.
    dw 0xffff       ; First 16 bits of the segment limit.
    
    ; First 24 bits of base.

    dw 0x0
    db 0x0

    db 0b10010010   ; Present, Privilage, Type and Type flags.
    db 0b11001111   ; Other 4 bit flags and last 4 bit of the segment limit.
    db 0x0          ; Last 8 bits of the base.

gdt_end: 

gdt_descriptor: 
    dw gdt_end - gdt_start - 1
    dd gdt_start

; 16-bit codes.

start: 
    ; Forcefully set the code segment.

    jmp 0x00:main

main: 
    ; Set all the segment registers.

    cli                ; Clear/Disable interrupts.

    ; We don't want any interrupts while changing the segment registers, cause
    ; in some interrupt calls segment register may be used and the values we want
    ; to insert may get corrupted.

    mov ax, 0x00

    ; You can't directly read a value from the RAM and set the segment registers.
    ; The only explanation I could find is engineers has not implemented the necessary
    ; curcuits or paths to read a value from RAM to segment registers, maybe due to 
    ; security.

    mov ds, ax
    mov es, ax
    mov ss, ax

    mov si, 0x7c00     ; Set the stack pointer.

    sti                ; Set/Enable interrupts.

    ; Switch to 32-bit protected mode.

    cli                      ; Clear interrupts.

    lgdt [gdt_descriptor]    ; Load the GDT to the GDT register.

    ; Set the PE (Protection Enable) bit in CR0 (Control Register 0).

    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Perform a far jump.

    jmp CODE_SEG:start_protected_mode

[BITS 32]

start_protected_mode: 
    ; Set all the segment registers.

    mov ax, DATA_SEG

    mov ds, ax
    mov es, ax
    mov ss, ax
    mov gs, ax
    mov fs, ax

    ; We are gonna load our kernel in address 0x100000, the stack pointer may change in the future.

    mov esp, 0x00200000
    mov ebp, eax

    ; Even if we are in 32-bit protected mode, we cannot access the full 4GB of memory. 
    ; To unleash the full addressing potential of 32-bit addressing, we have to enable the 
    ; Address Bus Line 20 (A20).

    ; In 1982, the i286 (Intel 80286) processor had a 20th address line, which used to be disabled
    ; for backward compatibility with 8086 processor. Programmers had to manually enable it to access memory
    ; beyond 1MB. But nowdays, there is a port available to enable the A20 line, which can be used to enable it
    ; faster. In modern Intel CPU's enabling the A20 line allows us to access the full 32/64 bit addressing 
    ; depending on the CPU architecture.

    ; Fast A20 Gate.

    in al, 0x92            ; Read from port 0x92.

    ; Check whether the A20 gate is already enabled.
    ; If so, skip the enabling part.

    and al, 0x02
    jnz .we_are_set

    ; Else enable it.
    
    or al, 0x02            ; Set the second bit.
    out 0x92, al           ; Write to the port 0x92.

.we_are_set:
    ; Load kernel from disk.

    mov eax, 1             ; Read the second sector (0 based indexing).
    mov ecx, 100           ; Number of sectors to read.

    ; The first contiguous unbounded memory starts after 1MB, from hex 0x00100000.

    mov edi, 0x00100000    ; Memory address to load the sectors to.

    call ata_lba_read      ; Read the drive sectors holding kernel into memory.

    ; Now jump to the kernel entry point.

    jmp CODE_SEG:KERNEL_ADDR

    jmp end

; ATA read sectors from disk (LBA MODE).
; Params: 
;     EAX: Logical Block Address of the sector to start reading from.
;     ECX: Numer of sectors to read.
;     EDI: The buffer to load the sectors to in the memory.
; 
; Returns: NONE
; 

ata_lba_read: 
    ; Keep a backup of the current LBA.

    mov ebx, eax

    ; Set the first high 3 bits of LBA and send it to port drive controller.
    ; Bits 24 - 31.

    mov edx, 0x01f6        ; Port to send the high 8 bits.
    shr eax, 24            ; Right shift it 24 bits to get high 8 bits.
    
    ; Set the high 3 bits of LBA. This is to tell modern drive controllers we want to read in
    ; LBA scheme.

    or eax, 0xe0
    out dx, al             ; Write to the i/o port.

    ; Send the number of sectors to read.

    mov edx, 0x01f2        ; Port to send the number of sectors.
    mov al, cl
    out dx, al             ; Send the first 8 bits.

    ; Send the first 8 bits of LBA.
    ; Bits 0 - 7.

    mov edx, 0x01f3        ; Port to send the first 8 bits of LBA.
    mov eax, ebx           ; Restore the saved LBA.
    out dx, al

    ; Send the next next 8 bits of LBA.
    ; Bits 8 - 15

    mov edx, 0x01f4        ; Port to send the next 8 bits.
    mov eax, ebx           ; Restore the LBA. Not necessary, do this for safety purposes.
    shr eax, 8             ; Shift 8 bits to the right to get to the next 8 bits.
    out dx, al

    ; Now, send the bits 16 - 23 (8 bits) of LBA.

    mov edx, 0x01f5        ; Port to send the remaining bits.
    mov eax, ebx           ; Restore the LBA. In case the eax gets currupted while while writing to the port bus.
    shr eax, 16            ; Get bits 16 - 23 to AL.
    out dx, al

    mov edx, 0x01f7        ; Command port.
    mov eax, 0x20          ; Read with retry.
    out dx, al

    ; Reading multiple sector loop.

.next_sector: 
    push ecx

    ; Check are we ready to read.

.retry: 
    mov edx, 0x01f7
    in al, dx              ; Read from previously defined port (0x01f7).
    
    ; Check whether the 4th bit is set. If it is, the disk is ready to 
    ; perform the read operation.

    and al, 0x8
    jz .retry              ; If the bit is not set, try again.

    mov ecx, 256           ; 256 words = 1 sector.
    mov edx, 0x01f0        ; Port to read sector words from.
    rep insw               ; Read the sectors, each word at a time.

    pop ecx
    loop .next_sector      ; Try reading the next sector.

    ret

end: 
    ; Halt the CPU.
    
    hlt

; If in some case the processor core doesn't stop executing, create a infinite
; jump loop to prevent further code execution.

jmp $

times 510 - ($ - $$) db 0

dw 0xaa55
