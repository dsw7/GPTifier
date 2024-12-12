#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "json.hpp"
#include "reporting.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <map>
#include <string>

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
    fmt::print("{:<40}{:<30}{}\n", "Model ID", "Owner", "Creation time");
    reporting::print_sep();

    for (auto it = models.begin(); it != models.end(); ++it) {
        const std::string datetime = datetime_from_unix_timestamp(it->first);
        fmt::print("{:<40}{:<30}{}\n", it->second.id, it->second.owned_by, datetime);
    }
}

void print_models_response(const std::string &response)
{
    nlohmann::json results = nlohmann::json::parse(response);

    if (results.contains("error")) {
        const std::string error = results["error"]["message"];
        reporting::print_error(error);
        return;
    }

    std::vector<UserModel> ft_models = {};
    std::map<int, OpenAIModel> models = {};

    for (const auto &entry: results["data"]) {
        if (is_fine_tuning_model(entry["id"])) {
            UserModel ft_model;
            ft_model.creation_time = entry["created"];
            ft_model.id = entry["id"];
            ft_model.owned_by = entry["owned_by"];
        } else {
            OpenAIModel model;
            model.id = entry["id"];
            model.owned_by = entry["owned_by"];
            models[entry["created"]] = model;
        }
    }

    print_openai_models(models);

    if (not ft_models.empty()) {
        reporting::print_sep();
    }

    reporting::print_sep();
}

} // namespace

void command_models(int argc, char **argv)
{
    cli::get_opts_models(argc, argv);

    const std::string response = query_models_api();
    print_models_response(response);
}
