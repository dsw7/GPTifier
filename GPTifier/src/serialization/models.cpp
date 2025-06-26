#include "models.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"
#include "utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

bool is_owned_by_openai(const std::string &owned_by)
{
    return owned_by.compare(0, 6, "openai") == 0 or owned_by.compare(0, 6, "system") == 0;
}

void unpack_models(const nlohmann::json &json, Models &models)
{
    for (const auto &entry: json["data"]) {
        Model model;
        model.created_at = entry["created"];
        model.id = entry["id"];
        model.owned_by_openai = is_owned_by_openai(entry["owned_by"]);
        model.owner = entry["owned_by"];
        models.models.push_back(model);
    }
}

Models unpack_response(const std::string &response)
{
    const nlohmann::json json = response_to_json(response);
    Models models;

    try {
        unpack_models(json, models);
    } catch (nlohmann::json::out_of_range &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    } catch (nlohmann::json::type_error &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return models;
}

} // namespace

Models get_models()
{
    const std::string response = networking::get_models();
    Models models = unpack_response(response);
    models.raw_response = response;
    return models;
}

bool delete_model(const std::string &model_id)
{
    networking::OpenAIUser api;
    const std::string response = api.delete_model(model_id);
    const nlohmann::json json = response_to_json(response);

    if (not json.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return json["deleted"];
}

} // namespace serialization
