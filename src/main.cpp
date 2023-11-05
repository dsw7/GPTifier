#include "configs.h"
#include "presentation.h"
#include "query.h"

#include <iostream>
#include <stdexcept>
#include <string>

void read_prompt(std::string &prompt)
{
    presentation::print_separator();
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

    std::string reply;

    try
    {
        QueryHandler q(configs);
        q.build_payload(prompt);
        q.run_query(reply);
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    presentation::print_results(reply);
    return EXIT_SUCCESS;
}
