#ifndef __NUTTLE_DISK_H__
#define __NUTTLE_DISK_H__

#include <kernint.h>

typedef enum __enum_NuttleDiskType {
    NUTTLE_DISK_TYPE_REAL
} NuttleDiskType;

typedef struct __struct_NuttleDisk {
    NuttleDiskType type;
    uint16_t sector_size;
} NuttleDisk;

void disk_all_init();
NuttleDisk* disk_get(int idx);
int disk_read_block(NuttleDisk* disk, long long lba, uint16_t sector_count, void* buffer);

#endif    // __NUTTLE_DISK_H__