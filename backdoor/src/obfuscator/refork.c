#include "refork.h"

void forkit(void) {
    SID = fork();
    if (SID == 0) {
        sleep(1);
        SID = setsid();
        printf("Child Became new process: %d\n", SID);
    } else {
        printf("Killing parent %d\n", SID);
        exit(EXIT_SUCCESS);
    }
}

void init(int count) {
    SID = getpid();
    printf("%ld\n", sizeof(callback));
    printf("%ld\n", sizeof(callback));
    
    //cb_array = malloc(count * sizeof(callback));
}

void refork_add_callback(callback cb) {

}