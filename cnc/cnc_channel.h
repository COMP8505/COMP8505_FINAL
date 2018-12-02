#ifndef CNC_CHANNEL_H
#define CNC_CHANNEL_H

#include <QObject>
#include <string>
#include <thread>
#include "covert_channel.h"

class Cnc_Channel : public QObject, Covert_Channel
{
    Q_OBJECT
public:
    explicit Cnc_Channel(QObject *parent = nullptr);
    bool send_run_cmd(std::string cmd);
    bool send_file(string filename);
    bool get_file(string filepath);
    void start(std::string ip, int port, std::string interface, int l_port);

signals:
    void appendText(QString text);

public slots:


private:
    bool parse_command(Job& j);
    bool handle_ret_cmd(Job& j);
    bool handle_return_file(Job& j);
    void thread_call();
};

#endif // CNC_CHANNEL_H
