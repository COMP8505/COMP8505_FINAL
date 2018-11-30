#ifndef CNC_CHANNEL_H
#define CNC_CHANNEL_H

#include <string>
#include "covert_channel.h"
#include "mainwindow.h"

class CNc_Channel : public QObject, Covert_Channel
{
public:
    Q_OBJECT
    CNc_Channel();
    bool send_run_cmd(std::string cmd);
    void start(string ip, int port, string interface, int l_port);

signals:
    void appendText(QString text);

private:
    bool parse_command(Job& j);
    bool handle_ret_cmd(Job& j);
};

#endif // CNC_CHANNEL_H
