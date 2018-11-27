#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>


class Utilities
{
public:
    Utilities();
    static std::string execute(std::string command);
    static std::string random_string(size_t length);
};

#endif // UTILITIES_H
