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

int SID = -1;

void forkit(void) {
    SID = fork();
    if (SID == 0) {
        SID = setsid();
        printf("Child Became new process: %d\n", SID);
    } else {
        printf("Killing parent %d\n", SID);
        exit(EXIT_SUCCESS);
    }
}

int main(void) {
    SID = getpid();
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            forkit();
        }
        printf("doing work on: %d\n", SID);
        sleep(1);
    }
}