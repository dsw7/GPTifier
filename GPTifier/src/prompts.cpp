#include "prompts.hpp"
#include "params.hpp"
#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace prompt
{

void read_prompt_from_file()
{
    ::print_separator();
    std::cout << "Reading prompt from file: " + ::params.prompt_file + '\n';

    std::ifstream file(::params.prompt_file);

    if (not file.is_open())
    {
        throw std::runtime_error("Could not open file '" + ::params.prompt_file + "'");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    ::params.prompt = buffer.str();

    file.close();
}

void read_prompt_interactively()
{
    ::print_separator();
    std::cout << "\033[1mInput:\033[0m ";

    std::getline(std::cin, ::params.prompt);
}

void get_prompt()
{
    // Prompt was passed via command line
    if (not ::params.prompt.empty())
    {
        return;
    }

    // Prompt was passed via file
    if (not ::params.prompt_file.empty())
    {
        read_prompt_from_file();
        return;
    }

    // Otherwise default to reading from stdin
    read_prompt_interactively();

    // If still empty then we cannot proceed
    if (::params.prompt.empty())
    {
        throw std::runtime_error("Prompt cannot be empty");
    }
}

} // namespace prompt
