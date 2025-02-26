#include "validation.hpp"

#include <stdexcept>

namespace {

void is_openai_response(const nlohmann::json &json)
{
    if (not json.contains("object")) {
        throw std::runtime_error("Missing 'object' key. Is this an OpenAI response?");
    }
}

bool compare_first_object_in_list(const nlohmann::json &json, const std::string &object)
{
    if (json["object"] != "list") {
        return false;
    }

    if (not json.contains("data")) {
        throw std::runtime_error("Missing 'data' key. Is this an OpenAI list?");
    }

    return json["data"][0]["object"] == object;
}

} // namespace

namespace validation {

bool is_chat_completion(const nlohmann::json &json)
{
    is_openai_response(json);
    return json["object"] == "chat.completion";
}

bool is_embedding_list(const nlohmann::json &json)
{
    is_openai_response(json);
    return compare_first_object_in_list(json, "embedding");
}

bool is_model_list(const nlohmann::json &json)
{
    is_openai_response(json);
    return compare_first_object_in_list(json, "model");
}

bool is_file_list(const nlohmann::json &json)
{
    is_openai_response(json);
    return compare_first_object_in_list(json, "file");
}

bool is_users_list(const nlohmann::json &json)
{
    is_openai_response(json);
    return compare_first_object_in_list(json, "organization.user");
}

} // namespace validation
