#include "cli.h"
#include "configs.h"
#include "query.h"
#include "utils.h"

#include <iostream>
#include <stdexcept>
#include <string>

void read_input(std::string &prompt)
{
    utils::print_separator();
    std::cout << "\033[1mInput:\033[0m ";

    std::getline(std::cin, prompt);
}

int main(int argc, char **argv)
{
    Params params;
    if (argc > 1)
    {
        cli::parse_cli(argc, argv, params);
    }

    Configs configs;
    try
    {
        ::read_configs(configs);
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (params.prompt.empty())
    {
        read_input(params.prompt);
    }

    if (params.prompt.empty())
    {
        std::cerr << "Prompt cannot be empty" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        QueryHandler q(configs);
        q.build_payload(params.prompt);
        q.print_payload();
        q.run_query();
        q.print_response();

        if (not params.dump.empty())
        {
            q.dump_response(params.dump);
        }
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << std::endl;
    return EXIT_SUCCESS;
}
