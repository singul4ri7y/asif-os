#ifndef __NUTTLE_FAT16_H__
#define __NUTTLE_FAT16_H__

#include <nuttle/fs/fs.h>
#include <nuttle/disk/stream.h>
#include <kernint.h>

// FAT directory attribute fields.

typedef enum __enum_FATDirectoryAttribute {
    FAT_FILE_READONLY  = 0x01,
    FAT_FILE_HIDDEN    = 0x02,
    FAT_FILE_SYSTEM    = 0x04,
    FAT_FILE_VOLUME_ID = 0x08,
    FAT_FILE_DIRECTORY = 0x10,
    FAT_FILE_ARCHIVE   = 0x20
} FATDirectoryAttribute;

typedef enum __enum_NuttleFATItemType {
    FAT_ITEM_FILE,
    FAT_ITEM_DIRECTORY
} NuttleFATItemType;

typedef struct __struct_NuttleFAT16Header {
    uint8_t  short_jmp[3];
    uint8_t  oem_identifier[8];
    uint16_t bytes_per_sector;
    uint8_t  sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t  fat_tables;
    uint16_t directory_entries;
    uint16_t total_sectors;
    uint8_t  media_type;
    uint16_t sectors_per_fat_table;
    uint16_t sectors_per_track;
    uint16_t number_of_heads;
    uint32_t hidden_sectors;
    uint32_t large_sectors;

    struct {
        uint8_t  drive_number;
        uint8_t  reserved;
        uint8_t  signature;
        uint32_t volume_id;
        uint8_t  volume_label[11];
        uint8_t  system_identifier[8];
    } __attribute__((packed)) shared;
} __attribute__((packed)) NuttleFAT16Header;

typedef struct __struct_NuttleFATDirectory {
    uint8_t  filename[8];
    uint8_t  extension[3];
    uint8_t  attributes;
    uint8_t  reserved;
    uint8_t  creation_time_tenths_of_a_second;
    uint16_t creation_time;                           // Multiply seconds by 2.
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t high_16_bits_of_cluster_number;          // Only used in 32 bit cluster addressing.
    uint16_t last_modification_time;
    uint16_t last_modification_date;
    uint16_t low_16_bits_of_cluster_number;           // Used by FAT 12 and 16.
    uint32_t file_size;
} __attribute__((packed)) NuttleFATDirectory;

// For our internal representation of FAT items.

typedef struct __struct_NuttleFATItem {
    NuttleFATItemType type;
    NuttleFATDirectory* directory;
    int total;        // Total number of items in the directory.
    int start_sector_pos;
    int end_sector_pos;
} NuttleFATItem;

// Will be used to represent an opened file.

typedef struct __struct_NuttleFATFileDescriptor {
    NuttleFATItem* item;
    uint32_t pos;               // The position the file is seeked to.
} NuttleFATFileDescriptor;

typedef struct __struct_NuttleFATPrivate {
    NuttleFAT16Header header;
    NuttleFATItem root_directory;

    NuttleDiskStream* fat_table_stream;          // To read cluster entries.
    NuttleDiskStream* fat_data_stream;           // To read a file raw data.
} NuttleFATPrivate;

void fat16_init();

#endif    // __NUTTLE_FAT16_H__