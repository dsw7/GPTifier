#include "commands.hpp"
#include "params.hpp"

#include <iostream>
#include <stdexcept>

::Params params;

int main(int argc, char **argv)
{
    if (argc > 1)
    {
        params.load_params_from_command_line(argc, argv);
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
        ::create_chat_completion();
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
