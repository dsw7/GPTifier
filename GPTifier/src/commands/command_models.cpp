#include "command_models.hpp"

#include "models.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <getopt.h>
#include <iterator>
#include <string>
#include <utility>
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

using serialization::Model;
using VecModels = std::vector<Model>;

void print_models_(const VecModels &models)
{
    fmt::print("Number of models: {}\n\n", models.size());
    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");

    for (const auto &model: models) {
        fmt::print("{:<25}{:<35}{}\n", model.created_at_dt_str, model.owner, model.id);
    }
}

} // namespace

namespace commands {

void command_models(const int argc, char **argv)
{
    const Parameters params = read_cli_(argc, argv);
    const serialization::Models response = serialization::get_models();

    if (params.print_raw_json) {
        fmt::print("{}\n", response.raw_response);
        return;
    }

    VecModels models = std::move(response.models);
    VecModels filtered_models;

    std::copy_if(
        models.begin(), models.end(), std::back_inserter(filtered_models),
        [params](const Model &model) {
            if (params.print_user_models) {
                return not model.owned_by_openai;
            }

            return model.owned_by_openai;
        });

    std::sort(filtered_models.begin(), filtered_models.end());
    print_models_(filtered_models);
}

} // namespace commands
