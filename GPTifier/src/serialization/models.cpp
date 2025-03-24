#include "serialization/models.hpp"

#include "networking/api_openai_admin.hpp"
#include "networking/api_openai_user.hpp"
#include "parsers.hpp"
#include "utils.hpp"
#include "validation.hpp"

#include <map>
#include <stdexcept>
#include <string>

namespace {

bool is_owned_by_openai(const std::string &owned_by)
{
    return owned_by.compare(0, 6, "openai") == 0 or owned_by.compare(0, 6, "system") == 0;
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

    const nlohmann::json results = parse_response(response);
    validation::is_list(results);

    for (const auto &user: results["data"]) {
        validation::is_user(user);
        std::string id = user.at("id");
        str_to_lowercase(id);
        users[id] = user.at("name");
    }
}

void unpack_models(const nlohmann::json &results, Models &models)
{
    std::map<std::string, std::string> users;
    resolve_users_from_ids(users);

    for (const auto &entry: results["data"]) {
        validation::is_model(entry);

        Model model;
        model.created_at = entry["created"];
        model.id = entry["id"];

        if (is_owned_by_openai(entry["owned_by"])) {
            model.owned_by_openai = true;
            model.owner = entry["owned_by"];
        } else {
            model.owned_by_openai = false;

            if (users.count(entry["owned_by"]) > 0) {
                model.owner = users[entry["owned_by"]];
            } else {
                model.owner = "-";
            }
        }

        models.models.push_back(model);
    }
}

} // namespace

Models get_models()
{
    OpenAIUser api;
    const std::string response = api.get_models();
    const nlohmann::json results = parse_response(response);

    validation::is_list(results);

    Models models;
    models.raw_response = response;

    unpack_models(results, models);
    return models;
}
