#include "cli.h"
#include "configs.h"
#include "query.h"
#include "utils.h"

#include <iostream>
#include <stdexcept>
#include <string>

int main(int argc, char** argv)
{
    Configs configs;

    cli::parse_cli(argc, argv);

    try
    {
        ::read_configs(configs);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    utils::print_separator();

    std::string prompt;
    std::cout << "\033[1mInput:\033[0m ";
    std::getline(std::cin, prompt);

    if (prompt.empty())
    {
        std::cerr << "Prompt cannot be empty" << std::endl;
        return EXIT_FAILURE;
    }

    try
    {
        QueryHandler q(configs);
        q.build_payload(prompt);
        q.print_payload();
        q.run_query();
        q.print_response();
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
