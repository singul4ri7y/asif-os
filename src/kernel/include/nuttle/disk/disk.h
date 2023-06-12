#ifndef __NUTTLE_DISK_H__
#define __NUTTLE_DISK_H__

#include <nuttle/fs/fs.h>
#include <nuttle/error.h>
#include <kernint.h>

typedef enum __enum_NuttleDiskType {
    NUTTLE_DISK_TYPE_REAL
} NuttleDiskType;

struct __struct_NuttleDisk {
    NuttleDiskType type;
    uint16_t sector_size;
    uint8_t id;
    NuttleFs* fs;
    void* fs_private;        // Private filesystem data.
};

void disk_all_init();
NuttleDisk* disk_get(int idx);
int disk_read_block(NuttleDisk* disk, long long lba, uint16_t sector_count, void* buffer);
NuttleFs* disk_get_fs(NuttleDisk* disk);

#endif    // __NUTTLE_DISK_H__