#include "help_messages.hpp"

#include <fmt/core.h>

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

typedef std::vector<std::tuple<std::string, std::string>> type_opts;

const std::string ws_2 = std::string(2, ' ');
const std::string ws_4 = std::string(4, ' ');

std::string add_description(const std::string &text)
{
    return fmt::format("\033[1mDESCRIPTION:\033[0m\n{}{}\n\n", ::ws_2, text);
}

std::string add_synopsis(const std::string &text)
{
    return fmt::format("\033[1mSYNOPSIS:\033[0m\n{}\033[4mgpt\033[0m {}\n\n", ::ws_2, text);
}

std::string add_options(const type_opts &options)
{
    std::string list_opts = "\033[1mOPTIONS:\033[0m\n";

    for (auto it = options.begin(); it != options.end(); it++)
    {
        list_opts += fmt::format("{}\033[2m{}\033[0m\n{}{}\n", ::ws_2, std::get<0>(*it), ::ws_4, std::get<1>(*it));
    }

    list_opts += '\n';
    return list_opts;
}

namespace help
{
void root_messages()
{
    std::string name = std::string(PROJECT_NAME);
    std::string version = std::string(PROJECT_VERSION);

    std::string body = "\033[1mNAME:\033[0m\n";
    body += fmt::format("{}\033[4m{} v{}\033[0m\n\n", ::ws_2, name, version);

    std::string description = "A command line program for interactively querying OpenAI via the OpenAI API.\n";
    description += ::ws_2 + "See \033[4mhttps://github.com/dsw7/GPTifier\033[0m for more information.";

    body += ::add_description(description);
    body += ::add_synopsis("[-v | --version] [-h | --help] [run] [models] [embed]");

    type_opts options = {};
    options.push_back({"-h, --help", "Print help information and exit."});
    options.push_back({"-v, --version", "Print version and exit."});

    body += ::add_options(options);
    body += "\033[1mCOMMANDS:\033[0m\n"
            "  \033[2mrun\033[0m    -> Run a query against an appropriate model.\n"
            "  \033[2mmodels\033[0m -> List available OpenAI models.\n"
            "  \033[2membed\033[0m  -> Get embedding representing a block of text.\n\n"
            "  Try gpt <subcommand> [-h | --help] for subcommand specific help.\n\n";
    std::cout << body;
}

void command_run()
{
    std::string body = ::add_description("Create a chat completion.");
    body += ::add_synopsis("run <options>");
    body += "\033[1mOPTIONS:\033[0m\n"
            "  \033[2m-h, --help\033[0m\n"
            "    Print help information and exit.\n\n"
            "  \033[2m-m <model-name>, --model=<model-name>\033[0m\n"
            "    Specify a valid model such as \"gpt-3.5-turbo\" or \"gpt-4.\"\n\n"
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
            "    $ gpt run\n\n"
            "  2. Run a query non-interactively and export results:\n"
            "    $ gpt run --prompt=\"What is 3 + 5\" --dump=\"/tmp/results.json\"\n\n";

    std::cout << body;
}

void command_models()
{
    std::string body = ::add_description("List available OpenAI models.");
    body += ::add_synopsis("models [-h | --help]");
    body += "\033[1mOPTIONS:\033[0m\n"
            "  \033[2m-h, --help\033[0m\n"
            "    -> Print help information and exit.\n\n";

    std::cout << body;
}

void command_embed()
{
    std::string body = ::add_description("Get embedding representing a block of text.");
    body += ::add_synopsis("embed <options>");

    type_opts options = {};
    options.push_back({"-h, --help", "-> Print help information and exit"});
    options.push_back({"-m <model-name>, --model=<model-name>", "-> Specify a valid embedding model"});
    options.push_back({"-i <text>, --input=<text>", "-> Input text to embed"});
    options.push_back({"-r <filename>, --read-from-file=<filename>", "-> Read input text to embed from a file"});
    body += ::add_options(options);

    std::cout << body;
}
} // namespace help
