#include <nuttle/fs/file.h>
#include <nuttle/fs/pparser.h>
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
    int res = NUTTLE_ALL_OK;

    int slot = file_get_free_slot();

    if(ISERR(slot)) {
        res = -ENOMEM;

        goto out;
    }

    NuttleFileDescriptor* desc = (NuttleFileDescriptor*) mallock(sizeof(NuttleFileDescriptor));

    if(ISERRP(desc)) {
        res = -ENOMEM;

        goto out;
    }

    desc -> index = slot + 1;
    
    file_descriptors[slot] = desc;

    *desc_out = desc;

out: 
    return res;
}

void file_free_descriptor(NuttleFileDescriptor* desc) {
    // Free the slot.

    file_descriptors[desc -> index - 1] = 0;

    // Now free the descriptor.

    freek(desc);
}

NuttleFileDescriptor* file_get_fd(int fd) {
    if(fd < 1 && fd > NUTTLE_MAX_FILE_DESCRIPTORS) 
        return 0;
    
    return file_descriptors[fd - 1];
}

static int file_is_valid_mode(FileMode mode) {
    return !(mode & FILE_MODE_INVALID) && mode >= 1 && mode < 6;
}

int file_open(const char* filename, FileMode mode) {
    int res = 0;

    NuttlePath* path;

    if(ISERR(res = pparser_parse_path(filename, &path))) 
        goto out;

    // If the user is trying to open a drive, it's not a valid path.
    // Say if the user is trying to open a path like '0:/', it's not 
    // going to be valid.

    if(ISERRP(path -> parts)) {
        res = -EBADPATH;

        goto out;
    }

    // Ensure the disk we are reading from exist.

    NuttleDisk* disk = disk_get(0);

    if(ISERRP(disk)) {
        res = -EBADPATH;

        return res;
    }

    // Ensure if the disk has a valid filesystem.

    if(ISERRP(disk -> fs)) {
        res = -ENOFS;

        goto out;
    }

    // Ensure that the file mode is valid.

    if(!file_is_valid_mode(mode)) {
        res = -EIO;

        goto out;
    }


    void* file_private_data = disk -> fs -> open(disk, path -> parts, mode);

    if(ISERR(file_private_data)) {
        res = ERROR_I(file_private_data);

        goto out;
    }

    NuttleFileDescriptor* desc = nullptr;

    file_new_descriptor(&desc);

    desc -> disk = disk;
    desc -> data = file_private_data;

    res = desc -> index;

out: 
    pparser_free(path);

    return res;
}

int file_read(void* buf, uint8_t size, size_t nmemb, int fd) {
    int res = 0;

    // Invalid file descriptor.

    if(fd < 1) {
        res = -EINVARG;

        goto out;
    }

    NuttleFileDescriptor* descriptor = file_get_fd(fd);

    if(ISERRP(descriptor)) {
        res = -EINVARG;

        goto out;
    }

    // Read from the filesystem.

    res = descriptor -> disk -> fs -> read(descriptor -> disk, descriptor -> data, size, nmemb, buf);

out: 
    return res;
}

int file_seek(int fd, long offset, FileSeekMode whence) {
    int res = NUTTLE_ALL_OK;

    NuttleFileDescriptor* desc = file_get_fd(fd);

    if(ISERRP(desc)) {
        res = -EINVARG;

        goto out;
    }

    res = desc -> disk -> fs -> seek(desc -> data, offset, whence);

out: 
    return res;
}

int file_stat(int fd, NuttleFileStat* stat) {
    int res = NUTTLE_ALL_OK;

    NuttleFileDescriptor* desc = file_get_fd(fd);

    if(ISERRP(desc)) {
        res = -EINVARG;

        goto out;
    }

    res = desc -> disk -> fs -> stat(desc -> data, stat);

out: 
    return res;
}

int file_close(int fd) {
    int res = NUTTLE_ALL_OK;

    NuttleFileDescriptor* desc = file_get_fd(fd);

    if(ISERRP(desc)) {
        res = -EINVARG;

        goto out;
    }

    if(ISERR(res = desc -> disk -> fs -> close(desc -> data))) 
        goto out;
    
    // Free the descriptor.

    file_free_descriptor(desc);

out: 
    return res;
}

int file_tell(int fd) {
    int res = -1;

    NuttleFileDescriptor* desc = file_get_fd(fd);

    if(ISERRP(desc)) 
        goto out;
    
    res = desc -> disk -> fs -> tell(desc -> data);

out: 
    return res;
}

int file_is_valid(int fd) {
    return fd > 0 && fd <= NUTTLE_MAX_FILE_DESCRIPTORS;
}