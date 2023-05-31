#ifndef __NUTTLE_FS_H__
#define __NUTTLE_FS_H__

#include <nuttle/fs/pparser.h>

// Forward declaration of disk.

typedef struct __struct_NuttleDisk NuttleDisk;

typedef enum __enum_FileMode {
    FILE_MODE_READ,
    FILE_MODE_WRITE,
    FILE_MODE_APPEND,
    FILE_MODE_INVALID
} FileMode;

typedef int (*FsResolveFn)(NuttleDisk* disk);
typedef void* (*FsOpenFn)(NuttleDisk* disk, PathPart* part, FileMode mode);

// Represents a filesystem.

typedef struct __struct_NuttleFs {
    FsResolveFn resolve;
    FsOpenFn open;

    char fs_name[16];
} NuttleFs;

void fs_init();

// To implement a new filesystem.

void fs_insert_filsystem(NuttleFs* fs);

// Find the suitable filesystem for a disk.

NuttleFs* fs_fetch_disk_fs(NuttleDisk* disk);

#endif    // __NUTTLE_FS_H__