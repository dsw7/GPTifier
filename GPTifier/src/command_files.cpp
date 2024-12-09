#include "command_files.hpp"

#include <iostream>

void command_files(int argc, char **argv)
{
    std::cout << argc << '\n';
    std::cout << argv[0] << ' ' << argv[1];
}
