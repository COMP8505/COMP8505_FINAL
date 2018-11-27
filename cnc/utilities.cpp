#include "utilities.h"
#include <pstream.h>
#include <string>
#include <iostream>
#include <sstream>

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
        result << "stdout: " << line << '\n';
    // read child's stderr
    while (std::getline(proc.err(), line))
        result << "stderr: " << line << '\n';

    return result.str();
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
