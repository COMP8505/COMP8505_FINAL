#ifndef MSBUFFER_H
#define MSBUFFER_H
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


typedef struct msbuffer {
    char* buffer;
    FILE* fd;
    size_t size;
} msbuffer;

msbuffer* create_msbuffer();
int fprintf_msbuffer(msbuffer* msbuf, const char* fmt, ...);
int strcat_msbuffer(msbuffer *msbuf, char *string);

#endif