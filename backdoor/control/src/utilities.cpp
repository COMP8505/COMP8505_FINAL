#include "utilities.h"
#include "pstream.h"
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <fstream>
#include <boost/filesystem.hpp>

Utilities::Utilities()
{

}




std::string Utilities::execute(std::string command)
{
    // run a process and create a streambuf that reads its stdout and stderr
    redi::ipstream proc(command, redi::pstreams::pstdout | redi::pstreams::pstderr);
    std::string line;
    std::ostringstream result;
    // read child's stdout
    while (std::getline(proc.out(), line))
        result << line << '\n';
    // read child's stderr
    while (std::getline(proc.err(), line))
        result << line << '\n';

    return result.str();
}


bool Utilities::readfile(std::string filename, std::vector<unsigned char>& data){
    std::ifstream f(filename);
    f >> std::noskipws;

    if (f.is_open()){
        std::istream_iterator<unsigned char> start(f), end;
        data = std::vector<unsigned char>(start, end);
        f.close();
        return true;
    }
    else{
        std::cout << "could not open file for reading" << std::endl;
        return false;
    }
}

bool Utilities::writefile(std::string filename, std::string data){
    std::ofstream f(filename, std::ios::out | std::ios::binary);

    if (f.is_open()){
        f.write((const char*)&data[0], data.size());
        f.close();
        return true;
    }
    else{
        std::cout << "could not open file for writing" << std::endl;
        return false;
    }
}


std::string Utilities::basename( std::string const& pathname ) {
    return std::string(
        std::find_if( pathname.rbegin(), pathname.rend(),
                      MatchPathSeparator() ).base(),
        pathname.end() );
}


std::string Utilities::random_string(size_t length)
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}


void Utilities::xor_crypt(const std::string &key, std::vector<unsigned char> &data) {
    for (size_t i = 0; i != data.size(); i++)
        data[i] ^= key[ i % key.size() ];
}

void Utilities::xor_crypt(const std::string &key, std::string &data) {
    for (size_t i = 0; i != data.size(); i++)
        data[i] ^= key[ i % key.size() ];
}
