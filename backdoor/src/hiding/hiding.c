#include "includes/msbuffer.h"
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

int this_process = 0;
int OWN_PID;

int compare_process_stats(const void *a, const void *b) {
    process_stats **a_stat = (process_stats **)a;
    process_stats **b_stat = (process_stats **)b;

    float res;
    float call_ratio_a, call_ratio_b;

    if ((*a_stat)->PID == 1)
        return 1;
    if ((*b_stat)->PID == 1)
        return -1;

    call_ratio_a = ((*a_stat)->rchar + (*a_stat)->wchar) /
                   fmax(((*a_stat)->syscr + (*a_stat)->syscw), 1.0);
    call_ratio_b = ((*b_stat)->rchar + (*b_stat)->wchar) /
                   fmax(((*b_stat)->syscr + (*b_stat)->syscw), 1.0);

    call_ratio_a += (strlen((*a_stat)->cmdline) * 2.5f);
    call_ratio_b += (strlen((*b_stat)->cmdline) * 2.5f);

    res = call_ratio_b - call_ratio_a;

    return (int)res;
}

char *read_file(const char *path) {
    char *databuf = malloc(DATA_BUFF_LEN);
    FILE *fd;

    memset(databuf, 0, DATA_BUFF_LEN);
    fd = fopen(path, "r");
    fread(databuf, DATA_BUFF_LEN, 1, fd);
    fclose(fd);
    return databuf;
}

process_stats **allocate_proc_stats() {
    DIR *proc_dir = opendir("/proc");
    struct dirent *ent;
    long proccount = 0;

    if (proc_dir == NULL) {
        perror("opendir(/proc)");
        return NULL;
    }
    while (ent = readdir(proc_dir)) {
        if (!isdigit((*ent->d_name)))
            continue;
        proccount++;
    }
    printf("Process count: %d\n", proccount);

    process_stats **processes = malloc((proccount) * sizeof(process_stats *));
    printf("Allocating: [%ld B] for for process info\n",
           (proccount * sizeof(process_stats *)));

    closedir(proc_dir);
    return processes;
}

process_stats *parse_process_stats(struct dirent *ent) {

    process_stats *stats;
    char pathbuf[LINE];
    char *databuf, *token;
    FILE *fd;
    long tgid;

    if (!isdigit((*ent->d_name))) {
        return NULL;
    }

    tgid = strtol(ent->d_name, NULL, 10);

    memset(pathbuf, 0, LINE);
    snprintf(pathbuf, LINE, "/proc/%ld/cmdline", tgid);
    databuf = read_file(pathbuf);

    if (strlen(databuf) == 0) {
        return NULL;
    }

    // create the stats variable we will return later
    stats = malloc(sizeof(process_stats));
    memset(stats, 0, sizeof(process_stats));

    stats->PID = tgid;
    strcpy(stats->cmdline, databuf);

    memset(pathbuf, 0, LINE);
    snprintf(pathbuf, LINE, "/proc/%ld/io", tgid);
    free(databuf); // gotta avoid those memory leaks
    databuf = read_file(pathbuf);
    sscanf(databuf, "rchar: %ld\n\
            wchar: %ld\
            syscr: %ld\
            syscw: %ld\
            read_bytes: %ld\
            write_bytes: %ld\
            cancelled_write_bytes: %ld",
           &(stats->rchar), &(stats->wchar), &(stats->syscr), &(stats->syscw),
           &(stats->read_bytes), &(stats->write_bytes),
           &(stats->cancelled_write_bytes));
    // sscanf(token, "IpExt: %ld %ld %ld %ld %ld %ld %ld %ld", &c0, &c1, &c2,
    // &c3, &c4, &c5, &(stats->read_bytes), &(stats->write_bytes)); printf("%ld,
    // %ld\n\n", stats->read_bytes, stats->write_bytes);

    memset(pathbuf, 0, LINE);
    // TODO: Keep going with the netstat parse

    return stats;
}

void print_process(process_stats *process) {
    printf("PID: %ld:\
        \n\tCMDLine: %s\
        \n\trchar: %ld\
        \n\twchar: %ld\
        \n\tsyscr: %ld\
        \n\tsyscw: %ld\
        \n\tread_bytes: %ld\
        \n\twrite_bytes: %ld\
        \n\tcancelled_write_bytes: %ld\
        \n\
        \n\n\n",
           process->PID, process->cmdline, process->rchar, process->wchar,
           process->syscr, process->syscw, process->read_bytes,
           process->write_bytes, process->cancelled_write_bytes);
}

int procscan_hide() {

    DIR *proc_dir;
    struct dirent *ent;
    long procstored = 0;
    process_stats **processes;

    processes = allocate_proc_stats();

    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir(/proc)");
        return 1;
    }

    if (processes == NULL) {
        return 0;
    }

    while (ent = readdir(proc_dir)) {
        process_stats *stats = parse_process_stats(ent);
        if (stats == NULL) {
            continue;
        }
        processes[procstored++] = stats;
    }
    closedir(proc_dir);

    qsort(processes, procstored, sizeof(process_stats *),
          compare_process_stats);

    // print_process(processes[0]);

    for (int i = 0; i < procstored; i++) {
        if (processes[i]->PID == OWN_PID) {
            print_process(processes[i]);
        }
    }

    this_process = 0;
    msbuffer *namebuf = create_msbuffer();
    fprintf_msbuffer(namebuf, "/proc/%ld", OWN_PID);
    char process_path[32];
    memset(process_path, 0, 32);
    strncpy(process_path, namebuf->buffer, 32);
    strcat_msbuffer(namebuf, "/cmdline");

    /* mask the process name */
    // memset(argv[0], 0, strlen(argv[0]));
    // strcpy(argv[0], namebuf->buffer);
    // prctl(PR_SET_NAME, namebuf->buffer, 0, 0);

    // prctl(PR_SET_NAME, "not a bad process\0", NULL, NULL, NULL);
    // memcpy(argv[0], "not a bad process at all", 25);

    // FILE* cmdline_fp = fopen(namebuf->buffer, "ab+");
    // fwrite("this is a great program totally dont worry about it", 1, 51,
    // cmdline_fp); fflush(cmdline_fp); fclose(cmdline_fp);

    for (int i = 0; i < procstored; i++) {
        free(processes[i]);
    }
}
