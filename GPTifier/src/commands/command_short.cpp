#include "command_short.hpp"

#include "chat_completions.hpp"
#include "configs.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <optional>
#include <string>

namespace {

void help_short()
{
    const std::string messages = R"(Create a chat completion but without threading or verbosity. Command is
useful for unit testing, vim integration, etc.

Usage:
  gpt short [OPTIONS] PROMPT

Options:
  -h, --help                     Print help information and exit
  -j, --json                     Print raw JSON response from OpenAI
  -t, --temperature=TEMPERATURE  Provide a sampling temperature between 0 and 2
  -s, --store-completion         Store results of completion on OpenAI servers

Examples:
  > Create a chat completion:
    $ gpt short "What is 2 + 2?"
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    bool print_raw_json = false;
    bool store_completion = false;
    std::optional<std::string> prompt = std::nullopt;
    std::optional<std::string> temperature = std::nullopt;
};

Parameters read_cli(int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "store-completion", no_argument, 0, 's' },
            { "temperature", required_argument, 0, 't' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hjst:", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_short();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 's':
                params.store_completion = true;
                break;
            case 't':
                params.temperature = optarg;
                break;
            default:
                utils::exit_on_failure();
        }
    }

    for (int i = optind; i < argc; i++) {
        if (strcmp("short", argv[i]) != 0) {
            params.prompt = argv[i];
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
    if (configs.model_short) {
        return configs.model_short.value();
    }
    throw std::runtime_error("Could not determine which model to use");
#endif
}

} // namespace

namespace commands {

void command_short(int argc, char **argv)
{
    const Parameters params = read_cli(argc, argv);

    if (not params.prompt) {
        throw std::runtime_error("Prompt is empty");
    }

    float temperature = 1.00;
    if (params.temperature) {
        temperature = utils::string_to_float(params.temperature.value());
    }

    if (temperature < 0 || temperature > 2) {
        throw std::runtime_error("Temperature must be between 0 and 2");
    }

    const std::string model = get_model();
    const serialization::ChatCompletion cc = serialization::create_chat_completion(
        params.prompt.value(), model, temperature, params.store_completion);

    if (params.print_raw_json) {
        fmt::print("{}\n", cc.raw_response);
        return;
    }

    fmt::print("{}\n", cc.completion);
}

} // namespace commands
