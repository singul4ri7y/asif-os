#ifndef __NUTTLE_STREAM_H__
#define __NUTTLE_STREAM_H__

#include <nuttle/disk/disk.h>

typedef struct __struct_NuttleDiskStream {
    uint32_t pos;
    NuttleDisk* disk;
} NuttleDiskStream;

NuttleDiskStream* diskstream_new(int disk_id);
int diskstream_seek(NuttleDiskStream* stream, uint32_t pos);
int diskstream_read(NuttleDiskStream* stream, void* buf, uint32_t total);

#endif    // __NUTTLE_STREAM_H__