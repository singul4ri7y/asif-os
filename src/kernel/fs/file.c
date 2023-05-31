#include <nuttle/fs/file.h>
#include <nuttle/config.h>
#include <nuttle/status.h>
#include <kernmem.h>

NuttleFileDescriptor* file_descriptors[NUTTLE_MAX_FILE_DESCRIPTORS];

void file_io_init() {
    memsetk(file_descriptors, 0, sizeof(file_descriptors));
}

static int file_get_free_slot() {
    int idx = -EINVIDX;

    for(int i = 0; i < NUTTLE_MAX_FILE_DESCRIPTORS; i++) {
        if(file_descriptors[i] == 0) {
            idx = i;

            break;
        }
    }

    return idx;
}

int file_new_descriptor(NuttleFileDescriptor** desc_out) {
    int slot = file_get_free_slot();

    if(slot < 0) return -ENOMEM;

    NuttleFileDescriptor* desc = (NuttleFileDescriptor*) mallock(sizeof(NuttleFileDescriptor));

    desc -> index = slot + 1;
    
    file_descriptors[slot] = desc;

    *desc_out = desc;

    return NUTTLE_ALL_OK;
}

NuttleFileDescriptor* file_get_fd(int fd) {
    if(fd < 1 && fd > NUTTLE_MAX_FILE_DESCRIPTORS) 
        return 0;
    
    return file_descriptors[fd - 1];
}