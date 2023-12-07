#include "cli.hpp"

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
        "\033[1m\033[4m" + name +
        "\033[0m - See \033[4mhttps://github.com/dsw7/GPTifier\033[0m for more information.\n\n"
        " \033[1mVersion:\033[0m " +
        version +
        "\n\n"
        " \033[1mOptions:\033[0m\n"
        "   -h, --help                      Print help information and exit.\n"
        "   -v, --version                   Print version and exit.\n"
        "   -u, --no-interactive-export     Disable prompt asking whether to export results.\n"
        "   -d, --dump <file>               Export results to a JSON file.\n"
        "   -m, --model <model-name>        Specify a valid model.\n"
        "   -p, --prompt <prompt>           Provide prompt via command line. Takes precedence over -r argument.\n"
        "   -r, --read-from-file <filename> Read prompt from file.\n"
        "   -t, --temperature <float>       Provide sampling temperature between 0 and 2.\n";
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
            print_help_messages();
            ::exit(EXIT_SUCCESS);
        case 'u':
            params.enable_export = false;
            break;
        case 'v':
            print_version();
            ::exit(EXIT_SUCCESS);
        case 'd':
            params.dump = ::optarg;
            break;
        case 'p':
            params.prompt = ::optarg;
            break;
        case 't':
            params.temperature = ::optarg;
            break;
        case 'r':
            params.prompt_file = ::optarg;
            break;
        case 'm':
            params.model = ::optarg;
            break;
        default:
            std::cerr << "Try running with -h or --help for more information" << std::endl;
            ::exit(EXIT_FAILURE);
        }
    };
}

} // namespace cli
