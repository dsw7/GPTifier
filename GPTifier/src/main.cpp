#include "command_models.hpp"
#include "command_run.hpp"
#include "help_messages.hpp"
#include "params.hpp"

#include <iostream>
#include <json.hpp>
#include <stdexcept>

::Params params;

void print_build_information()
{
    nlohmann::json data = {};

    data["build_date"] = BUILD_DATE;
    data["version"] = PROJECT_VERSION;

    std::cout << data.dump(2) << '\n';
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        ::print_help_messages();
        return EXIT_FAILURE;
    }

    std::string command = std::string(argv[1]);

    if (command.compare("-h") == 0 or command.compare("--help") == 0)
    {
        ::print_help_messages();
        return EXIT_SUCCESS;
    }

    if (command.compare("-v") == 0 or command.compare("--version") == 0)
    {
        ::print_build_information();
        return EXIT_SUCCESS;
    }

    try
    {
        params.load_params_from_config_file();
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    try
    {
        if (command.compare("run") == 0)
        {
            params.load_params_from_command_line(argc, argv);
            ::command_run();
        }
        else if (command.compare("models") == 0)
        {
            ::command_models();
        }
        else
        {
            std::cerr << "Received unknown command: \"" + command + "\"\n";
        }
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
