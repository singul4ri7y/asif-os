#include <nuttle/disk/stream.h>
#include <nuttle/status.h>
#include <nuttle/config.h>
#include <kernmem.h>

NuttleDiskStream* diskstream_new(int disk_id) {
    NuttleDiskStream* stream = mallock(sizeof(NuttleDiskStream));

    stream -> disk = disk_get(disk_id);
    stream -> pos  = 0;

    return stream;
}

int diskstream_seek(NuttleDiskStream* stream, uint32_t pos) {
    stream -> pos = pos;

    return NUTTLE_ALL_OK;
}

int diskstream_read(NuttleDiskStream* stream, void* buf, uint32_t total) {
    uint8_t buffer[512];

    uint32_t sector = stream -> pos / NUTTLE_DEFAULT_DISK_SECTOR_SIZE;
    uint32_t offset = stream -> pos % NUTTLE_DEFAULT_DISK_SECTOR_SIZE;

    // 'total' size is number of bytes we need to read including the 'offset'
    // byte.

    uint32_t size = offset + total - 1, sectors_total = size / NUTTLE_DEFAULT_DISK_SECTOR_SIZE;

    if(size % NUTTLE_DEFAULT_DISK_SECTOR_SIZE != 0) 
        sectors_total++;

    uint8_t* ptr = (uint8_t*) buf;

    int res = NUTTLE_ALL_OK, tot = total;

    while(sectors_total--) {
        if(ISERR(res = disk_read_block(stream -> disk, sector++, 1, buffer))) 
            goto out;
        
        for(; offset < 512 && tot; offset++, tot--) 
            *ptr++ = buffer[offset];
        
        offset = 0;
    }

    // Adjust the position.

    stream -> pos += total;

out: 
    return res;
}

void diskstream_close(NuttleDiskStream* stream) {
    freek(stream);
}