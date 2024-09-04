#include "cli.hpp"

#include <getopt.h>
#include <iostream>

namespace
{
void exit_on_failure()
{
    std::cerr << "Try running with -h or --help for more information\n";
    exit(EXIT_FAILURE);
}
} // namespace

namespace cli
{

bool get_opts_models(const int argc, char **argv)
{
    bool print_help = false;

    while (true)
    {
        static struct option long_options[] = {{"help", no_argument, 0, 'h'}, {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "h", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            print_help = true;
            break;
        default:
            exit_on_failure();
        }
    }

    return print_help;
}

} // namespace cli
