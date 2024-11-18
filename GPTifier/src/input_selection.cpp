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

std::optional<std::string> get_text_from_inputfile()
{
    static std::filesystem::path inputfile = std::filesystem::current_path() / "Inputfile";

    if (not std::filesystem::exists(inputfile)) {
        return std::nullopt;
    }

    if (std::filesystem::is_empty(inputfile)) {
        return std::nullopt;
    }

    testing::log_test("Loaded input text from Inputfile");
    std::cout << "Found an Inputfile in current working directory!\n";

    std::string text = read_text_from_file(inputfile);

    if (text.empty()) {
        return std::nullopt;
    }

    return text;
}

std::optional<std::string> get_text_from_stdin()
{
    std::cout << "\033[1mInput:\033[0m ";
    std::string text;

    std::getline(std::cin, text);
    testing::log_test("Loaded input text from stdin");

    if (text.empty()) {
        return std::nullopt;
    }

    return text;
}

} // namespace

std::string load_input_text(const std::optional<std::string> &input_file)
{
    if (input_file.has_value()) {
        return get_text_from_cli_specified_file(input_file.value());
    }

    std::optional<std::string> text_from_inputfile = get_text_from_inputfile();

    if (text_from_inputfile.has_value()) {
        return text_from_inputfile.value();
    }

    std::optional<std::string> text_from_stdin = get_text_from_stdin();

    if (text_from_stdin.has_value()) {
        return text_from_stdin.value();
    }

    throw std::runtime_error("No input text provided anywhere. Cannot proceed");
}
