#include "command_chats.hpp"
#include "command_costs.hpp"
#include "command_embed.hpp"
#include "command_files.hpp"
#include "command_fine_tune.hpp"
#include "command_models.hpp"
#include "command_run.hpp"
#include "command_short.hpp"
#include "configs.hpp"
#include "help_messages.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>
#include <string>

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
        cli::help_root_messages();
        return EXIT_FAILURE;
    }

    const std::string command = argv[1];

    if (command == "-h" or command == "--help") {
        cli::help_root_messages();
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
        } else {
            fmt::print(stderr, "Received unknown command. Re-run with -h or --help\n");
        }
    } catch (std::runtime_error &e) {
        fmt::print(stderr, "{}\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
