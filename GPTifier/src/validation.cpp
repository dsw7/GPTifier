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

bool is_chat_completion(const nlohmann::json &json)
{
    is_openai_response(json);
    return json["object"] == "chat.completion";
}

bool is_embedding(const nlohmann::json &json)
{
    is_openai_response(json);
    return json["object"] == "embedding";
}

} // namespace validation
