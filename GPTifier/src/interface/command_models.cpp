#include "interface/command_models.hpp"

#include "cli.hpp"
#include "help_messages.hpp"
#include "params.hpp"
#include "serialization/models.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <getopt.h>
#include <string>
#include <vector>

namespace {

ParamsModels read_cli(int argc, char **argv)
{
    ParamsModels params;

    while (true) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "json", no_argument, 0, 'j' },
            { "user", no_argument, 0, 'u' },
            { 0, 0, 0, 0 }
        };

        int option_index = 0;
        int c = getopt_long(argc, argv, "hju", long_options, &option_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                cli::help_command_models();
                exit(EXIT_SUCCESS);
            case 'j':
                params.print_raw_json = true;
                break;
            case 'u':
                params.print_user_models = true;
                break;
            default:
                cli::exit_on_failure();
        }
    }

    return params;
}

void get_openai_models(const std::vector<Model> &left, std::vector<Model> &right)
{
    for (const auto &it: left) {
        if (it.owned_by_openai) {
            right.push_back(it);
        }
    }
}

void get_user_models(const std::vector<Model> &left, std::vector<Model> &right)
{
    for (const auto &it: left) {
        if (not it.owned_by_openai) {
            right.push_back(it);
        }
    }
}

void print_models(std::vector<Model> &models)
{
    std::sort(models.begin(), models.end(), [](const Model &left, const Model &right) {
        return left.created_at < right.created_at;
    });

    fmt::print("> Number of models: {}\n", models.size());
    print_sep();

    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");
    print_sep();

    for (const auto &it: models) {
        const std::string dt_created_at = datetime_from_unix_timestamp(it.created_at);
        fmt::print("{:<25}{:<35}{}\n", dt_created_at, it.owner, it.id);
    }

    print_sep();
}

} // namespace

void command_models(int argc, char **argv)
{
    ParamsModels params = read_cli(argc, argv);
    Models models = get_models();

    if (params.print_raw_json) {
        fmt::print("{}\n", models.raw_response);
        return;
    }

    std::vector<Model> filtered_models;

    if (params.print_user_models) {
        get_user_models(models.models, filtered_models);
    } else {
        get_openai_models(models.models, filtered_models);
    }

    print_models(filtered_models);
}
