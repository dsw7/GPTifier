#include "command_files.hpp"

#include "cli.hpp"
#include "help_messages.hpp"

#include <iostream>
#include <string>

void command_files(int argc, char **argv)
{
    if (argc < 3) {
        cli::command_files();
        exit(EXIT_FAILURE);
    }

    std::string subcommand = argv[2];

    if (subcommand == "-h" or subcommand == "--help") {
        cli::command_files();
        exit(EXIT_SUCCESS);
    }
}
