#include "command_chats.hpp"
#include "command_costs.hpp"
#include "command_edit.hpp"
#include "command_embed.hpp"
#include "command_files.hpp"
#include "command_fine_tune.hpp"
#include "command_models.hpp"
#include "command_run.hpp"
#include "command_short.hpp"
#include "command_test.hpp"
#include "configs.hpp"
#include "help_messages.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

void print_help_messages()
{
    help::print_program_info();

    help::HelpMessages help;
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
            commands::command_run(argc, argv);
        } else if (command == "short") {
            commands::command_short(argc, argv);
        } else if (command == "models") {
            commands::command_models(argc, argv);
        } else if (command == "embed") {
            commands::command_embed(argc, argv);
        } else if (command == "files") {
            commands::command_files(argc, argv);
        } else if (command == "fine-tune") {
            commands::command_fine_tune(argc, argv);
        } else if (command == "costs") {
            commands::command_costs(argc, argv);
        } else if (command == "chats") {
            commands::command_chats(argc, argv);
        } else if (command == "edit") {
            commands::command_edit(argc, argv);
        } else if (command == "test") {
            commands::command_test(argc, argv);
        } else {
            throw std::runtime_error("Received unknown command. Re-run with -h or --help");
        }
    } catch (std::runtime_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
