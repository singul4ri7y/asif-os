#ifndef __NUTTLE_KERNEL_H__
#define __NUTTLE_KERNEL_H__

void kernel_main();
void kernel_panic(const char* msg);
extern void cpu_halt();

#endif    // __NUTTLE_KERNEL_H__