#include "help_messages.hpp"

#include <fmt/core.h>

#include <iostream>
#include <string>

const std::string ws_a = std::string(2, ' ');

std::string add_description(const std::string &text)
{
    return fmt::format("\033[1mDESCRIPTION:\033[0m\n{}{}\n\n", ws_a, text);
}

std::string add_synopsis(const std::string &text)
{
    return "\033[1mSYNOPSIS:\033[0m\n" + ws_a + "\033[4mgpt\033[0m " + text + "\n\n";
}

namespace help
{
void root_messages()
{
    std::string name = std::string(PROJECT_NAME);
    std::string version = std::string(PROJECT_VERSION);

    std::string body = "\033[1mNAME:\033[0m\n"
                       "  \033[4m" +
                       name + " v" + version +
                       "\033[0m\n\n"
                       "\033[1mDESCRIPTION:\033[0m\n"
                       "  A command line program for interactively querying OpenAI via the OpenAI API. See\n"
                       "  \033[4mhttps://github.com/dsw7/GPTifier\033[0m for more information.\n\n"
                       "\033[1mSYNOPSIS:\033[0m\n"
                       "  \033[4mgpt\033[0m [-v | --version] [-h | --help] [run] [models]\n\n"
                       "\033[1mTOP LEVEL OPTIONS:\033[0m\n"
                       "  \033[2m-h, --help\033[0m\n"
                       "    Print help information and exit.\n\n"
                       "  \033[2m-v, --version\033[0m\n"
                       "    Print version and exit.\n\n"
                       "\033[1mCOMMANDS:\033[0m\n"
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
    body += "\033[1mOPTIONS:\033[0m\n"
            "  \033[2m-h, --help\033[0m\n"
            "    -> Print help information and exit\n"
            "  \033[2m-m <model-name>, --model=<model-name>\033[0m\n"
            "    -> Specify a valid model such as \"text-embedding-3-small\"\n"
            "  \033[2m-i <text>, --input=<text>\033[0m\n"
            "    -> Input text to embed\n"
            "  \033[2m-r <filename>, --read-from-file=<filename>\033[0m\n"
            "    -> Read input text to embed from a file\n\n";

    std::cout << body;
}
} // namespace help
