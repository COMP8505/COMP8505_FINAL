/*This is the sample program to notify us for the file creation and file deletion takes place in “/tmp” directory*/

#include "file_watcher.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/inotify.h>


File_Watcher::File_Watcher(){
    
}

File_Watcher::File_Watcher(){
    
}


int File_Watcher::run()
{
    int length, i = 0;
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];

    /*creating the INOTIFY instance*/
    fd = inotify_init();

    /*checking for error*/
    if (fd < 0)
    {
        perror("inotify_init");
    }

    /*adding the “/tmp” directory into watch list. Here, the suggestion is to validate the existence of the directory before adding into monitoring list.*/
    wd = inotify_add_watch(fd, WATCH_DIR, IN_CLOSE_WRITE);

    /*read to determine the event change happens on “/tmp” directory. Actually this read blocks until the change event occurs*/

    while(length = read(fd, buffer, EVENT_BUF_LEN)){
        /*checking for error*/
        if (length < 0)
        {
            perror("read");
        }

        /*actually read return the list of change events happens. Here, read the change event one by one and process it accordingly.*/
        while (i < length)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len)
            {
                ///do stuff here
            }
            i += EVENT_SIZE + event->len;
        }
    }

    
    /*removing the “/tmp” directory from the watch list.*/
    inotify_rm_watch(fd, wd);

    /*closing the INOTIFY instance*/
    close(fd);
}