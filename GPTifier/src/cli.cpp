#include "cli.hpp"
#include "help_messages.hpp"
#include "params.hpp"

#include <cstdlib>
#include <getopt.h>
#include <iostream>

namespace cli
{

void parse_cli(const int argc, char **argv)
{
    while (true)
    {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"no-interactive-export", no_argument, 0, 'u'},
            {"version", no_argument, 0, 'v'},
            {"dump", required_argument, 0, 'd'},
            {"model", required_argument, 0, 'm'},
            {"prompt", required_argument, 0, 'p'},
            {"read-from-file", required_argument, 0, 'r'},
            {"temperature", required_argument, 0, 't'},
            {0, 0, 0, 0},
        };

        int option_index = 0;
        int c = ::getopt_long(argc, argv, "huvd:m:p:r:t:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            ::print_help_messages();
            ::exit(EXIT_SUCCESS);
        case 'u':
            ::params.enable_export = false;
            break;
        case 'v':
            std::cout << PROJECT_VERSION << std::endl;
            ::exit(EXIT_SUCCESS);
        case 'd':
            ::params.dump = ::optarg;
            break;
        case 'p':
            ::params.prompt = ::optarg;
            break;
        case 't':
            ::params.temperature = ::optarg;
            break;
        case 'r':
            ::params.prompt_file = ::optarg;
            break;
        case 'm':
            ::params.model = ::optarg;
            break;
        default:
            std::cerr << "Try running with -h or --help for more information" << std::endl;
            ::exit(EXIT_FAILURE);
        }
    };
}

} // namespace cli
