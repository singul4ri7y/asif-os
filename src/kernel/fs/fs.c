#include <nuttle/fs/fs.h>
#include <kernio.h>
#include <nuttle/config.h>
#include <kernmem.h>

NuttleFs* filesystems[NUTTLE_MAX_FILESYSTEMS];

static NuttleFs** fs_get_free_slot() {
    NuttleFs** fs = 0;

    for(int i = 0; i < NUTTLE_MAX_FILESYSTEMS; i++) {
        if(filesystems[i] == 0) {
            fs = filesystems + i;

            break;
        }
    }

    return fs;
}

void fs_insert_filesystem(NuttleFs* filesystem) {
    NuttleFs** fs = fs_get_free_slot();

    // If no filesystem slot is available, panic.

    if(!fs) {
        // TODO: Implement and use kernel panic.

        putsk("Could not insert the filesystem!\n");

        while(1) {}
    }

    *fs = filesystem;
}

// Load all the filesystems.

static void fs_load() {
    // fs_insert_filesystem(fat16_init());
}

void fs_init() {
    memsetk(filesystems, 0, sizeof(filesystems));

    // Now load all the filesystems.

    fs_load();
}

NuttleFs* fs_fetch_disk_fs(NuttleDisk* disk) {
    NuttleFs* fs = 0;

    for(int i = 0; i < NUTTLE_MAX_FILESYSTEMS; i++) {
        if(filesystems[i] != 0 && filesystems[i] -> resolve(disk)) {
            fs = filesystems[i];

            break;
        }
    }

    return fs;
}