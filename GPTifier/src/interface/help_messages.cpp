#include "interface/help_messages.hpp"

#include "utils.hpp"

#include <fmt/core.h>

namespace {

const std::string ws_2 = std::string(2, ' ');
const std::string ws_4 = std::string(4, ' ');

void print_bash_block(const std::string &command)
{
    fmt::print(fg(green), "{0}```bash\n{0}{1}\n{0}```\n", ws_2, command);
}

} // namespace

void HelpMessages::add_command(const std::string &name, const std::string &description)
{
    this->commands.push_back({ description, name });
}

void HelpMessages::add_description(const std::string &line)
{
    this->description.push_back(line);
}

void HelpMessages::add_example(const std::string &description, const std::string &command)
{
    this->examples.push_back({ command, description });
}

void HelpMessages::add_name_version()
{
    this->print_metadata = true;
}

void HelpMessages::add_option(const std::string &opt_short, const std::string &opt_long, const std::string &description)
{
    this->options.push_back({ description, opt_long, opt_short });
}

void HelpMessages::add_synopsis(const std::string &line)
{
    this->synopsis = line;
}

void HelpMessages::print_commands()
{
    if (this->commands.empty()) {
        return;
    }

    fmt::print(fg(white), "Commands:\n");

    for (const auto &it: this->commands) {
        fmt::print(fg(blue), "{}{}\n", ws_2, it.name);
        fmt::print("{} -> {}\n", ws_4, it.description);
    }

    fmt::print("\n{}Try gpt ", ws_2);
    fmt::print(fg(blue), "<subcommand>");
    fmt::print(" [-h | --help] for subcommand specific help.\n\n");
}

void HelpMessages::print_description()
{
    if (this->description.empty()) {
        return;
    }

    fmt::print(fg(white), "Description:\n");

    for (const auto &it: this->description) {
        fmt::print("{}{}\n", ws_2, it);
    }

    fmt::print("\n");
}

void HelpMessages::print_examples()
{
    if (this->examples.empty()) {
        return;
    }

    fmt::print(fg(white), "Examples:\n");

    for (const auto &it: this->examples) {
        fmt::print("{}{}:\n", ws_2, it.description);
        print_bash_block(it.command);
    }

    fmt::print("\n");
}

void HelpMessages::print_name_version()
{
    if (not this->print_metadata) {
        return;
    }

    const std::string name = std::string(PROJECT_NAME);
    const std::string version = std::string(PROJECT_VERSION);

    fmt::print(fg(white), "Name:\n");
    fmt::print("{}", ws_2);
    fmt::print(fmt::emphasis::underline, "{} v{}\n\n", name, version);
}

void HelpMessages::print_options()
{
    if (this->options.empty()) {
        return;
    }

    fmt::print(fg(white), "Options:\n");

    for (const auto &it: this->options) {
        fmt::print(fmt::emphasis::faint, "{}{}, {}\n", ws_2, it.opt_short, it.opt_long);
        fmt::print("{} -> {}\n", ws_4, it.description);
    }

    fmt::print("\n");
}

void HelpMessages::print_synopsis()
{
    if (this->synopsis.empty()) {
        return;
    }

    fmt::print(fg(white), "Synopsis:\n");
    fmt::print("{}gpt {}\n\n", ws_2, this->synopsis);
}

void HelpMessages::print()
{
    this->print_name_version();
    this->print_description();
    this->print_synopsis();
    this->print_options();
    this->print_commands();
    this->print_examples();
}

namespace cli {

void exit_on_failure()
{
    fmt::print(stderr, "Try running with -h or --help for more information\n");
    exit(EXIT_FAILURE);
}

void help_root_messages()
{
    HelpMessages help;
    help.add_name_version();
    help.add_description("A command line program for interactively querying OpenAI via the OpenAI API.");
    help.add_description("See \033[4mhttps://github.com/dsw7/GPTifier\033[0m for more information.");
    help.add_synopsis("[-v | --version] [-h | --help] <command> [<args>]");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-v", "--version", "Print version and exit");
    help.add_command("run", "Run a query against an appropriate model");
    help.add_command("short", "Run a query against an appropriate model but with no threading and limited verbosity");
    help.add_command("models", "List available OpenAI models");
    help.add_command("embed", "Get embedding representing a block of text");
    help.add_command("files", "Manage files uploaded to OpenAI");
    help.add_command("fine-tune", "Manage fine tuning operations");
    help.add_command("costs", "Get OpenAI usage details");
    help.add_command("edit", "Edit one or more files according to a prompt");
    help.print();
}

} // namespace cli
