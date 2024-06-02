#include "command_embed.hpp"

#include <getopt.h>
#include <iostream>

struct EmbeddingParameters
{
    bool print_help = false;
};

void read_cli_embed(const int argc, char **argv, EmbeddingParameters &params)
{
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
            params.print_help = true;
            break;
        default:
            std::cerr << "Try running with -h or --help for more information\n";
            ::exit(EXIT_FAILURE);
        }
    }
}

void command_embed(const int argc, char **argv)
{
    EmbeddingParameters embed_parameters;
    ::read_cli_embed(argc, argv, embed_parameters);
}
