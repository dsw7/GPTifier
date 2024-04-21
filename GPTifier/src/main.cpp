#include "commands.hpp"
#include "params.hpp"

#include <iostream>
#include <stdexcept>

::Params params;

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

    params.load_params_from_command_line(argc, argv);

    try
    {
        ::create_chat_completion();
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
