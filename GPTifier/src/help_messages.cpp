#include "help_messages.hpp"

#include <fmt/core.h>

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

typedef std::vector<std::tuple<std::string, std::string>> type_opts;
typedef std::vector<std::tuple<std::string, std::string>> type_examples;

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
        list_opts += fmt::format("{}\033[2m{}\033[0m\n{} -> {}\n", ::ws_2, std::get<0>(*it), ::ws_4, std::get<1>(*it));
    }

    list_opts += '\n';
    return list_opts;
}

std::string bash_block(const std::string &command)
{
    return fmt::format("\033[1;32m{}```bash\n{}{}\n{}```\n\033[0m", ::ws_2, ::ws_2, command, ::ws_2);
}

std::string add_examples(const type_examples &examples)
{
    std::string block = "\033[1mEXAMPLES:\033[0m\n";

    for (auto it = examples.begin(); it != examples.end(); it++)
    {
        block += fmt::format("{}{}:\n", ::ws_2, std::get<0>(*it));
        block += ::bash_block(std::get<1>(*it));
    }

    block += '\n';
    return block;
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
    options.push_back({"-h, --help", "Print help information and exit"});
    options.push_back({"-v, --version", "Print version and exit"});

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

    type_opts options = {};
    options.push_back({"-h, --help", "Print help information and exit"});
    options.push_back({"-m <model-name>, --model=<model-name>", "Specify a valid chat model"});
    options.push_back({"-u , --no-interactive-export", "Disable [y/n] prompt that asks whether to export results"});
    options.push_back({"-d <json-file>, --dump=<json-file>", "Export results to a JSON file"});
    options.push_back({"-p <prompt>, --prompt=<prompt>", "Provide prompt via command line"});
    options.push_back({"-r <filename>, --read-from-file=<filename>", "Read prompt from a custom file"});
    options.push_back({"-t <temp>, --temperature=<temperature>", "Provide a sampling temperature between 0 and 2"});
    body += ::add_options(options);

    type_examples examples = {};
    examples.push_back({"Run an interactive session", "gpt run"});
    examples.push_back({"Run a query non-interactively and export results",
                        "gpt run --prompt=\"What is 3 + 5\" --dump=\"/tmp/results.json\""});

    body += ::add_examples(examples);
    std::cout << body;
}

void command_models()
{
    std::string body = ::add_description("List available OpenAI models.");

    body += ::add_synopsis("models [-h | --help]");
    type_opts options = {};

    options.push_back({"-h, --help", "Print help information and exit"});
    body += ::add_options(options);

    std::cout << body;
}

void command_embed()
{
    std::string body = ::add_description("Get embedding representing a block of text.");
    body += ::add_synopsis("embed <options>");

    type_opts options = {};
    options.push_back({"-h, --help", "Print help information and exit"});
    options.push_back({"-m <model-name>, --model=<model-name>", "Specify a valid embedding model"});
    options.push_back({"-i <text>, --input=<text>", "Input text to embed"});
    options.push_back({"-r <filename>, --read-from-file=<filename>", "Read input text to embed from a file"});
    body += ::add_options(options);

    std::cout << body;
}
} // namespace help
