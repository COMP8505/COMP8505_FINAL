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

#define MAX_ARGLEN_LONG 8192

char *executable_name;
int OWN_PID;

typedef struct process_stats {
    char cmdline[MAX_ARGLEN_LONG];
    long rchar;
    long wchar;
    long syscr;
    long syscw;
    long read_bytes;
    long write_bytes;
} process_stats;

int main(int argc, char **argv) {
    int inotify_fd;
    DIR *proc_dir;
    struct dirent *ent;
    long tgid;
    long proccount = 0;
    process_stats *processes;

    OWN_PID = getpid();
    printf("Process PID: %d\n", OWN_PID);

    proc_dir = opendir("/proc");
    if (proc_dir == NULL) {
        perror("opendir(/proc)");
        return 1;
    }

    // while (ent = readdir(proc_dir)) {
    //     if (!isdigit((*ent->d_name)))
    //         continue;
    //     proccount++;
    // }
    // printf("Process count: %d\n%d\n", proccount, sizeof(process_stats));

    while (ent = readdir(proc_dir)) {
        if (!isdigit((*ent->d_name)))
            continue;
        tgid = strtol(ent->d_name, NULL, 10);

        char cmdlinepath[PATH_MAX];
        bzero(cmdlinepath, PATH_MAX);
        snprintf(cmdlinepath, PATH_MAX, "/proc/%ld/cmdline", tgid);

        FILE *cmdlinefile = fopen(cmdlinepath, "r");
        char cmdargs[MAX_ARGLEN_LONG];
        char cmdline[MAX_ARGLEN_LONG / 32];
        bzero(cmdargs, MAX_ARGLEN_LONG);
        bzero(cmdline, MAX_ARGLEN_LONG / 32);

        while (fgets(cmdline, MAX_ARGLEN_LONG / 32, cmdlinefile)) {
            strcat(cmdargs, cmdline);
        }
        if (strlen(cmdargs) == 0) {
            fprintf(stderr, "File has no command line arguments\n");
            continue;
        }
        printf("%s:%d\n", cmdargs, strlen(cmdargs));

        char iopath[PATH_MAX];
        bzero(iopath, PATH_MAX);
        snprintf(iopath, PATH_MAX, "/proc/%ld/io", tgid);

        FILE *iofile = fopen(iopath, "r");
        char io_data[1024];
        char io_line[32];
        printf("/proc/%ld/io\n", tgid);
        while (fgets(io_line, 32, iofile)) {
            strcat(io_data, io_line);
            bzero(io_line, 32);
        }
        printf("%s\n", io_data);
        bzero(io_data, 1024);
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