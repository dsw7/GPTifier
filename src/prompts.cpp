#include "prompts.hpp"
#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace prompt
{

void read_prompt_interactively(std::string &prompt)
{
    utils::print_separator();
    std::cout << "\033[1mInput:\033[0m ";

    std::getline(std::cin, prompt);
}

void read_prompt_from_file(std::string &prompt, const std::string &filename)
{
    utils::print_separator();
    std::cout << "Reading prompt from file: " + filename + '\n';

    std::ifstream file(filename);

    if (not file.is_open())
    {
        throw std::runtime_error("Could not open file " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    prompt = buffer.str();

    file.close();
}

void get_prompt(Params &params)
{
    // Prompt was passed via command line
    if (not params.prompt.empty())
    {
        return;
    }

    // Prompt was passed via file
    if (not params.prompt_file.empty())
    {
        read_prompt_from_file(params.prompt, params.prompt_file);
        return;
    }

    // Otherwise default to reading from stdin
    read_prompt_interactively(params.prompt);
}

} // namespace prompt
