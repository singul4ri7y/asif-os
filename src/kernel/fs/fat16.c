#include <nuttle/fs/fat16.h>
#include <nuttle/status.h>
#include <kernmem.h>

int   fat16_resolve(NuttleDisk* disk);
void* fat16_open(NuttleDisk* disk, PathPart* part, FileMode mode);

static NuttleFs fat16_fs = {
    .resolve = fat16_resolve,
    .open    = fat16_open,
    .fs_name = "FAT16"
};

static int fs_fat16_init = 0;

void fat16_init() {
    if(!fs_fat16_init) {
        fs_insert_filesystem(&fat16_fs);

        fs_fat16_init = 1;
    }
}

static void fat16_init_private(NuttleDisk* disk, NuttleFATPrivate* fat_private) {
    memsetk(fat_private, 0, sizeof(NuttleFATPrivate));

    fat_private -> fat_cluster_stream   = diskstream_new(disk -> id);
    fat_private -> fat_read_stream      = diskstream_new(disk -> id);
}

static int fat16_get_root_directory(NuttleDisk* disk, NuttleFATPrivate* fat_private) {
    int res = NUTTLE_ALL_OK;

    NuttleFAT16Header* header = &fat_private -> header;

    int root_directory_sector_pos = (header -> fat_tables * header -> sectors_per_fat_table) + header -> reserved_sectors;
    int root_directory_size       = (int) header -> directory_entries * sizeof(NuttleFATDirectory);

    int root_directory_total_sectors = root_directory_size / disk -> sector_size;

    if(root_directory_size % disk -> sector_size) 
        root_directory_total_sectors++;
    
    
    NuttleFATItem item;

    item.directory = (NuttleFATDirectory*) mallock(root_directory_size);

    if(!item.directory) {
        res = -ENOMEM;

        goto out;
    }

    NuttleDiskStream* stream = diskstream_new(disk -> id);

    // Seek to the absolute position where the root directory resides.

    if((res = diskstream_seek(stream, root_directory_sector_pos * disk -> sector_size)) != NUTTLE_ALL_OK) 
        goto out;

    if(diskstream_read(stream, item.directory, root_directory_size) != NUTTLE_ALL_OK) {
        res = -EIO;

        goto out;
    }

    // Now calculate total number of items in the root directory.

    item.total = 0;

    for(uint16_t i = 0; i < header -> directory_entries; i++) {
        // If the first byte of the filename is 0x00, that means the entry is free and all the entries
        // following the entry is also free. So, we can stop counting.

        if(item.directory[i].filename[0] == 0x00) break;

        // If the first byte of the filename is 0xe5, that indicates the entry is free (available), but entries after
        // that can be occupied or free. So, we simply ignore the entry.

        // This happens, say when a file is deleted and an entry becomes free in in middle of other entries.

        if(item.directory[i].filename[0] == 0xe5) 
            continue;
        
        item.total++;
    }

    item.start_cluster_number = root_directory_sector_pos;
    item.end_cluster_number   = root_directory_sector_pos + (root_directory_size / disk -> sector_size);

    fat_private -> root_directory = item;
    
out: 
    if(res != NUTTLE_ALL_OK && item.directory) 
        freek(item.directory);

    if(stream) 
        diskstream_close(stream);

    return res;
}

int fat16_resolve(NuttleDisk* disk) {
    int res = 0;

    NuttleFATPrivate* fat_private = mallock(sizeof(NuttleFATPrivate));

    fat16_init_private(disk, fat_private);

    NuttleDiskStream* stream = diskstream_new(disk -> id);

    if(!stream) {
        res = -ENOMEM;

        goto out;
    }
    
    if(diskstream_read(stream, &fat_private -> header, sizeof(NuttleFAT16Header)) != NUTTLE_ALL_OK) {
        res = -EIO;

        goto out;
    }
    
    // Check for FAT16 signature in the extended header.
    // If the signature is 0x28 or 0x29, we are good to go.

    if(fat_private -> header.shared.signature != 0x28 && fat_private -> header.shared.signature != 0x29) {
        res = -EFSNOTUS;

        goto out;
    }

    if(fat16_get_root_directory(disk, fat_private) != NUTTLE_ALL_OK) {
        res = -EIO;

        goto out;
    }

    // Bind the filesystem.

    disk -> fs         = &fat16_fs;
    disk -> fs_private = (void*) fat_private;

out: 
    if(stream) 
        diskstream_close(stream);
    
    // Clear the allocated private data, if the resolve fails.

    if(res != NUTTLE_ALL_OK && fat_private) 
        freek(fat_private);

    return res;
}

void* fat16_open(NuttleDisk* disk, PathPart* part, FileMode mode) {
    disk = (NuttleDisk*) part;
    part = (PathPart*) disk;
    mode++;

    return 0;
}