#include "refork.h"

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

void init(void) {
    SID = getpid();
}