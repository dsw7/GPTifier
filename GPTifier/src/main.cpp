#include "command_chats.hpp"
#include "command_costs.hpp"
#include "command_edit.hpp"
#include "command_embed.hpp"
#include "command_files.hpp"
#include "command_fine_tune.hpp"
#include "command_img.hpp"
#include "command_models.hpp"
#include "command_run.hpp"
#include "command_short.hpp"
#include "command_test.hpp"
#include "configs.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

void print_help_messages()
{
    const std::string messages = R"(-- Copyright (C) 2023-2025 by David Weber
-- Site: https://github.com/dsw7/GPTifier

A command line OpenAI toolkit.

Usage:
  gpt [OPTIONS] COMMAND [ARGS]...

Options:
  -h, --help     Print help information and exit
  -v, --version  Print version and exit

Commands:
  run            Run a query against an appropriate model
  short          Run a query against an appropriate model but with no threading and limited verbosity
  models         List available OpenAI models
  embed          Get embedding representing a block of text
  files          Manage files uploaded to OpenAI
  fine-tune      Manage fine tuning operations
  costs          Get OpenAI usage details
  chats          Manage chat completions uploaded to OpenAI
  edit           Edit one or more files according to a prompt [DEPRECATED]
  img            Generate an image from a prompt

Try 'gpt <subcommand> [-h | --help]' for subcommand specific help.
The 'edit' subcommand has been moved. See https://github.com/dsw7/FuncGraft for more information.
)";

    fmt::print("-- GPTifier | v{}\n{}", PROJECT_VERSION, messages);
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
            commands::command_edit();
        } else if (command == "test") {
            commands::command_test(argc, argv);
        } else if (command == "img") {
            commands::command_img(argc, argv);
        } else {
            throw std::runtime_error("Received unknown command. Re-run with -h or --help");
        }
    } catch (std::runtime_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
