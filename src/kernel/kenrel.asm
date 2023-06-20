[BITS 32]

global cpu_halt

cpu_halt: 
    hlt
    ret        ; If the CPU don't halt for some reason, this instruction will
               ; execute, and the caller knows what to do with it next.