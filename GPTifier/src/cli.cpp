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

ParamsEmbedding get_opts_embed(const int argc, char **argv)
{
    ParamsEmbedding params;

    while (true)
    {
        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"model", required_argument, 0, 'm'},
                                               {"input", required_argument, 0, 'i'},
                                               {"read-from-file", required_argument, 0, 'r'},
                                               {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "hm:i:r:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            params.print_help = true;
            break;
        case 'm':
            params.model = optarg;
            break;
        case 'i':
            params.input = optarg;
            break;
        case 'r':
            params.input_file = optarg;
            break;
        default:
            exit_on_failure();
        }
    }

    return params;
}

} // namespace cli
