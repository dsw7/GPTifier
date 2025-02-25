#include "validation.hpp"

#include <stdexcept>

namespace {

void is_openai_response(const nlohmann::json &json)
{
    if (not json.contains("object")) {
        throw std::runtime_error("Missing 'object' key. Is this an OpenAI response?");
    }
}

bool is_list(const nlohmann::json &json)
{
    return json["object"] == "list";
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

    if (not is_list(json)) {
        return false;
    }

    return json["data"][0]["object"] == "embedding";
}

bool is_model_list(const nlohmann::json &json)
{
    is_openai_response(json);

    if (not is_list(json)) {
        return false;
    }

    return json["data"][0]["object"] == "model";
}

bool is_file_list(const nlohmann::json &json)
{
    is_openai_response(json);

    if (not is_list(json)) {
        return false;
    }

    return json["data"][0]["object"] == "file";
}

} // namespace validation
