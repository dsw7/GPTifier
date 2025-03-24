#include "command_models.hpp"

#include "cli.hpp"
#include "serialization/models.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <string>
#include <vector>

namespace {

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
    ParamsModels params = cli::get_opts_models(argc, argv);
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
