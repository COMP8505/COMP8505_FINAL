#ifndef CNC_CHANNEL_H
#define CNC_CHANNEL_H

#include <string>
#include "covert_channel.h"


class Cnc_Channel : public Covert_Channel
{
public:
    Cnc_Channel();
    bool send_run_cmd(std::string cmd);
    void start(string interface);

private:
    bool parse_command(Job& j);
    bool handle_ret_cmd(Job& j);
};

#endif // CNC_CHANNEL_H
