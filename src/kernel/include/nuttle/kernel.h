#ifndef __NUTTLE_KERNEL_H__
#define __NUTTLE_KERNEL_H__

void kernel_main();
void kernel_panic(const char* msg);
void kernel_page();

// Assembly defined external functions.

extern void cpu_halt();
extern void restore_kernel_segment_registers();

#endif    // __NUTTLE_KERNEL_H__