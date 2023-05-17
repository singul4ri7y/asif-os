#ifndef __NUTTLE_DISK_H__
#define __NUTTLE_DISK_H__

#include <kernint.h>

int disk_read_sectors(long long lba, uint16_t sector_count, void* buffer);

#endif    // __NUTTLE_DISK_H__