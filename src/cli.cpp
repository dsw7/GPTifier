#include "cli.h"

#include <cstdlib>
#include <getopt.h>
#include <iostream>

namespace cli
{

void print_help_messages()
{
    std::string name = std::string(PROJECT_NAME);
    std::string version = std::string(PROJECT_VERSION);

    std::string doc =
    "\033[1m\033[4m" + name + "\033[0m - See https://github.com/dsw7/GPTifier for more information.\n\n"
    " \033[1mVersion:\033[0m " + version + "\n\n"
    " \033[1mOptions:\033[0m\n"
    "   -h, --help            Print help information and exit.\n"
    "   -v, --version         Print version and exit.\n"
    "   -p, --prompt <prompt> Provide prompt via command line.\n"
    "   -d, --dump <file>     Export results to a JSON file.\n";
    std::cout << doc << std::endl;
}

void print_version()
{
    std::cout << PROJECT_VERSION << std::endl;
}

void parse_cli(const int argc, char **argv, Params &params)
{
    while (true)
    {
        static struct option long_options[] =
        {
            {"help",    no_argument,       0, 'h'},
            {"version", no_argument,       0, 'v'},
            {"prompt",  required_argument, 0, 'p'},
            {"dump",    required_argument, 0, 'd'},
            {0,         0,                 0,  0 },
        };

        int option_index = 0;
        int c = ::getopt_long(
            argc, argv, "hvd:p:", long_options, &option_index
        );

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
            case 'h':
                print_help_messages();
                ::exit(EXIT_SUCCESS);
            case 'v':
                print_version();
                ::exit(EXIT_SUCCESS);
            case 'd':
                params.dump = ::optarg;
                break;
            case 'p':
                params.prompt = ::optarg;
                break;
            default:
                std::cerr << "Try running with -h or --help for more information" << std::endl;
                ::exit(EXIT_FAILURE);
        }
    };
}

} // cli
