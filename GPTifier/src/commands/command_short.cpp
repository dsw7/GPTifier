#include "command_short.hpp"

#include "configs.hpp"
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
  -m, --model                    Select model
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
    std::optional<std::string> model;
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
            { "model", required_argument, 0, 'm' },
            { "temperature", required_argument, 0, 't' },
            { "use-local", no_argument, 0, 'l' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hjm:t:l", long_options, &option_index);

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
            case 'm':
                params.model = optarg;
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

void use_ollama(const Parameters &params)
{
    std::string model;

    if (params.model) {
        model = params.model.value();
    } else {
        if (configs.model_short_ollama) {
            model = configs.model_short_ollama.value();
        } else {
            throw std::runtime_error("Could not determine which Ollama model to use");
        }
    }

    const serialization::OllamaResponse response = serialization::create_ollama_response(params.prompt.value(), model);

    if (params.print_raw_json) {
        fmt::print("{}\n", response.raw_response);
        return;
    }

    fmt::print("{}\n", response.output);
}

void use_openai(const Parameters &params)
{
    std::string model;

    if (params.model) {
        model = params.model.value();
    } else {
        model = configs.model_short_openai.value();
    }

    const float temperature = get_temperature(params.temperature);
    const serialization::OpenAIResponse response = serialization::create_openai_response(params.prompt.value(), model, temperature);

    if (params.print_raw_json) {
        fmt::print("{}\n", response.raw_response);
        return;
    }

    fmt::print("{}\n", response.output);
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

    if (params.use_local) {
        use_ollama(params);
    } else {
        use_openai(params);
    }
}

} // namespace commands
