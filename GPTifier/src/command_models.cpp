#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "json.hpp"
#include "parsers.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <map>
#include <string>
#include <vector>

namespace {

struct OpenAIModel {
    std::string id;
    std::string owned_by;
};

struct UserModel {
    int creation_time;
    std::string id;
    std::string owned_by;
};

bool is_fine_tuning_model(const std::string &model)
{
    return model.compare(0, 3, "ft:") == 0;
}

void print_openai_models(const std::map<int, OpenAIModel> &models)
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

void print_user_models(const std::vector<UserModel> &models)
{
    reporting::print_sep();
    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");
    reporting::print_sep();

    for (auto it = models.begin(); it != models.end(); ++it) {
        const std::string datetime = datetime_from_unix_timestamp(it->creation_time);
        fmt::print("{:<25}{:<35}{}\n", datetime, it->owned_by, it->id);
    }

    reporting::print_sep();
}

void print_models_response(const std::string &response)
{
    const nlohmann::json results = parse_response(response);

    std::vector<UserModel> user_models = {};
    std::map<int, OpenAIModel> openai_models = {};

    for (const auto &entry: results["data"]) {
        if (is_fine_tuning_model(entry["id"])) {
            UserModel model;
            model.creation_time = entry["created"];
            model.id = entry["id"];
            model.owned_by = entry["owned_by"];
            user_models.push_back(model);
        } else {
            OpenAIModel model;
            model.id = entry["id"];
            model.owned_by = entry["owned_by"];
            openai_models[entry["created"]] = model;
        }
    }

    fmt::print("> OpenAI models:\n");
    print_openai_models(openai_models);

    if (not user_models.empty()) {
        fmt::print("\n> User models:\n");
        print_user_models(user_models);
    }
}

} // namespace

void command_models(int argc, char **argv)
{
    cli::get_opts_models(argc, argv);

    const std::string response = query_models_api();
    print_models_response(response);
}
