#include "testing.h"
#include "utilities.h"
#include <iostream>
#include <string>

Testing::Testing()
{

}



void Testing::run_tests(){
    //ifconfig
    std::string command = "ifconfig";
    std::string result = Utilities::execute(command);
    std::cout << "Running command: " << command << std::endl;
    std::cout << result << std::endl;;

}
