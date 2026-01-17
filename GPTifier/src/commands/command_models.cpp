#include "command_models.hpp"

#include "models.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <getopt.h>
#include <string>
#include <vector>

namespace {

void help_models_()
{
    const std::string messages = R"(List available OpenAI or user models.

Usage:
  gpt models [OPTIONS]

Options:
  -h, --help  Print help information and exit
  -j, --json  Print raw JSON response from OpenAI
  -u, --user  List user models if they exist. Command defaults to listing OpenAI owned models
)";

    fmt::print("{}\n", messages);
}

struct Parameters {
    bool print_raw_json = false;
    bool print_user_models = false;
};

Parameters read_cli_(const int argc, char **argv)
{
    Parameters params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "user", no_argument, 0, 'u' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        const int c = getopt_long(argc, argv, "hju", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                help_models_();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 'u':
                params.print_user_models = true;
                break;
            default:
                utils::exit_on_failure();
        }
    }

    return params;
}

using VecModels = std::vector<serialization::Model>;

VecModels get_openai_models_(const VecModels &all_models)
{
    VecModels openai_models;

    for (const auto &model: all_models) {
        if (model.owned_by_openai) {
            openai_models.push_back(model);
        }
    }

    return openai_models;
}

VecModels get_user_models_(const VecModels &all_models)
{
    VecModels user_models;

    for (const auto &model: all_models) {
        if (not model.owned_by_openai) {
            user_models.push_back(model);
        }
    }

    return user_models;
}

void print_models_(const VecModels &models)
{
    fmt::print("Number of models: {}\n\n", models.size());
    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");

    for (const auto &model: models) {
        const std::string dt_created_at = utils::datetime_from_unix_timestamp(model.created_at);
        fmt::print("{:<25}{:<35}{}\n", dt_created_at, model.owner, model.id);
    }
}

} // namespace

namespace commands {

void command_models(const int argc, char **argv)
{
    const Parameters params = read_cli_(argc, argv);

    serialization::Models all_models = serialization::get_models();
    std::sort(all_models.models.begin(), all_models.models.end());

    if (params.print_raw_json) {
        fmt::print("{}\n", all_models.raw_response);
        return;
    }

    VecModels filtered_models;

    if (params.print_user_models) {
        filtered_models = get_user_models_(all_models.models);
    } else {
        filtered_models = get_openai_models_(all_models.models);
    }

    print_models_(filtered_models);
}

} // namespace commands
