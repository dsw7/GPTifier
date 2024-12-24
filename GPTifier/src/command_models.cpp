#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "json.hpp"
#include "parsers.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <string>
#include <utility>
#include <vector>

namespace {

struct OpenAIModel {
    std::string id;
    std::string owned_by;
};

typedef std::vector<std::pair<int, OpenAIModel>> vec_models;

bool is_fine_tuning_model(const std::string &model)
{
    return model.compare(0, 3, "ft:") == 0;
}

void print_models(const vec_models &models)
{
    reporting::print_sep();
    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");
    reporting::print_sep();

    for (auto it = models.begin(); it != models.end(); ++it) {
        const std::string datetime = datetime_from_unix_timestamp(it->first);
        fmt::print("{:<25}{:<35}{}\n", datetime, it->second.owned_by, it->second.id);
    }

    reporting::print_sep();
}

void print_models_response(const std::string &response)
{
    const nlohmann::json results = parse_response(response);

    vec_models openai_models = {};
    vec_models user_models = {};

    for (const auto &entry: results["data"]) {
        if (is_fine_tuning_model(entry["id"])) {
            OpenAIModel model;
            model.id = entry["id"];
            model.owned_by = entry["owned_by"];
            user_models.push_back(std::make_pair(entry["created"], model));
        } else {
            OpenAIModel model;
            model.id = entry["id"];
            model.owned_by = entry["owned_by"];
            openai_models.push_back(std::make_pair(entry["created"], model));
        }
    }

    fmt::print("> OpenAI models:\n");
    print_models(openai_models);

    if (not user_models.empty()) {
        fmt::print("\n> User models:\n");
        print_models(user_models);
    }
}

} // namespace

void command_models(int argc, char **argv)
{
    cli::get_opts_models(argc, argv);

    const std::string response = query_models_api();
    print_models_response(response);
}
