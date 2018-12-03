#ifndef File_Watcher_H
#define File_Watcher_H

#include <string>
using std::string;

#define EVENT_SIZE (sizeof(struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))





class File_Watcher
{
public:
    File_Watcher();
    void start(string folder);

private:
    const char* WATCH_DIR = "/tmp";

};

#endif 
