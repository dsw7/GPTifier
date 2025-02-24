#include "command_models.hpp"

#include "api.hpp"
#include "cli.hpp"
#include "models.hpp"
#include "parsers.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <map>
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

void resolve_users_from_ids(std::map<std::string, std::string> &users)
{
    OpenAIAdmin api;
    std::string response;

    try {
        response = api.get_users();
    } catch (const std::runtime_error &e) {
        return;
    }

    const json results = parse_response(response);

    if (not results.contains("data")) {
        return;
    }

    for (auto user = results["data"].begin(); user != results["data"].end(); user++) {
        std::string id = user->at("id");
        str_to_lowercase(id);
        users[id] = user->at("name");
    }
}

void get_user_models(const json &response, std::vector<models::Model> &models)
{
    std::map<std::string, std::string> users;
    resolve_users_from_ids(users);

    for (const auto &entry: response["data"]) {
        if (is_owned_by_openai(entry["owned_by"])) {
            continue;
        }

        models::Model m;

        if (users.count(entry["owned_by"]) > 0) {
            m.owned_by = users[entry["owned_by"]];
        } else {
            m.owned_by = "-";
        }

        m.created_at = entry["created"];
        m.id = entry["id"];
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

    OpenAI curl;
    const std::string response = curl.get_models();
    const json results = parse_response(response);

    if (params.print_raw_json) {
        fmt::print("{}\n", results.dump(4));
        return;
    }

    std::vector<models::Model> models;

    if (params.print_user_models) {
        get_user_models(results, models);
    } else {
        get_openai_models(results, models);
    }

    print_models(models);
}
