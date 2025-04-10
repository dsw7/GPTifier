#include "interface/command_edit.hpp"

#include "interface/help_messages.hpp"
#include "interface/model_selector.hpp"
#include "serialization/chat_completions.hpp"
#include "utils.hpp"

#include <cstring>
#include <fmt/core.h>
#include <getopt.h>
#include <iostream>
#include <json.hpp>
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
    prompt += fmt::format("```plaintext\n{}\n```\n", instructions);
    prompt += fmt::format("To the following code:\n```\n{}\n```\n", input_code);
    prompt += ("Please return the code edits in a JSON format with keys \"code\" and \"description.\" "
               "For example:\n"
               "{\n"
               "  \"code\": \"Your updated code here\",\n"
               "  \"description\": \"A brief explanation of the changes\",\n"
               "}\n");
    return prompt;
}

void print_debug(const std::string &prompt, const std::string &completion)
{
    utils::separator();
    fmt::print("The prompt was:\n");
    fmt::print(fg(blue), "{}", prompt);

    utils::separator();
    fmt::print("The completion was:\n");
    fmt::print(fg(green), "{}\n", completion);
}

struct Output {
    std::string code;
    std::string description;
};

void get_output_from_completion(const std::string &completion, Output &output)
{
    std::stringstream ss(completion);
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(ss, line)) {
        lines.push_back(line);
    }

    if (lines.size() <= 2) {
        throw std::runtime_error("Could not get lines from completion");
    }

    bool append_enabled = false;
    std::string raw_json;

    for (const auto &line: lines) {
        if (line == "```json") {
            append_enabled = true;
        } else if (line == "```") {
            append_enabled = false;
        } else {
            if (append_enabled) {
                raw_json += line;
            }
        }
    }

    if (append_enabled) {
        throw std::runtime_error("Closing triple backticks not found. Raw JSON might be malformed");
    }

    if (raw_json.empty()) {
        throw std::runtime_error("JSON with code and description is empty. Cannot proceed");
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(raw_json);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse JSON: {}", e.what()));
    }

    output.code = json["code"];
    output.description = json["description"];
}

void print_code_to_stdout(const Output &output)
{
    utils::separator();
    fmt::print("The updated code is:\n");
    fmt::print(fg(green), "```\n{}\n```\n", output.code);

    utils::separator();
    fmt::print("A description of the changes:\n");
    fmt::print(fg(green), "{}\n", output.description);
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

    if (params.output_file) {
        fmt::print("{}\nComplete!\n", params.output_file.value());
    } else {
        fmt::print("stdout\n");
    }

    if (params.debug) {
        print_debug(prompt, cc.completion);
        return;
    }

    Output output;
    get_output_from_completion(cc.completion, output);

    if (params.output_file) {
        utils::write_to_file(params.output_file.value(), output.code);
    } else {
        print_code_to_stdout(output);
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
