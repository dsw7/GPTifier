#include "help_messages.hpp"

#include <fmt/core.h>

#include <iostream>
#include <string>
#include <utility>
#include <vector>

namespace {

typedef std::vector<std::pair<std::string, std::string>> str_pair;

const std::string ws_2 = std::string(2, ' ');
const std::string ws_4 = std::string(4, ' ');

std::string add_description(const std::string &text)
{
    return fmt::format("\033[1mDESCRIPTION:\033[0m\n{}{}\n\n", ws_2, text);
}

std::string add_synopsis(const std::string &text)
{
    return fmt::format("\033[1mSYNOPSIS:\033[0m\n{}\033[4mgpt\033[0m {}\n\n", ws_2, text);
}

std::string add_options(const str_pair &options)
{
    std::string body = "\033[1mOPTIONS:\033[0m\n";

    for (auto it = options.begin(); it != options.end(); it++) {
        body += fmt::format("{}\033[2m{}\033[0m\n{} -> {}\n", ws_2, it->first, ws_4, it->second);
    }

    body += '\n';
    return body;
}

std::string add_commands(const str_pair &commands)
{
    std::string body = "\033[1mCOMMANDS:\033[0m\n";

    for (auto it = commands.begin(); it != commands.end(); it++) {
        body += fmt::format("{}\033[2m{}\033[0m\n{} -> {}\n", ws_2, it->first, ws_4, it->second);
    }

    body += fmt::format("\n{}Try gpt <subcommand> [-h | --help] for subcommand specific help.\n\n", ws_2);
    return body;
}

std::string bash_block(const std::string &command)
{
    return fmt::format("\033[1;32m{0}```bash\n{0}{1}\n{0}```\n\033[0m", ws_2, command);
}

std::string add_examples(const str_pair &examples)
{
    std::string body = "\033[1mEXAMPLES:\033[0m\n";

    for (auto it = examples.begin(); it != examples.end(); it++) {
        body += fmt::format("{}{}:\n", ws_2, it->first);
        body += bash_block(it->second);
    }

    body += '\n';
    return body;
}

} // namespace

namespace cli {

void root_messages()
{
    const std::string name = std::string(PROJECT_NAME);
    const std::string version = std::string(PROJECT_VERSION);

    std::string body = "\033[1mNAME:\033[0m\n";
    body += fmt::format("{}\033[4m{} v{}\033[0m\n\n", ws_2, name, version);

    std::string description = "A command line program for interactively querying OpenAI via the OpenAI API.\n";
    description += ws_2 + "See \033[4mhttps://github.com/dsw7/GPTifier\033[0m for more information.";

    body += add_description(description);
    body += add_synopsis("[-v | --version] [-h | --help] [run] [models] [embed]");

    str_pair options = {};
    options.push_back({ "-h, --help", "Print help information and exit" });
    options.push_back({ "-v, --version", "Print version and exit" });
    body += add_options(options);

    str_pair commands = {};
    commands.push_back({ "run", "Run a query against an appropriate model" });
    commands.push_back(
        { "short", "Run a query against an appropriate model but with no threading and limited verbosity" });
    commands.push_back({ "models", "List available OpenAI models" });
    commands.push_back({ "embed", "Get embedding representing a block of text" });
    commands.push_back({ "files", "Manage files uploaded to OpenAI" });
    body += add_commands(commands);

    std::cout << body;
}

void command_run()
{
    std::string body = add_description("Create a chat completion.");
    body += add_synopsis("run <options>");

    str_pair options = {};
    options.push_back({ "-h, --help", "Print help information and exit" });
    options.push_back({ "-m <model-name>, --model=<model-name>", "Specify a valid chat model" });
    options.push_back({ "-u , --no-interactive-export", "Disable [y/n] prompt that asks whether to export results" });
    options.push_back({ "-d <json-file>, --dump=<json-file>", "Export results to a JSON file" });
    options.push_back({ "-p <prompt>, --prompt=<prompt>", "Provide prompt via command line" });
    options.push_back({ "-r <filename>, --read-from-file=<filename>", "Read prompt from a custom file" });
    options.push_back({ "-t <temp>, --temperature=<temperature>", "Provide a sampling temperature between 0 and 2" });
    body += add_options(options);

    str_pair examples = {};
    examples.push_back({ "Run an interactive session", "gpt run" });
    examples.push_back({ "Run a query non-interactively and export results",
        "gpt run --prompt=\"What is 3 + 5\" --dump=\"/tmp/results.json\"" });

    body += add_examples(examples);
    std::cout << body;
}

void command_short()
{
    std::string body = add_description("Create a chat completion but without threading or verbosity.");
    body += add_synopsis("short <options>");

    str_pair options = {};
    options.push_back({ "-h, --help", "Print help information and exit" });
    options.push_back({ "-p <prompt>, --prompt=<prompt>", "Provide prompt via command line" });
    body += add_options(options);

    str_pair examples = {};
    examples.push_back({ "Create a chat completion", "gpt short --prompt=\"What is 2 + 2?\"" });
    body += add_examples(examples);
    std::cout << body;
}

void command_models()
{
    std::string body = add_description("List available OpenAI models.");

    body += add_synopsis("models [-h | --help]");
    str_pair options = {};

    options.push_back({ "-h, --help", "Print help information and exit" });
    body += add_options(options);

    std::cout << body;
}

void command_embed()
{
    std::string body = add_description("Get embedding representing a block of text.");
    body += add_synopsis("embed <options>");

    str_pair options = {};
    options.push_back({ "-h, --help", "Print help information and exit" });
    options.push_back({ "-m <model-name>, --model=<model-name>", "Specify a valid embedding model" });
    options.push_back({ "-i <text>, --input=<text>", "Input text to embed" });
    options.push_back({ "-r <filename>, --read-from-file=<filename>", "Read input text to embed from a file" });
    body += add_options(options);

    std::cout << body;
}

void command_files()
{
    std::string body = add_description("Manage files uploaded to OpenAI.");
    body += add_synopsis("files <subcommands>");

    str_pair options = {};
    options.push_back({ "-h, --help", "Print help information and exit" });
    body += add_options(options);

    str_pair commands = {};
    commands.push_back({ "list", "List uploaded files" });
    body += add_commands(commands);

    std::cout << body;
}

} // namespace cli
