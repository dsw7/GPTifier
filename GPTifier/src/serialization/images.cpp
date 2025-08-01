#include "images.hpp"

#include "api_openai_user.hpp"
#include "response_to_json.hpp"

#include <json.hpp>

namespace serialization {

namespace {

void unpack_image(const nlohmann::json &json, Image &image)
{
    image.b64_json = json["data"][0]["b64_json"];
    image.created = json["created"];

    if (json["data"][0].contains("revised_prompt")) {
        image.revised_prompt = json["data"][0]["revised_prompt"];
    }
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
    const std::string response = networking::create_image(data.dump());
    return unpack_response<Image>(response, unpack_image);
}

} // namespace serialization
