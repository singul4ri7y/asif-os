#include <nuttle/task/tss.h>
#include <nuttle/config.h>
#include <kernmem.h>

void tss_init(NuttleTSS* tss) {
    memsetk(tss, 0x00, sizeof(NuttleTSS));

    // Our kernel stack pointer starts from 0x600000, and our stack segment is our data segment (see 'boot.asm').

    tss -> esp0 = 0x600000;
    tss -> ss0  = NUTTLE_DATA_SEGEMNT_SELECTOR;
}