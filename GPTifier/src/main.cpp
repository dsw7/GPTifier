#include "command_embed.hpp"
#include "command_models.hpp"
#include "command_run.hpp"
#include "command_short.hpp"
#include "configs.hpp"
#include "help_messages.hpp"
#include "json.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

Configs configs;

void print_build_information()
{
    nlohmann::json data = {};

    data["build_date"] = BUILD_DATE;
    data["version"] = PROJECT_VERSION;

    std::cout << data.dump(2) << '\n';
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        cli::root_messages();
        return EXIT_FAILURE;
    }

    const std::string command = argv[1];

    if (command == "-h" or command == "--help") {
        cli::root_messages();
        return EXIT_SUCCESS;
    }

    if (command == "-v" or command == "--version") {
        print_build_information();
        return EXIT_SUCCESS;
    }

    try {
        configs.load_configs_from_config_file();
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << '\n';
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
        } else {
            std::cerr << "Received unknown command. Re-run with -h or --help\n";
        }
    } catch (std::runtime_error &e) {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
