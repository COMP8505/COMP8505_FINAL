#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <vector>


class Utilities
{
public:
    struct MatchPathSeparator
    {
        bool operator()( char ch ) const
        {
            return ch == '/';
        }
    };

    Utilities();
    static std::string execute(std::string command);
    static std::string random_string(size_t length);
    static std::string basename( std::string const& pathname );

    static bool readfile(std::string filename, std::vector<unsigned char>& data);
    static bool writefile(std::string filename, std::string data);

    static void xor_crypt(const std::string &key, std::vector<unsigned char> &data);
    static void xor_crypt(const std::string &key, std::string &data);
};

#endif // UTILITIES_H
