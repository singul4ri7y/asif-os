#include <nuttle/io.h>
#include <nuttle/status.h>
#include <nuttle/disk/disk.h>
#include <nuttle/config.h>
#include <nuttle/kernel.h>
#include <kernmem.h>
#include <kernio.h>

// We are going to use a single primary disk for now.

static NuttleDisk p_disk;

// I am using an 48-bit LBA scheme. 28-bit LBA scheme is OK, but it will only allow you to access
// storage up to 12 GiB. I just wanted to use the latest ATA standard (ATA-6, 2003) to read more storage.
// Maybe I will not need to access that amount of storage with the kernel I am making right now, But I think
// I should be as sophisticated as possible :).

static int disk_read_sectors(long long lba, uint16_t sector_count, void* buffer) {
    // 0x40 is for master drive, 0x50 for the slave drive. I hate that naming scheme.

    // In 48-bit LBA mode, just send the drive selection byte in port 0x1f6. No need to send 
    // any part of LBA with it, as 48 is divisible by 8 without any remainder.

    // I will only work with the first drive (master), for now.

    outb(0x1f6, 0x40);

    // Send the sector count high bytes to port 0x1f2.

    outb(0x1f2, (sector_count >> 8) & 0xff);

    // Send 4th, 5th and 6th byte of LBA, as single byte to port 0x1f3, 0x1f4, 0x1f5 respectively.

    outb(0x1f3, (lba >> 24) & 0xff);
    outb(0x1f4, (lba >> 32) & 0xff);
    outb(0x1f5, (lba >> 40) & 0xff);

    // Now, the port 0x1f2 is ready to take in the lower sector count byte.

    outb(0x1f2, sector_count & 0xff);

    // Send the 1st, 2nd and 3rd byte of LBA to ports same way as before.

    outb(0x1f3, lba & 0xff);
    outb(0x1f4, (lba >> 8) & 0xff);
    outb(0x1f5, (lba >> 16) & 0xff);

    // 0x1f7 is the command port for ATA controllers. Send 0x20 for reading in 28-bit LBA mode and 0x24 48-bit LBA mode.

    outb(0x1f7, 0x24);        // Read extended with retry.

    uint16_t* ptr = (uint16_t*) buffer;

    // We are gonna read 'sector_count' sectors.

    for(uint16_t i = 0; i < sector_count; i++) {
        // The response command for port 0x1f7 is if the disk is ready to read a sector. If it is, the 4th bit of the byte will be set.

        uint8_t response = inb(0x1f7);

        while(!(response & 0x8)) response = inb(0x1f7);

        // Now read the sector. Read 256 words in total.
        // Port to read word from is 0x1f0.

        for(int j = 0; j < 256; j++) 
            *ptr++ = inw(0x1f0);
    }

    return NUTTLE_ALL_OK;
}

// I am just creating a very simple, silly abstraction of disks, which I will expand 
// if need be.

// Initializes all the disks our kernel could find.

void disk_all_init() {
    memsetk(&p_disk, 0, sizeof(p_disk));

    p_disk.type        = NUTTLE_DISK_TYPE_REAL;
    p_disk.sector_size = NUTTLE_DEFAULT_DISK_SECTOR_SIZE;
    p_disk.fs          = nullptr;
    p_disk.fs_private  = nullptr;
    p_disk.id          = 0;

    // Fetch the suitable filesystem.

    if(ISERR(fs_fetch_disk_fs(&p_disk))) 
        kernel_panic("Could not resolve a suitable filesystem for disk!");
}

// Get's a specific disk from index.

NuttleDisk* disk_get(int idx) {
    if(idx < 0) 
        return 0;
    
    return &p_disk;
}

// Reads blocks from disk specified. For now the block size is hardcoded 512 bytes.

int disk_read_block(NuttleDisk* disk, long long lba, uint16_t sector_count, void* buffer) {
    int res = NUTTLE_ALL_OK;

    if(disk != &p_disk) {
        res = -EINVARG;

        goto out;
    }

    res = disk_read_sectors(lba, sector_count, buffer);

out: 
    return res;
}

// Get's the associated filesystem.

NuttleFs* disk_get_fs(NuttleDisk* disk) {
    return disk -> fs;
}