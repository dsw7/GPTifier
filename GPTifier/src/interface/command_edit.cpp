#include "interface/command_edit.hpp"

#include "interface/help_messages.hpp"
#include "selectors.hpp"
#include "serialization/chat_completions.hpp"
#include "utils.hpp"

#include <cstring>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Params {
    bool debug = false;
    std::optional<std::string> input_file = std::nullopt;
    std::optional<std::string> instructions_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> output_file = std::nullopt;
};

void read_cli(int argc, char **argv, Params &params)
{
    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "debug", no_argument, 0, 'd' },
            { "model", required_argument, 0, 'm' },
            { "output", required_argument, 0, 'o' },
            { "prompt", required_argument, 0, 'p' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int opt = getopt_long(argc, argv, "hdm:o:p:", long_options, &option_index);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'h':
                cli::help_command_edit();
                exit(EXIT_SUCCESS);
            case 'd':
                params.debug = true;
                break;
            case 'm':
                params.model = optarg;
                break;
            case 'o':
                params.output_file = optarg;
                break;
            case 'p':
                params.instructions_file = optarg;
                break;
            default:
                cli::exit_on_failure();
        }
    }

    for (int i = optind; i < argc; i++) {
        if (strcmp("edit", argv[i]) != 0) {
            params.input_file = argv[i];
            break;
        }
    }
}

std::string build_prompt(const std::string &instructions, const std::string &input_code)
{
    std::string prompt;
    prompt += "I am editing some code. Apply the following instructions:\n";
    prompt += fmt::format("```\n{}\n```\n", instructions);
    prompt += fmt::format("To the following code:\n```\n{}\n```\n", input_code);
    prompt += "And return the answer formatted as follows: ```(the updated code here)```";
    return prompt;
}

void print_debug(const std::string &prompt, const std::string &completion)
{
    print_sep();
    fmt::print("The prompt was:\n");
    fmt::print(fg(blue), "{}\n", prompt);

    print_sep();
    fmt::print("The completion was:\n");
    fmt::print(fg(green), "{}\n", completion);
}

std::optional<std::string> get_output_from_completion(const std::string &completion)
{
    std::stringstream ss(completion);
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(ss, line)) {
        lines.push_back(line);
    }

    int num_lines = lines.size();

    if (num_lines <= 2) {
        return std::nullopt;
    }

    std::string output_code;

    for (int i = 1; i < num_lines - 1; ++i) {
        output_code += lines[i] + "\n";
    }

    if (!output_code.empty()) {
        output_code.pop_back();
    }

    return output_code;
}

void print_code_to_stdout(const std::optional<std::string> &output_code)
{
    if (output_code) {
        fmt::print("{}\n", output_code.value());
    } else {
        fmt::print(stderr, "Could not parse code from completion. Try again\n");
    }
}

void apply_transformation(const Params &params)
{
    const std::string instructions = utils::read_from_file(params.instructions_file.value());

    fmt::print("Read instructions from: {}\n", params.instructions_file.value());
    std::cout << fmt::format("{} -> ", params.input_file.value()) << std::flush;

    const std::string input_code = utils::read_from_file(params.input_file.value());
    const std::string prompt = build_prompt(instructions, input_code);

    std::string model;
    if (params.model) {
        model = params.model.value();
    } else {
        model = select_chat_model();
    }

    ChatCompletion cc;
    try {
        cc = create_chat_completion(prompt, model, 1.00, false);
    } catch (std::runtime_error &e) {
        fmt::print("?\n");
        throw std::runtime_error(e.what());
    }

    fmt::print("{}\nComplete!\n", params.output_file.value());

    if (params.debug) {
        print_debug(prompt, cc.completion);
        return;
    }

    const std::optional<std::string> output_code = get_output_from_completion(cc.completion);

    if (params.output_file) {
        utils::write_to_file(params.output_file.value(), output_code.value());
    } else {
        print_code_to_stdout(output_code);
    }
}

} // namespace

void command_edit(int argc, char **argv)
{
    Params params;
    read_cli(argc, argv, params);

    if (not params.instructions_file) {
        throw std::runtime_error("No prompt file provided. Cannot proceed");
    }

    if (not params.input_file) {
        throw std::runtime_error("No file to edit provided. Cannot proceed");
    }

    apply_transformation(params);
}
