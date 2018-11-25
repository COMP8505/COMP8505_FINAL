#include "includes/msbuffer.h"
#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/prctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define DATA_BUFF_LEN 8192
#define LINE 128

char *executable_name;
int OWN_PID;

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
    return processes;
}

process_stats *parse_process_stats(struct dirent *ent) {

    process_stats *stats;
    char pathbuf[LINE];
    char *databuf, *token;
    FILE *fd;
    long tgid;
    int count = 3;
    long c0;

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

int main(int argc, char **argv) {
    int inotify_fd;
    DIR *proc_dir;
    struct dirent *ent;
    long tgid;
    long proccount = 0;
    long procstored = 0;
    process_stats **processes;
    process_stats stat;

    OWN_PID = getpid();
    printf("Process PID: %d\n", OWN_PID);

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

    for (int i = 0; i < procstored; i++) {
        printf("PID: %ld:\
        \nCMDLine: %s\
        \nrchar: %ld\
        \nwchar: %ld\
        \nsyscr: %ld\
        \nsyscw: %ld\
        \nread_bytes: %ld\
        \nwrite_bytes: %ld\
        \ncancelled_write_bytes: %ld\
        \n\
        \n\n\n",
               processes[i]->PID, processes[i]->cmdline,
               processes[i]->rchar, processes[i]->wchar,
               processes[i]->syscr, processes[i]->syscw,
               processes[i]->read_bytes, processes[i]->write_bytes,
               processes[i]->cancelled_write_bytes);
    }

    inotify_fd = inotify_init1(IN_NONBLOCK);

    // int fd = socket(AF_INET, SOCK_DGRAM, 0);
    // sockaddr_in sa;
    // bzero(&sa, sizeof(sockaddr_in));
    // sa.sin_family = AF_INET;
    // sa.sin_port = htons(8000);
    // sa.sin_addr.s_addr = inet_addr("142.232.142.161");
    // for (int i = 0; i < 1000; i++) {
    //     sleep(1);
    //     char buf[52];
    //     memset(buf, 65, 51);
    //     buf[52] = '\0';
    //     int ret = sendto(fd, buf, sizeof(buf), 0, (sockaddr *)&sa,
    //     sizeof(sa)); printf("Sent: %d\n", ret);
    // }
    return 1;
}