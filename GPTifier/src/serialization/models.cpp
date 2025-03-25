#include "serialization/models.hpp"

#include "networking/api_openai_admin.hpp"
#include "networking/api_openai_user.hpp"
#include "serialization/parse_response.hpp"
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

std::map<std::string, std::string> resolve_users_from_ids()
{
    OpenAIAdmin api;

    std::string response;
    std::map<std::string, std::string> users;

    try {
        response = api.get_users();
    } catch (const std::runtime_error &e) {
        return users;
    }

    const nlohmann::json results = parse_response(response);
    validation::is_list(results);

    for (const auto &user: results["data"]) {
        validation::is_user(user);
        std::string id = user.at("id");
        str_to_lowercase(id);
        users[id] = user.at("name");
    }

    return users;
}

std::string get_owner(const std::string &id)
{
    static std::map<std::string, std::string> users = resolve_users_from_ids();

    if (users.count(id) > 0) {
        return users[id];
    }

    return "-";
}

void unpack_models(const nlohmann::json &results, Models &models)
{
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
            model.owner = get_owner(entry["owned_by"]);
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

bool delete_model(const std::string &model_id)
{
    OpenAIUser api;
    const std::string response = api.delete_model(model_id);
    const nlohmann::json results = parse_response(response);

    validation::is_model(results);
    return results["deleted"];
}
