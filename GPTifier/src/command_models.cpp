#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "json.hpp"
#include "parsers.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <algorithm>
#include <fmt/core.h>
#include <string>
#include <vector>

namespace {

struct OpenAIModel {
    int created;
    std::string id;
    std::string owned_by;
};

bool is_fine_tuning_model(const std::string &model)
{
    return model.compare(0, 3, "ft:") == 0;
}

void print_models(std::vector<OpenAIModel> &models)
{
    reporting::print_sep();
    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");
    reporting::print_sep();

    std::sort(models.begin(), models.end(), [](const OpenAIModel &a, const OpenAIModel &b) {
        if (a.created != b.created) {
            return a.created < b.created;
        }

        return a.id < b.id;
    });

    for (auto it = models.begin(); it != models.end(); ++it) {
        const std::string datetime = datetime_from_unix_timestamp(it->created);
        fmt::print("{:<25}{:<35}{}\n", datetime, it->owned_by, it->id);
    }

    reporting::print_sep();
}

void print_models_response(const std::string &response)
{
    const nlohmann::json results = parse_response(response);

    std::vector<OpenAIModel> openai_models = {};
    std::vector<OpenAIModel> user_models = {};

    for (const auto &entry: results["data"]) {
        if (is_fine_tuning_model(entry["id"])) {
            OpenAIModel model;
            model.created = entry["created"];
            model.id = entry["id"];
            model.owned_by = entry["owned_by"];
            user_models.push_back(model);
        } else {
            OpenAIModel model;
            model.created = entry["created"];
            model.id = entry["id"];
            model.owned_by = entry["owned_by"];
            openai_models.push_back(model);
        }
    }

    fmt::print("> OpenAI models:\n");
    fmt::print("> Number of models: {}\n", openai_models.size());
    print_models(openai_models);

    if (not user_models.empty()) {
        fmt::print("\n> User models:\n");
        fmt::print("> Number of models: {}\n", user_models.size());
        print_models(user_models);
    }
}

} // namespace

void command_models(int argc, char **argv)
{
    bool print_raw = cli::get_opts_models(argc, argv);
    const std::string response = api::get_models();

    if (print_raw) {
        print_raw_response(response);
        return;
    }

    print_models_response(response);
}
