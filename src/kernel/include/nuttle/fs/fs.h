#ifndef __NUTTLE_FS_H__
#define __NUTTLE_FS_H__

#include <nuttle/fs/pparser.h>
#include <kerndef.h>

// Forward declaration of disk.

typedef struct __struct_NuttleDisk NuttleDisk;

typedef uint8_t FileMode;

#define FILE_MODE_READ    1
#define FILE_MODE_WRITE   2
#define FILE_MODE_APPEND  4
#define FILE_MODE_INVALID 8

typedef uint8_t FileSeekMode;

#define FILE_SEEK_MODE_SET  1
#define FILE_SEEK_MODE_CUR  2
#define FILE_SEEK_MODE_END  4

typedef uint8_t FileStatFlags;

#define FILE_STAT_FLAG_READ_ONLY  1

typedef struct __struct_NuttleFileStat {
    FileStatFlags flags;
    uint32_t file_size;
} NuttleFileStat;

typedef int   (*FsResolveFn)(NuttleDisk* disk);
typedef int   (*FsReadFn)(NuttleDisk* disk, void* private_desc, uint8_t size, size_t nmemb, void* out);
typedef int   (*FsSeekFn)(void* private_desc, long offset, FileSeekMode whence);
typedef int   (*FsStatFn)(void* private_desc, NuttleFileStat* stat);
typedef int   (*FsCloseFn)(void* private_desc);
typedef long  (*FsTellFn)(void* private_desc);
typedef void* (*FsOpenFn)(NuttleDisk* disk, PathPart* part, FileMode mode);

// Represents a filesystem.

typedef struct __struct_NuttleFs {
    FsResolveFn resolve;
    FsReadFn    read;
    FsSeekFn    seek;
    FsStatFn    stat;
    FsOpenFn    open;
    FsCloseFn   close;
    FsTellFn    tell;

    char fs_name[16];
} NuttleFs;

void fs_init();

// To implement a new filesystem.

void fs_insert_filesystem(NuttleFs* fs);

// Find the suitable filesystem for a disk.

int fs_fetch_disk_fs(NuttleDisk* disk);

#endif    // __NUTTLE_FS_H__