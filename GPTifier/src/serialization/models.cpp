#include "models.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

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
        throw std::runtime_error(fmt::format("Failed to unpack models response: {}", e.what()));
    }

    return models;
}

} // namespace

ModelResult get_models()
{
    const auto result = networking::get_models();

    if (result.has_value()) {
        return unpack_models(result.value().response);
    }

    return std::unexpected(unpack_error(result.error().response, result.error().code));
}

bool delete_model(const std::string &model_id)
{
    const std::string response = networking::delete_model(model_id);
    const nlohmann::json json = response_to_json(response);

    if (not json.contains("deleted")) {
        throw std::runtime_error("Malformed response. Missing 'deleted' key");
    }

    return json["deleted"];
}

} // namespace serialization
