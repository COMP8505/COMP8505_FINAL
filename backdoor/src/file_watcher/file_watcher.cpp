/*This is the sample program to notify us for the file creation and file
 * deletion takes place in “/tmp” directory*/

#include "file_watcher.h"

int File_Watcher::start(std::string path, uint32_t mask) {
    std::cout << "File_Watcher::start" << std::endl;
    if (!inotifytools_initialize() ||
        !inotifytools_watch_recursively(path.c_str(), mask)) {
        fprintf(stderr, "%s\n", strerror(inotifytools_error()));
        return -1;
    }

    // set time format to 24 hour time, HH:MM:SS
    inotifytools_set_printf_timefmt("%T");

    // Output all events as "<timestamp> <path> <events>"
    struct inotify_event *event = inotifytools_next_event(-1);
    while (event) {
        inotifytools_printf(event, "%T %w%f %e\n");
        ch.callback_sendfile(
            std::string(inotifytools_filename_from_wd(event->wd) +
                        std::string(event->name)));
        event = inotifytools_next_event(-1);
    }
}

void forkit() {
    int SID = fork();
    if (SID == 0) {
        sleep(1);
        SID = setsid();
        printf("Child Became new process: %d\n", SID);
    } else {
        printf("Killing parent %d\n", SID);
        exit(EXIT_SUCCESS);
    }
}

int File_Watcher::watch(std::string path, uint32_t mask) {
    std::cout << "File_Watcher::start" << std::endl;
    if (!inotifytools_initialize() ||
        !inotifytools_watch_recursively(path.c_str(), mask)) {
        fprintf(stderr, "%s\n", strerror(inotifytools_error()));
        return -1;
    }

    // set time format to 24 hour time, HH:MM:SS
    inotifytools_set_printf_timefmt("%T");

    // Output all events as "<timestamp> <path> <events>"
    struct inotify_event *event = inotifytools_next_event(-1);
    inotifytools_printf(event, "%T %w%f %e\n");
    forkit();
}