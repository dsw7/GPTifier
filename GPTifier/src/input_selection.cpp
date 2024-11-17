#include "input_selection.hpp"

#include "testing.hpp"
#include "utils.hpp"

#include <filesystem>
#include <iostream>
#include <stdexcept>

namespace {

std::string get_text_from_cli_specified_file(const std::string &filename)
{
    testing::log_test("Loaded input text from custom file");
    return read_text_from_file(filename);
}

std::string get_text_from_inputfile()
{
    static std::filesystem::path inputfile = std::filesystem::current_path() / "Inputfile";

    std::string text;

    if (not std::filesystem::exists(inputfile)) {
        return text;
    }

    if (std::filesystem::is_empty(inputfile)) {
        return text;
    }

    testing::log_test("Loaded input text from Inputfile");
    std::cout << "Found an Inputfile in current working directory!\n";

    text = read_text_from_file(inputfile);
    return text;
}

std::string get_text_from_stdin()
{
    std::cout << "\033[1mInput:\033[0m ";
    std::string text;

    std::getline(std::cin, text);
    testing::log_test("Loaded input text from stdin");

    return text;
}

} // namespace

std::string load_input_text(const std::string &input_file)
{
    if (not input_file.empty()) {
        return get_text_from_cli_specified_file(input_file);
    }

    std::string text_from_inputfile = get_text_from_inputfile();

    if (not text_from_inputfile.empty()) {
        return text_from_inputfile;
    }

    std::string text_from_stdin = get_text_from_stdin();

    if (text_from_stdin.empty()) {
        throw std::runtime_error("No input text provided anywhere. Cannot proceed");
    }

    return text_from_stdin;
}
