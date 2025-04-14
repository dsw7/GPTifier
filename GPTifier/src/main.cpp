#include "configs.hpp"
#include "interface/command_chats.hpp"
#include "interface/command_costs.hpp"
#include "interface/command_edit.hpp"
#include "interface/command_embed.hpp"
#include "interface/command_files.hpp"
#include "interface/command_fine_tune.hpp"
#include "interface/command_models.hpp"
#include "interface/command_run.hpp"
#include "interface/command_short.hpp"
#include "interface/command_test.hpp"
#include "interface/help_messages.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

void print_help_messages()
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

void print_build_information()
{
    nlohmann::json data;

    data["build_date"] = BUILD_DATE;
    data["version"] = PROJECT_VERSION;

#ifdef TESTING_ENABLED
    data["build_type"] = "Testing";
#else
    data["build_type"] = "Production";
#endif

    fmt::print("{}\n", data.dump(2));
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        print_help_messages();
        return EXIT_FAILURE;
    }

    const std::string command = argv[1];

    if (command == "-h" or command == "--help") {
        print_help_messages();
        return EXIT_SUCCESS;
    }

    if (command == "-v" or command == "--version") {
        print_build_information();
        return EXIT_SUCCESS;
    }

    try {
        configs.load_configs_from_config_file();
    } catch (std::runtime_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }

    try {
        if (command == "run") {
            command_run(argc, argv);
        } else if (command == "short") {
            command_short(argc, argv);
        } else if (command == "models") {
            command_models(argc, argv);
        } else if (command == "embed") {
            command_embed(argc, argv);
        } else if (command == "files") {
            command_files(argc, argv);
        } else if (command == "fine-tune") {
            command_fine_tune(argc, argv);
        } else if (command == "costs") {
            command_costs(argc, argv);
        } else if (command == "chats") {
            command_chats(argc, argv);
        } else if (command == "edit") {
            command_edit(argc, argv);
        } else if (command == "test") {
            command_test(argc, argv);
        } else {
            throw std::runtime_error("Received unknown command. Re-run with -h or --help");
        }
    } catch (std::runtime_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
