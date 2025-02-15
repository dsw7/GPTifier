#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "models.hpp"
#include "parsers.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

namespace {

bool is_owned_by_openai(const std::string &owned_by)
{
    return owned_by.compare(0, 6, "openai") == 0 or owned_by.compare(0, 6, "system") == 0;
}

void get_openai_models(const json &response, std::vector<models::Model> &models)
{
    for (const auto &entry: response["data"]) {
        if (not is_owned_by_openai(entry["owned_by"])) {
            continue;
        }

        models::Model m;
        m.created_at = entry["created"];
        m.id = entry["id"];
        m.owned_by = entry["owned_by"];
        models.push_back(m);
    }
}

void get_user_models(const json &response, std::vector<models::Model> &models)
{
    for (const auto &entry: response["data"]) {
        if (is_owned_by_openai(entry["owned_by"])) {
            continue;
        }

        models::Model m;
        m.created_at = entry["created"];
        m.id = entry["id"];
        m.owned_by = entry["owned_by"];
        models.push_back(m);
    }
}

void print_models(std::vector<models::Model> &models)
{
    fmt::print("> Number of models: {}\n", models.size());
    print_sep();

    fmt::print("{:<25}{:<35}{}\n", "Creation time", "Owner", "Model ID");
    print_sep();

    models::sort(models);

    for (auto it = models.begin(); it != models.end(); ++it) {
        it->print();
    }

    print_sep();
}

} // namespace

void command_models(int argc, char **argv)
{
    ParamsModels params = cli::get_opts_models(argc, argv);

    Curl curl;
    const std::string response = curl.get_models();

    if (params.print_raw_json) {
        print_raw_response(response);
        return;
    }

    const json results = parse_response(response);
    std::vector<models::Model> models;

    if (params.print_user_models) {
        get_user_models(results, models);
    } else {
        get_openai_models(results, models);
    }

    print_models(models);
}
