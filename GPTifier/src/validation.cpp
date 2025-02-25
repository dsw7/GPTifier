#include "validation.hpp"

#include <stdexcept>

namespace {

void is_openai_response(const nlohmann::json &json)
{
    if (not json.contains("object")) {
        throw std::runtime_error("Missing 'object' key. Is this an OpenAI response?");
    }
}

} // namespace

namespace validation {

void is_chat_completion(const nlohmann::json &json)
{
    is_openai_response(json);

    if (json["object"] != "chat.completion") {
        throw std::runtime_error("Is this OpenAI response a valid chat completion?");
    }
}

} // namespace validation
