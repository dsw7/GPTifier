#include "configs.h"
#include "query.h"
#include "utils.h"

#include <iostream>
#include <stdexcept>
#include <string>

void read_prompt(std::string &prompt)
{
    utils::print_separator();
    std::cout << "\033[1mInput:\033[0m ";
    std::getline(std::cin, prompt);
}

int main()
{
    Configs configs;

    try
    {
        ::read_configs(configs);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::string prompt;
    ::read_prompt(prompt);

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
