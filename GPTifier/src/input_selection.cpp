#include "input_selection.hpp"

#include "utils.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

void get_text_from_cli_specified_file(std::string &text, const std::string &filename)
{
    ::read_text_from_file(filename, text);
}

void get_text_from_inputfile(std::string &text)
{
    static std::filesystem::path inputfile = std::filesystem::current_path() / "Inputfile";

    if (not std::filesystem::exists(inputfile))
    {
        return;
    }

    if (std::filesystem::is_empty(inputfile))
    {
        return;
    }

    std::cout << "Found an Inputfile in current working directory!\n";
    ::read_text_from_file(inputfile, text);
}

void get_text_from_stdin(std::string &text)
{
    std::cout << "\033[1mInput:\033[0m ";
    std::getline(std::cin, text);
}

void load_input_text(std::string &input, const std::string &input_file)
{
    if (not input_file.empty())
    {
        ::get_text_from_cli_specified_file(input, input_file);
        return;
    }

    ::get_text_from_inputfile(input);

    if (not input.empty())
    {
        return;
    }

    ::get_text_from_stdin(input);

    if (input.empty())
    {
        throw std::runtime_error("No input text provided anywhere. Cannot proceed");
    }
}
