#include "testing.h"
#include "utilities.h"
#include <iostream>
#include <string>

Testing::Testing()
{

}


void Testing::runtests(){
    execute_test();
    writefile_test();
    readfile_test();

}


void Testing::execute_test(){
    //ifconfig
    std::string command = "ifconfig";
    std::string result = Utilities::execute(command);
    std::cout << "Running command: " << command << std::endl;
    std::cout << result << std::endl;
}




void Testing::readfile_test(){
    std::string filename = "file.txt";
    std::cout << "ReadFile: " << filename << std::endl;
    std::vector<unsigned char> data;
    Utilities::readfile(filename, data);
    for(auto& c : data)
        std::cout << c;
    std::cout << std::endl;

}

void Testing::writefile_test(){
    std::string filename = "file.txt";
    std::cout << "WriteFile: " << filename << std::endl;
    std::string testdata = "test1 test2\n test3 test4";
    std::cout << testdata << std::endl;
    Utilities::writefile(filename, testdata);
}
