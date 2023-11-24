#include "prompts.h"
#include "utils.h"

#include <iostream>

namespace prompt
{

void read_prompt_interactively(std::string &prompt)
{
    utils::print_separator();
    std::cout << "\033[1mInput:\033[0m ";

    std::getline(std::cin, prompt);
}

void get_prompt(Params &params)
{
    // Prompt was passed via command line
    if (not params.prompt.empty())
    {
        return;
    }

    // Otherwise default to reading from stdin
    read_prompt_interactively(params.prompt);
}

} // namespace prompt
