#include "selectors.hpp"

#include "configs.hpp"
#include "utils.hpp"

#include <filesystem>
#include <fmt/color.h>
#include <fmt/core.h>
#include <iostream>
#include <stdexcept>

namespace {

std::string get_text_from_cli_specified_file(const std::string &filename)
{
#ifdef TESTING_ENABLED
    fmt::print("[TEST] Loaded input text from custom file\n");
#endif
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

#ifdef TESTING_ENABLED
    fmt::print("[TEST] Loaded input text from Inputfile\n");
#endif
    fmt::print("Found an Inputfile in current working directory!\n");

    const std::string text = read_text_from_file(inputfile);

    if (text.empty()) {
        return std::nullopt;
    }

    return text;
}

std::optional<std::string> get_text_from_stdin()
{
    fmt::print(fg(white), "Input: ");
    std::string text;

    std::getline(std::cin, text);
#ifdef TESTING_ENABLED
    fmt::print("[TEST] Loaded input text from stdin\n");
#endif

    if (text.empty()) {
        return std::nullopt;
    }

    return text;
}

} // namespace

std::string select_chat_model()
{
#ifdef TESTING_ENABLED
    static std::string low_cost_model = "gpt-3.5-turbo";
    return low_cost_model;
#endif

    if (configs.chat.model.has_value()) {
        return configs.chat.model.value();
    }

    throw std::runtime_error("Could not determine which model to use");
}

std::string select_input_text(const std::optional<std::string> &input_file)
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
