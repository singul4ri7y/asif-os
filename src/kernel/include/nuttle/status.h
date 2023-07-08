#ifndef __NUTTLE_STATUS_H__
#define __NUTTLE_STATUS_H__

#define NUTTLE_ALL_OK     0        // Self explanatory.
#define EIO               1        // I/O error.
#define EINVARG           2        // Invalid argument.
#define ENOMEM            3        // System is out of memory.
#define EBADPATH          4        // Filepath is not valid.
#define EINVIDX           5        // Invalid index provided.
#define ENOFS             6        // No filesystem binded to a disk.
#define EFSNOTUS          7        // Not valid filesystem. Return if a filesystem cannot resolve a disk.
#define ERDONLY           8        // Readonly filesystem.
#define EOPENDIR          9        // Trying to open a directory.
#define ENOTASK          10        // No current task.
#define ETAKEN           11        // An entry is taken.

#endif    // __NUTTLE_STATUS_H__