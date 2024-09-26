#include "command_short.hpp"

#include "cli.hpp"
#include <iostream>
#include <string>

void command_short(const int argc, char **argv)
{
    std::string prompt = cli::get_opts_short(argc, argv);
    std::cout << prompt;
}
