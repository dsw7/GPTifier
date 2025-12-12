#include "models.hpp"

#include "api_openai_user.hpp"
#include "ser_utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

bool is_owned_by_openai(const std::string &owned_by)
{
    return owned_by.compare(0, 6, "openai") == 0 or owned_by.compare(0, 6, "system") == 0;
}

Models unpack_models(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    Models models;
    models.raw_response = response;

    try {
        for (const auto &entry: json["data"]) {
            Model model;
            model.created_at = entry["created"];
            model.id = entry["id"];
            model.owned_by_openai = is_owned_by_openai(entry["owned_by"]);
            model.owner = entry["owned_by"];
            models.models.push_back(model);
        }
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return models;
}

} // namespace

Models get_models()
{
    const auto result = networking::get_models();

    if (not result) {
        throw_on_error_response(result.error().response);
    }

    return unpack_models(result->response);
}

bool delete_model(const std::string &model_id)
{
    const auto result = networking::delete_model(model_id);

    if (not result) {
        throw_on_error_response(result.error().response);
    }

    const nlohmann::json json = parse_json(result->response);

    if (not json.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return json["deleted"];
}

} // namespace serialization
