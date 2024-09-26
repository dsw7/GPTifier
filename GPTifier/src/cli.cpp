#include "cli.hpp"

#include "help_messages.hpp"

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

void get_opts_models(const int argc, char **argv)
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
            command_models();
            exit(EXIT_SUCCESS);
        default:
            exit_on_failure();
        }
    }
}

std::string get_opts_short(const int argc, char **argv)
{
    std::string prompt;

    while (true)
    {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'}, {"prompt", required_argument, 0, 'p'}, {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "hp:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            command_short();
            exit(EXIT_SUCCESS);
        case 'p':
            prompt = optarg;
            break;
        default:
            exit_on_failure();
        }
    }

    if (prompt.empty())
    {
        std::cerr << "Prompt is empty\n";
        exit_on_failure();
    }

    return prompt;
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
            cli::command_embed();
            exit(EXIT_SUCCESS);
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

ParamsRun get_opts_run(const int argc, char **argv)
{
    ParamsRun params;

    while (true)
    {
        static struct option long_options[] = {
            {"help", no_argument, 0, 'h'},
            {"no-interactive-export", no_argument, 0, 'u'},
            {"dump", required_argument, 0, 'd'},
            {"model", required_argument, 0, 'm'},
            {"prompt", required_argument, 0, 'p'},
            {"read-from-file", required_argument, 0, 'r'},
            {"temperature", required_argument, 0, 't'},
            {0, 0, 0, 0},
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hud:m:p:r:t:", long_options, &option_index);

        if (c == -1)
        {
            break;
        }

        switch (c)
        {
        case 'h':
            cli::command_run();
            exit(EXIT_SUCCESS);
        case 'u':
            params.enable_export = false;
            break;
        case 'd':
            params.json_dump_file = optarg;
            break;
        case 'p':
            params.prompt = optarg;
            break;
        case 't':
            params.temperature = optarg;
            break;
        case 'r':
            params.prompt_file = optarg;
            break;
        case 'm':
            params.model = optarg;
            break;
        default:
            exit_on_failure();
        }
    };

    return params;
}

} // namespace cli
