#ifndef __NUTTLE_TSS_H__
#define __NUTTLE_TSS_H__

#include <kernint.h>

// Definition of the structure of Task State Segment.

typedef struct __struct_NuttleTSS {
    uint32_t link;    // Contains the previous Segment Selector of the previous task.
    uint32_t esp0;    // Kernel stack pointer.
    uint32_t ss0;     // Kernel stack segment.
    uint32_t esp1;
    uint32_t ss1;
    uint32_t esp2;
    uint32_t ss2;
    uint32_t cr3;
    uint32_t eip;
    uint32_t eflags;  // Extended flags.

    // General purpose registers.

    uint32_t eax, ecx, edx, ebx, esp, epb, esi, edi;

    // 16-bit segment registers.

    uint32_t es, cs, ss, ds, fs, gs;

    uint32_t ldtr;    // No idea what it is.

    uint32_t iobp;    // I/O map base address field.
    uint32_t ssp;     // Shadow stack pointer.

} NuttleTSS;

extern void tss_load(int segment);

void tss_init(NuttleTSS* tss);

#endif    // __NUTTLE_TSS_H__