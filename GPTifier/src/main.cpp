#include "configs.hpp"
#include "help_messages.hpp"
#include "interface/command_chats.hpp"
#include "interface/command_costs.hpp"
#include "interface/command_embed.hpp"
#include "interface/command_files.hpp"
#include "interface/command_fine_tune.hpp"
#include "interface/command_models.hpp"
#include "interface/command_run.hpp"
#include "interface/command_short.hpp"
#include "interface/command_test.hpp"

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
