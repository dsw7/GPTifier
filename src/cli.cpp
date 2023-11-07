#include "cli.h"

#include <cstdlib>
#include <getopt.h>
#include <iostream>

namespace cli
{

void help_message()
{
    std::string doc =
    "\033[1m\033[4mGPTifier\033[0m - See https://github.com/dsw7/GPTifier for more information.\n\n"
    "\033[1mOptions:\033[0m\n"
    "  -h, --help    Print help information and exit.\n"
    "  -v, --version Print version and exit.\n";
    std::cout << doc << std::endl;
}

void parse_cli(const int argc, char **argv)
{
    while (true)
    {
        static struct option long_options[] =
        {
            {"help",        no_argument,       0, 'h'},
            {"version",     no_argument,       0, 'v'},
            {0,             0,                 0,  0 }
        };

        int option_index = 0;
        int c = ::getopt_long(
            argc, argv, "hv", long_options, &option_index
        );

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'h':
                help_message();
                ::exit(EXIT_SUCCESS);
            case 'v':
                ::exit(EXIT_SUCCESS);
            default:
                std::cerr << "Try running with -h or --help for more information" << std::endl;
                ::exit(EXIT_FAILURE);
        }
    };
}

} // cli
