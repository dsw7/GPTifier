#include "interface/command_short.hpp"

#include "interface/help_messages.hpp"
#include "interface/model_selector.hpp"
#include "serialization/chat_completions.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <optional>
#include <string>

namespace {

void help_short()
{
    help::HelpMessages help;
    help.add_description("Create a chat completion but without threading or verbosity.");
    help.add_synopsis("short [-h | --help] [-j | --json] [-t <temp> | --temperature=<temp>]\n  "
                      "[-s | --store-completion] PROMPT");
    help.add_option("-h", "--help", "Print help information and exit");
    help.add_option("-j", "--json", "Print raw JSON response from OpenAI");
    help.add_option("-t <temp>", "--temperature=<temperature>", "Provide a sampling temperature between 0 and 2");
    help.add_option("-s", "--store-completion", "Store results of completion on OpenAI servers");
    help.add_example("Create a chat completion", "gpt short \"What is 2 + 2?\"");
    help.print();
}

struct Params {
    bool print_raw_json = false;
    bool store_completion = false;
    std::optional<std::string> prompt = std::nullopt;
    std::optional<std::string> temperature = std::nullopt;
};

void read_cli(int argc, char **argv, Params &params)
{
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
                help::exit_on_failure();
        }
    }

    for (int i = optind; i < argc; i++) {
        if (strcmp("short", argv[i]) != 0) {
            params.prompt = argv[i];
            break;
        }
    }
}

} // namespace

void command_short(int argc, char **argv)
{
    Params params;
    read_cli(argc, argv, params);

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

    const std::string model = select_chat_model();
    ChatCompletion cc = create_chat_completion(params.prompt.value(), model, temperature, params.store_completion);

    if (params.print_raw_json) {
        fmt::print("{}\n", cc.raw_response);
        return;
    }

    fmt::print("{}\n", cc.completion);
}
