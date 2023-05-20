#ifndef __NUTTLE_PATHPARSER_H__
#define __NUTTLE_PATHPARSER_H__

#include <kernint.h>

typedef struct __struct_PathPart PathPart;

struct __struct_PathPart {
    const char* part;
    PathPart* next;
};

typedef struct __struct_NuttlePath {
    uint8_t drive_no;
    PathPart* parts;
} NuttlePath;

int pparser_parse_path(const char* filepath, NuttlePath** nuttle_path);
void pparser_free(NuttlePath* path);

#endif    // __NUTTLE_PATHPARSER_H__