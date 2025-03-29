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
#include <stdexcept>
#include <string>
#include <vector>

namespace {

struct Params {
    bool debug = false;
    std::optional<std::string> instructions_file = std::nullopt;
    std::optional<std::string> model = std::nullopt;
    std::vector<std::string> files;
};

void read_cli(int argc, char **argv, Params &params)
{
    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "debug", no_argument, 0, 'd' },
            { "model", required_argument, 0, 'm' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int opt = getopt_long(argc, argv, "hdm:", long_options, &option_index);

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
            default:
                cli::exit_on_failure();
        }
    }

    bool prompt_set = false;

    for (int i = optind; i < argc; i++) {
        if (strcmp("edit", argv[i]) != 0) {
            if (prompt_set) {
                params.files.push_back(argv[i]);
            } else {
                params.instructions_file = argv[i];
                prompt_set = true;
            }
        }
    }
}

std::string build_prompt(const std::string &instructions, const std::string &input_code)
{
    return fmt::format("I am editing some code. Apply the following instructions:\n"
                       "```\n{}\n```\n"
                       "To the following code:\n"
                       "```\n{}\n```\n"
                       "And return the answer in the following format: "
                       "<code>(the updated code here)</code>\n",
        instructions, input_code);
}

void apply_transformation(const Params &params)
{
    std::string instructions = utils::read_from_file(params.instructions_file.value());
    std::string input_code = utils::read_from_file(params.files[0]);
    std::string prompt = build_prompt(instructions, input_code);

    if (params.debug) {
        fmt::print(prompt);
        return;
    }

    std::string model;
    if (params.model) {
        model = params.model.value();
    } else {
        model = select_chat_model();
    }

    // ChatCompletion cc = create_chat_completion(prompt, model, 1.00, false);
}

} // namespace

void command_edit(int argc, char **argv)
{
    Params params;
    read_cli(argc, argv, params);

    if (not params.instructions_file) {
        throw std::runtime_error("No prompt file provided. Cannot proceed");
    }

    if (params.files.size() == 0) {
        throw std::runtime_error("No files to edit provided");
    }

    apply_transformation(params);
}
