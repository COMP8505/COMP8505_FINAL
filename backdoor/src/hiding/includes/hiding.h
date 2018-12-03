#ifndef HIDING_H
#define HIDING_H

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define DATA_BUFF_LEN 8192
#define LINE 128

typedef struct process_stats {
    char cmdline[DATA_BUFF_LEN];
    long PID;
    long read_bytes;
    long write_bytes;
    long rchar;
    long wchar;
    long syscw;
    long syscr;
    long cancelled_write_bytes;
} process_stats;

int compare_process_stats(const void *a, const void *b);
char *read_file(const char *path);
process_stats **allocate_proc_stats() ;
process_stats *parse_process_stats(struct dirent *ent) ;
void print_process(process_stats *process) ;
int procscan_hide(char** argv) ;

#endif