#ifndef __NUTTLE_KERNIO_H__
#define __NUTTLE_KERNIO_H__

#include <nuttle/tty.h>
#include <nuttle/fs/file.h>

#define SEEK_CUR   FILE_SEEK_MODE_CUR
#define SEEK_END   FILE_SEEK_MODE_END
#define SEEK_SET   FILE_SEEK_MODE_SET

void putsk(const char* str);

// File manipulation specific functions.

int fopenk(const char* filename, const char* mode);
int fclosek(int fd);
int freadk(void* buff, uint8_t size, size_t nmemb, int fd);
int fseekk(int fd, long offset, int whence);
int ftellk(int fd);
int fvalidk(int fd);

#endif    // __NUTTLE_KERNIO_H__