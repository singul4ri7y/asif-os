#ifndef __NUTTLE_FILE_H__
#define __NUTTLE_FILE_H__

#include <nuttle/disk/disk.h>

typedef struct __struct_NuttleFileDescriptor {
    int index;
    NuttleDisk* disk;
    void* data;        // Private data only readable by the filesystem driver.
} NuttleFileDescriptor;

// This function is not entirely necessary, it will just zero out the file descriptors
// chunk of the kernel.

void file_io_init();
int file_new_descriptor(NuttleFileDescriptor** desc_out);
NuttleFileDescriptor* file_get_fd(int fd);

#endif    // __NUTTLE_FILE_H__