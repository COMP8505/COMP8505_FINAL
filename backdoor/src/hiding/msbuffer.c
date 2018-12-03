#include "includes/msbuffer.h"

msbuffer *create_msbuffer() {
    msbuffer *msbuf = (msbuffer *)malloc(sizeof(msbuffer));
    msbuf->fd = open_memstream(&(msbuf->buffer), &(msbuf->size));
    if (msbuf->fd != NULL) {
        return msbuf;
    }
    return NULL;
}

int fprintf_msbuffer(msbuffer *msbuf, const char *fmt, ...) {
    va_list args;
    int n;
    va_start(args, fmt);
    if (!(n = vfprintf(msbuf->fd, fmt, args))) {
        perror("Error in sprintf_msbuffer fprintf call");
        return -1;
    }
    fflush(msbuf->fd);
#ifndef NDEBUG
    printf("Buffer->%s:%ld\n", msbuf->buffer, msbuf->size);
#endif
    return n;
}

int strcat_msbuffer(msbuffer *msbuf, char *string) {
    int n = fprintf(msbuf->fd, "%s", string);
    fflush(msbuf->fd);
    return n;
}