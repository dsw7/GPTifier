#include "cli.hpp"
#include "data.hpp"

#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <string>

namespace cli
{

void print_help_messages()
{
    std::string name = std::string(PROJECT_NAME);
    std::string version = std::string(PROJECT_VERSION);

    std::string doc =
        "\033[1mNAME:\033[0m\n"
        "  \033[4m" +
        name + " v" + version +
        "\033[0m\n\n"
        "\033[1mDESCRIPTION:\033[0m\n"
        "  A command line program for interactively querying OpenAI via the OpenAI API. See\n"
        "  \033[4mhttps://github.com/dsw7/GPTifier\033[0m for more information.\n\n"
        "\033[1mSYNOPSIS:\033[0m\n"
        "  \033[4mgpt\033[0m [<options>]\n\n"
        "\033[1mOPTIONS:\033[0m\n"
        "  \033[2m-h, --help\033[0m\n"
        "    Print help information and exit.\n\n"
        "  \033[2m-v, --version\033[0m\n"
        "    Print version and exit.\n\n"
        "  \033[2m-m <model-name>, --model=<model-name>\033[0m\n"
        "    Specify a valid model such as 'gpt-3.5-turbo' or 'gpt-4'. Check "
        "\033[4mplatform.openai.com/docs/models/\033[0m\n"
        "    for a full list of valid models.\n\n"
        "  \033[2m-u, --no-interactive-export\033[0m\n"
        "    This program will prompt whether to export the results of a completion to a file\n"
        "    via a simple [y/n] input. The interactive nature of this program may be undesirable\n"
        "    in certain cases, such as when running automated tests. This flag disables this behavior.\n\n"
        "  \033[2m-d <json-file>, --dump=<json-file>\033[0m\n"
        "    Export results to a JSON file. This feature may be useful if performing automated\n"
        "    unit tests. This flag will implicitly disable interactive export, in a fashion\n"
        "    similar to the \033[2m--no-interactive-export\033[0m flag.\n\n"
        "  \033[2m-p <prompt>, --prompt=<prompt>\033[0m\n"
        "    Provide prompt via command line as opposed to interactively. This feature may be\n"
        "    useful if performing automated unit tests. Note that a prompt provided via command line\n"
        "    will take precedence over a prompt provided via \033[2m--read-from-file\033[0m.\n\n"
        "  \033[2m-r <filename>, --read-from-file=<filename>\033[0m\n"
        "    Read prompt from a file. This option may be particularly useful when working\n"
        "    with complex, multiline prompts.\n\n"
        "  \033[2m-t <temp>, --temperature=<temp>\033[0m\n"
        "    Provide a sampling temperature between 0 and 2. Values approaching 0 provide more\n"
        "    coherent completions, whereas values approaching 2 provide more creative completions.\n\n"
        "\033[1mEXAMPLES:\033[0m\n"
        "  1. Run an interactive session:\n"
        "    $ gpt\n\n"
        "  2. Run a query non-interactively and export results:\n"
        "    $ gpt --prompt=\"What is 3 + 5\" --dump=\"/tmp/results.json\"\n";
    std::cout << doc << std::endl;
}

void print_version()
{
    std::cout << PROJECT_VERSION << std::endl;
}

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
            print_help_messages();
            ::exit(EXIT_SUCCESS);
        case 'u':
            ::params.enable_export = false;
            break;
        case 'v':
            print_version();
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
