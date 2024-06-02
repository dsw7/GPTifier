#include "command_embed.hpp"

#include "help_messages.hpp"

#include <getopt.h>
#include <iostream>
#include <string>

struct EmbeddingParameters
{
    bool print_help = false;
    std::string input;
    std::string model;
};

void read_cli_embed(const int argc, char **argv, EmbeddingParameters &params)
{
    while (true)
    {
        static struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                               {"model", required_argument, 0, 'm'},
                                               {"input", required_argument, 0, 'i'},
                                               {0, 0, 0, 0}};

        int option_index = 0;
        int c = ::getopt_long(argc, argv, "hm:i:", long_options, &option_index);

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
            params.model = ::optarg;
            break;
        case 'i':
            params.input = ::optarg;
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

    if (embed_parameters.print_help)
    {
        help::command_embed();
        return;
    }
}
