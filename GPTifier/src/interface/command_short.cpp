#include "interface/command_short.hpp"

#include "interface/help_messages.hpp"
#include "interface/params.hpp"
#include "selectors.hpp"
#include "serialization/chat_completions.hpp"

#include <fmt/core.h>
#include <getopt.h>
#include <string>

namespace {

ParamsShort read_cli(int argc, char **argv)
{
    ParamsShort params;

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
                cli::help_command_short();
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
                cli::exit_on_failure();
        }
    }

    for (int i = optind; i < argc; i++) {
        if (strcmp("short", argv[i]) != 0) {
            params.prompt = argv[i];
            break;
        }
    }

    params.sanitize();
    return params;
}

} // namespace

void command_short(int argc, char **argv)
{
    ParamsShort params = read_cli(argc, argv);

    float temperature = 1.00;
    if (std::holds_alternative<float>(params.temperature)) {
        temperature = std::get<float>(params.temperature);
    }

    const std::string model = select_chat_model();
    ChatCompletion cc = create_chat_completion(params.prompt.value(), model, temperature, params.store_completion);

    if (params.print_raw_json) {
        fmt::print("{}\n", cc.raw_response);
        return;
    }

    fmt::print("{}\n", cc.completion);
}
