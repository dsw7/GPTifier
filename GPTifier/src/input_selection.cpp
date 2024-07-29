#include "input_selection.hpp"

#include "utils.hpp"

#include <iostream>
#include <stdexcept>

void load_input_text(std::string &input, const std::string &input_file)
{
    // Input text was passed via file
    if (not input_file.empty())
    {
        ::read_text_from_file(input_file, input);
        return;
    }

    // Otherwise default to reading the input text from stdin
    std::cout << "\033[1mInput:\033[0m ";
    std::getline(std::cin, input);

    // If still empty then we cannot proceed
    if (input.empty())
    {
        throw std::runtime_error("No input text provided anywhere. Cannot proceed");
    }
}
