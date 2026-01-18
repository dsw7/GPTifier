#include "models.hpp"

#include "api_openai_user.hpp"
#include "ser_utils.hpp"

#include <fmt/core.h>
#include <json.hpp>
#include <stdexcept>

namespace serialization {

namespace {

bool is_model_owned_by_openai_(const std::string &owned_by)
{
    if (owned_by.compare(0, 6, "openai") == 0) {
        return true;
    }

    if (owned_by.compare(0, 6, "system") == 0) {
        return true;
    }

    return false;
}

Model unpack_model_object_(const nlohmann::json &entry)
{
    Model model_object;
    model_object.created_at = entry["created"];
    model_object.created_at_dt_str = datetime_from_unix_timestamp(entry["created"]);
    model_object.id = entry["id"];
    model_object.owned_by_openai = is_model_owned_by_openai_(entry["owned_by"]);
    model_object.owner = entry["owned_by"];
    return model_object;
}

Models unpack_models_response_(const std::string &response)
{
    const nlohmann::json json = parse_json(response);

    Models models;
    models.raw_response = response;

    try {
        for (const auto &entry: json["data"]) {
            models.models.push_back(unpack_model_object_(entry));
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
        throw_on_openai_error_response(result.error().response);
    }

    return unpack_models_response_(result->response);
}

bool delete_model(const std::string &model_id)
{
    const auto result = networking::delete_model(model_id);

    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    const nlohmann::json json = parse_json(result->response);

    if (not json.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return json["deleted"];
}

} // namespace serialization
