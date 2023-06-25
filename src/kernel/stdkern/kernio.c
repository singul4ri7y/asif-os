#include <kernio.h>
#include <nuttle/tty.h>
#include <kernstr.h>

void putsk(const char* str) {
    while(*str) 
        tty_putc(*str++);
}

static FileMode fetch_mode(const char* mode) {
    FileMode fmode = FILE_MODE_INVALID;
    
    if(!strcmpk(mode, "r")) {
        fmode = FILE_MODE_READ;

        goto out;
    }
    
    if(!strcmpk(mode, "w")) {
        fmode = FILE_MODE_WRITE;

        goto out;
    }

    if(!strcmpk(mode, "a")) {
        fmode = FILE_MODE_APPEND;

        goto out;
    }
    
out: 
    return fmode;
}

int fopenk(const char* filename, const char* mode) {
    return file_open(filename, fetch_mode(mode));
}

int fclosek(int fd) {
    return file_close(fd);
}

int freadk(void* buf, uint8_t size, size_t nmemb, int fd) {
    return file_read(buf, size, nmemb, fd);
}

int fseekk(int fd, long offset, int whence) {
    return file_seek(fd, offset, (FileSeekMode) whence);
}

int ftellk(int fd) {
    return file_tell(fd);
}

int fvalidk(int fd) {
    return file_is_valid(fd);
}