#include "command_edit.hpp"

#include "chat_completions.hpp"
#include "configs.hpp"
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

void help_edit()
{
    fmt::print("Edit code according to rules provided by file or command line option.\n");
}

struct Parameters {
    bool debug = false;
    std::optional<std::string> input_file = std::nullopt;
    std::optional<std::string> instructions_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::optional<std::string> output_file = std::nullopt;
    std::optional<std::string> rule = std::nullopt;
};

Parameters read_cli(int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "debug", no_argument, 0, 'd' },
            { "model", required_argument, 0, 'm' },
            { "output", required_argument, 0, 'o' },
            { "instructions", required_argument, 0, 'i' },
            { "rule", required_argument, 0, 'r' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int opt = getopt_long(argc, argv, "hdm:o:i:r:", long_options, &option_index);

        if (opt == -1) {
            break;
        }

        switch (opt) {
            case 'h':
                help_edit();
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
            case 'i':
                params.instructions_file = optarg;
                break;
            case 'r':
                params.rule = optarg;
                break;
            default:
                utils::exit_on_failure();
        }
    }

    for (int i = optind; i < argc; i++) {
        if (strcmp("edit", argv[i]) != 0) {
            params.input_file = argv[i];
            break;
        }
    }

    return params;
}

std::string get_model()
{
#ifdef TESTING_ENABLED
    static std::string low_cost_model = "gpt-3.5-turbo";
    return low_cost_model;
#else
    if (configs.model_edit) {
        return configs.model_edit.value();
    }
    throw std::runtime_error("Could not determine which model to use");
#endif
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

std::string get_stringified_json_from_completion(const std::string &completion)
{
    /*
     * Some models return a JSON completion without triple backticks:
     * {
     *   k: v,
     * }
     * Others return a JSON completion with triple backticks:
     * ```json
     * {
     *   k: v,
     * }
     * ```
     */

    if (completion[0] == '{' and completion.back() == '}') {
        return completion;
    }

    bool append_enabled = false;
    std::string line;
    std::stringstream ss(completion);
    std::string raw_json;

    while (std::getline(ss, line)) {
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

    return raw_json;
}

struct Output {
    std::string code;
    std::string description;
};

Output get_output_from_completion(const std::string &completion)
{
    const std::string raw_json = get_stringified_json_from_completion(completion);

    if (raw_json.empty()) {
        throw std::runtime_error("JSON with code and description is empty. Cannot proceed");
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(raw_json);
    } catch (const nlohmann::json::parse_error &e) {
        throw std::runtime_error(fmt::format("Failed to parse JSON: {}", e.what()));
    }

    Output output;
    output.code = json["code"];
    output.description = json["description"];
    return output;
}

void write_to_stdout(const Output &output)
{
    utils::separator();
    fmt::print("The updated code is:\n");
    fmt::print(fg(green), "```\n{}\n```\n", output.code);

    utils::separator();
    fmt::print("A description of the changes:\n");
    fmt::print(fg(green), "{}\n", output.description);
}

void write_to_file(const Output &output, const std::string &filename)
{
    utils::write_to_file(filename, output.code);
    utils::separator();

    fmt::print("A description of the changes:\n");
    fmt::print(fg(green), "{}\n", output.description);
}

void apply_transformation(const Parameters &params)
{
    std::string instructions;

    if (params.rule) {
        instructions = params.rule.value();
        fmt::print("Read instructions from CLI\n");
    } else {
        instructions = utils::read_from_file(params.instructions_file.value());
        fmt::print("Read instructions from: {}\n", params.instructions_file.value());
    }

    std::cout << fmt::format("{} -> ", params.input_file.value()) << std::flush;

    std::string model;
    if (params.model) {
        model = params.model.value();
    } else {
        model = get_model();
    }

    const std::string input_code = utils::read_from_file(params.input_file.value());
    const std::string prompt = build_prompt(instructions, input_code);

    serialization::ChatCompletion cc;
    try {
        cc = serialization::create_chat_completion(prompt, model, 1.00, false);
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

    const Output output = get_output_from_completion(cc.completion);

    if (params.output_file) {
        write_to_file(output, params.output_file.value());
    } else {
        write_to_stdout(output);
    }
}

} // namespace

namespace commands {

void command_edit(int argc, char **argv)
{
    const Parameters params = read_cli(argc, argv);

    if (not params.instructions_file and not params.rule) {
        throw std::runtime_error("No instructions file or rule provided. Cannot proceed");
    }

    if (not params.input_file) {
        throw std::runtime_error("No file to edit provided. Cannot proceed");
    }

    apply_transformation(params);
}

} // namespace commands
