#include <nuttle/fs/fat16.h>
#include <nuttle/status.h>
#include <nuttle/config.h>
#include <kernstr.h>
#include <kernmem.h>

int   fat16_resolve(NuttleDisk* disk);
void* fat16_open(NuttleDisk* disk, PathPart* part, FileMode mode);
int   fat16_read(NuttleDisk* disk, void* private_desc, uint8_t size, size_t nmemb, void* out);
int   fat16_seek(void* private_desc, long offset, FileSeekMode whence);
int   fat16_stat(void* private_data, NuttleFileStat* stat);
int   fat16_close(void* private_data);
long  fat16_tell(void* private_data);

static NuttleFs fat16_fs = {
    .resolve = fat16_resolve,
    .open    = fat16_open,
    .read    = fat16_read,
    .seek    = fat16_seek,
    .stat    = fat16_stat,
    .close   = fat16_close,
    .tell    = fat16_tell,
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

    fat_private -> fat_table_stream   = diskstream_new(disk -> id);
    fat_private -> fat_data_stream    = diskstream_new(disk -> id);
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

    if(ISERR(res = diskstream_seek(stream, root_directory_sector_pos * disk -> sector_size))) 
        goto out;

    if(ISERR(diskstream_read(stream, item.directory, root_directory_size))) {
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

    item.start_sector_pos = root_directory_sector_pos;
    item.end_sector_pos   = root_directory_sector_pos + (root_directory_size / disk -> sector_size);

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
    
    if(ISERR(diskstream_read(stream, &fat_private -> header, sizeof(NuttleFAT16Header)))) {
        res = -EIO;

        goto out;
    }
    
    // Check for FAT16 signature in the extended header.
    // If the signature is 0x28 or 0x29, we are good to go.

    if(fat_private -> header.shared.signature != 0x28 && fat_private -> header.shared.signature != 0x29) {
        res = -EFSNOTUS;

        goto out;
    }

    if(ISERR(fat16_get_root_directory(disk, fat_private))) {
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

static void fat16_get_plain_item_name(char* out, const char* filename, const char* extension) {
    while(*filename != 0 && *filename != ' ') 
        *out++ = *filename++;
    
    if(*extension != 0 && *extension != ' ') 
        *out++ = '.';
    
    while(*extension != 0 && *extension != ' ') 
        *out++ = *extension++;
    
    *out = 0;
}

static uint32_t fat16_cluster_to_sector(uint32_t root_directory_sector_end, uint8_t sector_per_cluster, uint16_t cluster_number) {
    // First 2 entry of the FAT table are reserved.

    return root_directory_sector_end + (cluster_number - 2) * sector_per_cluster;
}

static int fat16_get_cluster_number(NuttleFATDirectory* directory) {
    // In FAT12 and FAT16, the low 16 bit field represents the cluster number.
    
    return directory -> low_16_bits_of_cluster_number;
}

static uint16_t fat16_get_next_cluster(NuttleFATPrivate* private, uint16_t cluster) {
    uint16_t res = 0;

    NuttleDiskStream* stream = private -> fat_table_stream;

    if(ISERR(diskstream_seek(stream, private -> header.reserved_sectors * private -> header.bytes_per_sector + cluster * NUTTLE_FAT16_TABLE_ENTRY_SIZE))) {
        res = 0xffff;

        goto out;
    }

    if(ISERR(diskstream_read(stream, &res, 2))) {
        res = 0xffff;

        goto out;
    }

    // Is the cluster final cluster?

    if(res >= 0xfff8) {
        res = 0xffff;

        goto out;
    }

    // Is the cluster bad?

    if(res == 0xfff7) {
        res = 0xffff;

        goto out;
    }

    // Is the cluster reserved?

    if(res >= 0xfff0 && res <= 0xfff6) {
        res = 0xffff;

        goto out;
    }

    // Pointing to an empty cluster??

    if(res == 0x00) {
        res = 0xffff;

        goto out;
    }

    // All good. Return.

out:
    return res;
}

// This function reads a sector at a time. Use this function to read a single sectors from a single cluster, 
// eventually switching the cluster.

// Note: This function expects you to already set out the disk stream to the position you want to read from.

static int fat16_read_internals(NuttleFATPrivate* private, NuttleDiskStream* stream, int cluster_size_in_bytes, 
int directory_end_sector_pos, int sector_per_cluster, int bytes_per_sector, uint16_t* cluster, int* count, void* out, int* read) {
    int res = 1;

    int bytes_to_read = *count + 512 > cluster_size_in_bytes ? cluster_size_in_bytes - *count : 512;

    if(ISERR(diskstream_read(stream, out, bytes_to_read))) {
        res = 0;

        goto out;
    }

    (*count) += bytes_to_read;

    if(read) *read = bytes_to_read;

    if(*count == cluster_size_in_bytes) {
        *cluster = fat16_get_next_cluster(private, *cluster);

        if(*cluster == 0xffff) {
            res = 0;

            goto out;
        }

        if(ISERR(diskstream_seek(stream, fat16_cluster_to_sector(directory_end_sector_pos, sector_per_cluster, *cluster) * bytes_per_sector))) {
            res = 0;

            goto out;
        }

        *count = 0;
    }

out: 
    return res;
}

static int fat16_get_total_number_of_directory_entries(NuttleFATPrivate* fat_private, uint16_t cluster) {
    int res = 0;

    NuttleDiskStream* stream = fat_private -> fat_data_stream;

    if(ISERR(diskstream_seek(stream, fat16_cluster_to_sector(fat_private -> root_directory.end_sector_pos, 
    fat_private -> header.sectors_per_cluster, cluster) * fat_private -> header.bytes_per_sector))) {
        res = -1;

        goto out;
    }

    // A single FAT directory entry consists of size 32 byte. So, total of 16 directory entry will 
    // fit in a single sector.

    NuttleFATDirectory dir[32];

    int cluster_size_in_bytes = fat_private -> header.sectors_per_cluster * fat_private -> header.bytes_per_sector;
    int count = 0;

    while(fat16_read_internals(fat_private, stream, cluster_size_in_bytes, fat_private -> root_directory.end_sector_pos, 
    fat_private -> header.sectors_per_cluster, fat_private -> header.bytes_per_sector, &cluster, &count, dir, nullptr)) {
        for(int i = 0; i < 32; i++) {
            // If the first character in the filename in a directory entry is
            // 0, then then we reached the end.

            if(dir[i].filename[0] == 0x00) 
                break;
            
            // If the first character is 0xe5, then the directory is not occupied,
            // but there can be more directory entries later. So, just ignore the 
            // entry.

            if(dir[i].filename[0] == 0xe5) 
                continue;
            
            res++;
        }
    }

out: 
    return res;
}

static int fat16_resolve_directories(NuttleFATPrivate* fat_private, uint16_t cluster, NuttleFATDirectory** directory, int* total) {
    int res = NUTTLE_ALL_OK;

    // Get the total number of directory entries.

    *total = fat16_get_total_number_of_directory_entries(fat_private, cluster);

    // Now allocate the directory array.

    *directory = (NuttleFATDirectory*) mallock(*total * sizeof(NuttleFATDirectory));

    if(ISERRP(*directory)) {
        res = -ENOMEM;

        goto out;
    }

    // First find total number of directory entries.

    // The idea is to read a single sector at a time, until we reach our cluster end. 
    // Then read the next cluster and do the same, until we found a empty directory.

    NuttleDiskStream* stream = fat_private -> fat_data_stream;

    if(ISERR(res = diskstream_seek(stream, fat16_cluster_to_sector(fat_private -> root_directory.end_sector_pos, 
    fat_private -> header.sectors_per_cluster, cluster) * fat_private -> header.bytes_per_sector))) 
        goto out;
    
    // A single FAT directory entry consists of size 32 byte. So, total of 16 directory entry will 
    // fit in a single sector.

    NuttleFATDirectory dir[32];

    int cluster_size_in_bytes = fat_private -> header.sectors_per_cluster * fat_private -> header.bytes_per_sector;
    int count = 0, dir_entry = 0;

    while(fat16_read_internals(fat_private, stream, cluster_size_in_bytes, fat_private -> root_directory.end_sector_pos, 
    fat_private -> header.sectors_per_cluster, fat_private -> header.bytes_per_sector, &cluster, &count, dir, nullptr)) {
        for(int i = 0; i < 32; i++) {
            // If the first character in the filename in a directory entry is
            // 0, then then we reached the end.

            if(dir[i].filename[0] == 0x00) 
                break;
            
            // If the first character is 0xe5, then the directory is not occupied,
            // but there can be more directory entries later. So, just ignore the 
            // entry.

            if(dir[i].filename[0] == 0xe5) 
                continue;
            
            (*directory)[dir_entry++] = dir[i];
        }
    }

out: 
    return res;
}

static int fat16_resolve_subdir(NuttleFATPrivate* fat_private, NuttleFATItem* item, NuttleFATDirectory* directory) {
    int res = NUTTLE_ALL_OK;

    // If the given entry is not a subdirectory.

    if(!(directory -> attributes & FAT_FILE_DIRECTORY)) {
        res = -EINVARG;

        goto out;
    }

    if(ISERR(res = fat16_resolve_directories(fat_private, fat16_get_cluster_number(directory), &item -> directory, &item -> total))) 
        goto out;

out: 
    return res;
}

static NuttleFATItem* fat16_directory_to_item(NuttleFATPrivate* fat_private, NuttleFATDirectory* directory) {
    if(directory == 0) 
        return 0;
    
    int res = NUTTLE_ALL_OK;
    
    NuttleFATItem* item = mallock(sizeof(NuttleFATItem));

    if(!item) {
        item = ERROR_P(-ENOMEM);

        goto out;
    }

    if(directory -> attributes & FAT_FILE_DIRECTORY) {
        item -> type = FAT_ITEM_DIRECTORY;

        res = fat16_resolve_subdir(fat_private, item, directory);

        goto out;
    }

    item -> type      = FAT_ITEM_FILE;
    item -> directory = mallock(sizeof(NuttleFATDirectory));

    if(!item -> directory) {
        item = ERROR_P(-ENOMEM);

        goto out;
    }

    memcpyk(item -> directory, directory, sizeof(NuttleFATDirectory));

out: 
    if(ISERR(res)) 
        freek(item);

    return item;
}

static NuttleFATItem* fat16_find_item_in_directory(NuttleFATPrivate* fat_private, NuttleFATDirectory* items, PathPart* part) {
    NuttleFATDirectory* directory = 0;

    char tmp_name[NUTTLE_MAX_PATH_SIZE];

    int i = 0;

    while(1) {
        if(items[i].filename[0] == 0x00) 
            break;
        
        if(items[i].filename[0] == 0xe5) 
            continue;

        fat16_get_plain_item_name(tmp_name, (const char*) items[i].filename, (const char*) items[i].extension);

        if(strcmpk(tmp_name, part -> part) == 0) {
            directory = items + i;

            break;
        }

        i++;
    }

    return fat16_directory_to_item(fat_private, directory);
}

static void fat16_free_item(NuttleFATItem* item) {
    if(item -> directory) 
        freek(item -> directory);

    freek(item);
}

static NuttleFATItem* fat16_get_directory_item(NuttleFATPrivate* fat_private, PathPart* part) {
    NuttleFATItem* current_item = nullptr;

    // Consider this: 
    //     0:/bin/shell.bin
    // We have already extracted the disk number from the filename string.
    // Now the 'bin' part is the a subdirectory residing in the root directory.
    // Now figure out whether that entry in the root directory exist.

    current_item = fat16_find_item_in_directory(fat_private, fat_private -> root_directory.directory, part);

    if(!current_item) {
        current_item = ERROR_P(-EIO);

        goto out;
    }

    part = part -> next;

    while(part) {
        // If we try to open a file inside a subdirectory, and turns out the 
        // subdirectory itself is a file, then we won't be able to open that file.

        if(current_item -> type != FAT_ITEM_DIRECTORY) {
            current_item = ERROR_P(-EIO);

            goto out;
        }

        NuttleFATItem* tmp = fat16_find_item_in_directory(fat_private, current_item -> directory, part);
        
        // Free current item, we no longer need it.

        fat16_free_item(current_item);

        current_item = tmp;

        part = part -> next;
    }

out: 
    return current_item;
}

void* fat16_open(NuttleDisk* disk, PathPart* part, FileMode mode) {
    int res = 0;

    if(mode & FILE_MODE_APPEND || mode & FILE_MODE_WRITE) {
        res = -ERDONLY;

        goto out;
    }
    
    NuttleFATFileDescriptor* desc = mallock(sizeof(NuttleFATFileDescriptor));

    if(!desc) {
        res = -ENOMEM;

        goto out;
    }

    NuttleFATPrivate* fat_private = disk -> fs_private;

    desc -> item = fat16_get_directory_item(fat_private, part);

    if(ISERR(desc -> item)) {
        res = ERROR_I(desc -> item);

        goto out;
    }

    if(desc -> item -> type == FAT_ITEM_DIRECTORY) {
        res = -EOPENDIR;

        goto out;
    }

    desc -> pos  = 0;

out: 
    if(ISERR(res) && desc) {
        if(desc -> item) 
            fat16_free_item(desc -> item);
        
        freek(desc);
    }
    
    return !res ? desc : ERROR_P(res);
}

int fat16_read(NuttleDisk* disk, void* private_desc, uint8_t size, size_t nmemb, void* out) {
    int res = 0;

    int total = nmemb * size;

    NuttleFATPrivate* private     = disk -> fs_private;
    NuttleFATFileDescriptor* desc = private_desc;

    if(desc -> pos + total > desc -> item -> directory -> file_size) 
        total = desc -> item -> directory -> file_size - desc -> pos;

    NuttleDiskStream* stream = private -> fat_data_stream;

    uint16_t cluster = fat16_get_cluster_number(desc -> item -> directory);

    int bps           = private -> header.bytes_per_sector;
    int sector_per_c  = private -> header.sectors_per_cluster;
    int cluster_bytes = sector_per_c * bps;
    int sectors       = total / bps + (total % bps ? 1 : 0);
    
    int count = desc -> pos, read;

    char* outptr = (char*) out;
    
    char buf[512];

    int tmp_total = total;

    // Now set the stream to the very first cluster position.

    if(ISERR(res = diskstream_seek(stream, fat16_cluster_to_sector(private -> root_directory.end_sector_pos, sector_per_c, cluster) * bps + desc -> pos))) 
        goto out;

    while(sectors--) {
        if(!fat16_read_internals(private, stream, cluster_bytes, private -> root_directory.end_sector_pos, sector_per_c, bps, &cluster, &count, buf, &read)) 
            goto out;
        
        if(tmp_total < read) {
            memcpyk(outptr, buf, tmp_total * sizeof(char));

            desc -> pos += tmp_total;

            break;
        }

        memcpyk(outptr, buf, read * sizeof(char));

        tmp_total -= read;
        outptr    += read;

        desc -> pos += read;
    }

    res = total / size;

out: 
    return res;
}

int fat16_seek(void* private_desc, long offset, FileSeekMode whence) {
    int res = NUTTLE_ALL_OK;

    NuttleFATFileDescriptor* desc = private_desc;

    long file_size = desc -> item -> directory -> file_size;

    switch(whence) {
        case FILE_SEEK_MODE_CUR: {
            if(offset + (long) desc -> pos >= file_size) {
                res = -EIO;

                goto out;
            }

            desc -> pos += offset;
            
            break;
        }

        case FILE_SEEK_MODE_SET: {
            if(offset >= file_size) {
                res = -EIO;

                goto out;
            }

            desc -> pos = offset;

            break;
        }

        case FILE_SEEK_MODE_END: {
            // When using seek end, use negative offset value to move the cursor in 
            // backward direction.

            if(offset > 0) {
                res = -EIO;

                goto out;
            }

            if(-offset > file_size) {
                res = -EIO;

                goto out;
            }

            desc -> pos = file_size + offset;

            break;
        }

        default: {
            res = -EINVARG;

            goto out;
        }
    }

out: 
    return res;
}

int fat16_stat(void* private_data, NuttleFileStat* stat) {
    int res = NUTTLE_ALL_OK;

    if(ISERRP(stat)) {
        res = -EINVARG;

        goto out;
    }

    NuttleFATFileDescriptor* desc = private_data;

    stat -> file_size = desc -> item -> directory -> file_size;

    stat -> flags = 0;

    if(desc -> item -> directory -> attributes & FAT_FILE_READONLY) 
        stat -> flags |= FILE_STAT_FLAG_READ_ONLY;

out: 
    return res;
}

int fat16_close(void* private_data) {
    int res = NUTTLE_ALL_OK;

    // Free the descriptor item.

    NuttleFATFileDescriptor* desc = private_data;

    if(desc -> item) 
        fat16_free_item(desc -> item);
    
    // Now free the descriptor.

    freek(desc);

    return res;
}

long fat16_tell(void* private_data) {
    NuttleFATFileDescriptor* desc = private_data;

    return (long) desc -> pos;
}