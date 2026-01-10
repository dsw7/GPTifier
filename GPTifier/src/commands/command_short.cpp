#include "command_short.hpp"

#include "configs.hpp"
#include "ollama_generate.hpp"
#include "responses.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <getopt.h>
#include <optional>
#include <string>

namespace {

const float MIN_TEMP = 0.00;
const float MAX_TEMP = 2.00;

void help_short()
{
    const std::string messages = R"(Create a response but without threading or verbosity. Command is
useful for unit testing, vim integration, etc.

Usage:
  gpt short [OPTIONS] PROMPT

Options:
  -h, --help                     Print help information and exit
  -j, --json                     Print raw JSON response from OpenAI
  -l, --use-local                Connect to locally hosted LLM as opposed to OpenAI
  -t, --temperature=TEMPERATURE  Provide a sampling temperature between 0 and 2. Note that
                                 temperature will be clamped between 0 and 2

Examples:
  > Create a chat completion:
    $ gpt short "What is 2 + 2?"
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    bool print_raw_json = false;
    bool use_local = false;
    std::optional<std::string> prompt;
    std::optional<std::string> temperature;
};

Parameters read_cli(int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "temperature", required_argument, 0, 't' },
            { "use-local", no_argument, 0, 'l' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hjt:l", long_options, &option_index);

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
            case 't':
                params.temperature = optarg;
                break;
            case 'l':
                params.use_local = true;
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

float get_temperature(const std::optional<std::string> &temperature)
{
    float temp_f = 1.00;

    if (temperature) {
        if (temperature.value().empty()) {
            throw std::runtime_error("Empty temperature");
        }

        temp_f = utils::string_to_float(temperature.value());
    }

    return std::clamp(temp_f, MIN_TEMP, MAX_TEMP);
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

    if (params.prompt.value().empty()) {
        throw std::runtime_error("Prompt is empty");
    }

    const float temperature = get_temperature(params.temperature);
    const std::string prompt = params.prompt.value();

    std::string raw_response;
    std::string output;

    if (params.use_local) {
        const std::string model = "gemma3:latest"; // temporary
        const serialization::OllamaResponse response = serialization::create_ollama_response(prompt, model);
        output = response.output;
        raw_response = response.raw_response;
    } else {
        const std::string model = get_model();
        const serialization::Response response = serialization::create_response(prompt, model, temperature);
        output = response.output;
        raw_response = response.raw_response;
    }

    if (params.print_raw_json) {
        fmt::print("{}\n", raw_response);
        return;
    }

    fmt::print("{}\n", output);
}

} // namespace commands
