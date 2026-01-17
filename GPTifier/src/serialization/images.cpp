#include "images.hpp"

#include "api_openai_user.hpp"
#include "ser_utils.hpp"

#include <fmt/core.h>
#include <json.hpp>

namespace serialization {

namespace {

Image unpack_image_response_(const std::string &response)
{
    const nlohmann::json json = parse_json(response);
    Image image_obj;

    try {
        image_obj.b64_json = json["data"][0]["b64_json"];
        image_obj.created = json["created"];

        if (json["data"][0].contains("revised_prompt")) {
            image_obj.revised_prompt = json["data"][0]["revised_prompt"];
        }
    } catch (const nlohmann::json::exception &e) {
        throw std::runtime_error(fmt::format("Failed to unpack response: {}", e.what()));
    }

    return image_obj;
}

} // namespace

Image create_image(const std::string &model, const std::string &prompt, const std::string &quality, const std::string &style)
{
    const nlohmann::json data = {
        { "model", model },
        { "prompt", prompt },
        { "quality", quality },
        { "response_format", "b64_json" },
        { "size", "1024x1024" },
        { "style", style },
    };

    const auto result = networking::create_image(data.dump());

    if (not result) {
        throw_on_openai_error_response(result.error().response);
    }

    return unpack_image_response_(result->response);
}

} // namespace serialization
