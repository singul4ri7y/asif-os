#include <nuttle/fs/pparser.h>
#include <nuttle/config.h>
#include <nuttle/status.h>
#include <kernstr.h>
#include <kernmem.h>

static int is_digit(char ch) {
    return ch >= '0' && ch <= '9';
}

// Checks whether the provided path is valid path or not.
// I am kidna going with Windows conventional way of path definition.
// Windows path convention is, DriveLetter:/path_part1/path_part2/...
//     e.g. C:/Program Files/VS Code/...
// I am gonna use drive number instead of drive letter.
//     e.g. 0:/path/to/file

static int pparser_is_valid_path(const char* filepath) {
    // I am supporting drive numbers from 0 - 9, for now.

    return strnlenk(filepath, NUTTLE_MAX_PATH_SIZE) >= 3 && is_digit(filepath[0]) && !memcmpk((void*) filepath + 1, ":/", 2 * sizeof(char));
}

static int pparser_extract_drive_number(const char** filepath, uint8_t* drive_number) {
    int res = NUTTLE_ALL_OK;

    if(!pparser_is_valid_path(*filepath)) {
        res = -EBADPATH;

        goto out;
    }
    
    // Now extract the drive number.

    *drive_number = *filepath[0] - '0';

    // Now, change the character buffer pointer, increasing it 3 bytes, removing the drive number.
    // So, something like this, 0:/path/to/file will be something like this, path/to/file in actual
    // character pointer.

    *filepath += 3;

out: 
    return res;
}

static int pparser_extract_path_part(const char** filepath, char** container) {
    int res = NUTTLE_ALL_OK;

    if(!**filepath) {
        res = -EBADPATH;

        goto out;
    }
    
    // It's gonna create a 4096 byte block anyway.

    char* part = mallock(NUTTLE_MAX_PATH_SIZE * sizeof(char));

    if(!part) {
        res = -ENOMEM;

        goto out;
    }

    int count = 0;

    while(**filepath != '/' && **filepath != 0x0) {
        part[count++] = **filepath;

        *filepath += 1;
    }

    // Termination character.

    part[count] = '\0';

    if(**filepath == '/') 
        *filepath += 1;
    
    *container = part;

out: 
    return res;
}

static int pparser_get_path(uint8_t drive_number, PathPart* part, NuttlePath** path) {
    int res = NUTTLE_ALL_OK;

    *path = (NuttlePath*) mallock(sizeof(NuttlePath));

    if(!*path) {
        res = -ENOMEM;

        goto out;
    }

    (*path) -> drive_no = drive_number;
    (*path) -> parts    = part;

out: 
    return res;
}

static int pparser_make_path_part(PathPart* prev, const char** filepath, PathPart** path_part) {
    int res = NUTTLE_ALL_OK;

    *path_part = mallock(sizeof(PathPart));

    if(!*path_part) {
        res = -ENOMEM;

        goto out;
    }

    char* path = 0x00;

    res = pparser_extract_path_part(filepath, &path);
    
    if(res < 0) goto out;

    (*path_part) -> part = path;
    (*path_part) -> next = 0x00;

    if(prev) prev -> next = *path_part;

out: 
    return res;
}

int pparser_parse_path(const char* filepath, NuttlePath** nuttle_path) {
    const char* tmp_path = filepath;

    uint8_t drive_number;

    int res = pparser_extract_drive_number(&tmp_path, &drive_number);

    if(res < 0) return res;

    PathPart* parts = 0x00;

    res = pparser_make_path_part(NULL, &tmp_path, &parts);

    if(res < 0) return res;

    PathPart* next = parts, *tmp_part;

    while(res == NUTTLE_ALL_OK) {
        res = pparser_make_path_part(next, &tmp_path, &tmp_part);

        next = tmp_part;
    } 

    return pparser_get_path(drive_number, parts, nuttle_path);
}

void pparser_free(NuttlePath* path) {
    PathPart* next = path -> parts;

    while(next) {
        PathPart* tmp = next -> next;

        freek((void*) next -> part);
        freek(next);

        next = tmp;
    }

    freek(path);
}